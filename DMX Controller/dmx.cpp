//
//  dmx.cpp
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "dmx.h"

#include <avr/io.h>
#include <avr/interrupt.h>

namespace {

dmx::Dmx* s_dmx;

}  // anon namespace

// DMX transmit interrupt
ISR(USART_TXC_vect)
{
    s_dmx->txInterrupt();
}

// DMX data empty
ISR(USART_UDRE_vect)
{
    s_dmx->udreInterrupt();
}

namespace dmx {

// calculate prescaler from baud rate and cpu clock rate at compile time
// nb implements rounding of ((clock / 16) / baud) - 1 per atmega datasheet
namespace {

constexpr uint16_t calcPrescale(uint32_t baud)
{
    return (((F_CPU / 8) / baud) - 1) / 2;
}

// the break timing is 10 bits (start + 8 data + parity) of this speed
// the mark-after-break is 1 bit of this speed plus approx 6 usec
// 100000 bit/sec is good: gives 100 usec break and 16 usec MAB
// 1990 spec says transmitter must send >= 92 usec break and >= 12 usec MAB
// receiver must accept 88 us break and 8 us MAB
static constexpr uint32_t BREAK_SPEED = 100000u;
static constexpr uint32_t DMX_SPEED = 250000u;
static constexpr uint8_t BREAK_FORMAT = (0<<USBS) | (2<<UPM0) | (3<<UCSZ0);
static constexpr uint8_t DMX_FORMAT = (1<<USBS) | (2<<UPM0) | (3<<UCSZ0);

}  // anon namespace

Dmx::Dmx() :
    m_data{0},
    m_currentChannel{-1}
{
    s_dmx = this;
    initTx();
}

void Dmx::initTx()
{
    m_currentChannel = 0;
    UCSRB = (1 << TXEN) | (1 << TXCIE);
    setBaud(calcPrescale(BREAK_SPEED), BREAK_FORMAT);
    writeByte(0);
}

void Dmx::setChannel(uint16_t channel, uint8_t value)
{
    if (channel >= 0 && channel < MAX_CHANNEL)
    {
        m_data[channel] = value;
    }
}

uint8_t& Dmx::getChannel(uint16_t channel)
{
    return m_data[channel];
}

void Dmx::writeByte(uint8_t byte)
{
    UDR = byte;
}

void Dmx::txInterrupt()
{
    if (m_currentChannel == -1)
    {
        // this interrupt occurs after the stop bits of the last data byte
        // start sending a BREAK and loop forever in ISR
        setBaud(calcPrescale(BREAK_SPEED), BREAK_FORMAT);
        writeByte(0);
        m_currentChannel = 0;
    }
    else if (m_currentChannel == 0)
    {
        // this interrupt occurs after the stop bits of the break byte
        // now back to DMX speed: 250000baud
        setBaud(calcPrescale(DMX_SPEED), DMX_FORMAT);
        // take next interrupt when data register empty (early)
        UCSRB = (1 << TXEN) | (1 << UDRIE);
        // write start code
        writeByte(0);
        m_currentChannel = 1;
    }
}

// Initialize the Hardware serial port with the given baud rate
// using 8 data bits, no parity, 2 stop bits for data
// and 8 data bits, even parity, 1 stop bit for the break
void Dmx::setBaud(uint16_t baudSetting, uint8_t format)
{
  UCSRA = 0;
  UBRRH = baudSetting >> 8;
  UBRRL = baudSetting;
  UCSRC = format;
}

void Dmx::udreInterrupt()
{
    writeByte(m_data[m_currentChannel]);
    ++m_currentChannel;

    if (m_currentChannel >= MAX_CHANNEL)
    {
        // this series is done. Next time: restart with break.
        m_currentChannel = -1;
        // get interrupt after this byte is actually transmitted
        UCSRB = (1 << TXEN) | (1 << TXCIE);
    }
}

}  // namespace dmx
