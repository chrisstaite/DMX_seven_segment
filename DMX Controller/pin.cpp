//
//  pin.cpp
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "pin.h"

namespace avr {

Pin::Pin(volatile uint8_t& ddr, volatile uint8_t& port,
         volatile uint8_t& pin, uint8_t mask) :
    m_ddr{ddr},
    m_port{port},
    m_pin{pin},
    m_mask{mask}
{ }

void Pin::input()
{
    ddr() &= ~mask();
}

void Pin::output()
{
    ddr() |= mask();
}

Pin& Pin::operator=(bool value)
{
    if (value)
    {
        port() |= mask();
    }
    else
    {
        port() &= ~mask();
    }
    return *this;
}

Pin::operator bool() const
{
    return pin() & mask();
}

uint8_t Pin::mask() const
{
    return m_mask;
}

volatile uint8_t& Pin::port()
{
    return m_port;
}

volatile uint8_t Pin::pin() const
{
    return m_pin;
}

volatile uint8_t& Pin::pin()
{
    return m_pin;
}

volatile uint8_t& Pin::ddr()
{
    return m_ddr;
}

}  // namespace avr
