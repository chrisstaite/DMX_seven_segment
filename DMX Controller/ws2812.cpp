//
//  ws2812.cpp
//  DMX Controller
//
//  Created by Chris Staite on 09/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "ws2812.h"
#include "pin.h"

#include <avr/io.h>
#include <avr/interrupt.h>

namespace led {

namespace {

// This routine writes an array of bytes with RGB values to the Dataout pin
// using the fast 800kHz clockless WS2811/2812 protocol.

// Timing in ns
constexpr int32_t ZERO_PULSE_NS   = 350;
constexpr int32_t ONE_PULSE_NS    = 900;
constexpr int32_t TOTAL_PERIOD_NS = 1250;

// The number of cycles used by the instructions for
// each bit of the loop
constexpr int32_t LOW_CYCLES_USED   = 3;
constexpr int32_t HIGH_CYCLES_USED  = 6;
constexpr int32_t TOTAL_CYCLES_USED = 10;

// Calculate the number of cycles we should be emitting each pulse for
constexpr int32_t ZERO_CYCLES  = ((F_CPU / 1000) *   ZERO_PULSE_NS         ) / 1000000;
// Round up
constexpr int32_t ONE_CYCLES   = ((F_CPU / 1000) *    ONE_PULSE_NS + 500000) / 1000000;
// Round up
constexpr int32_t TOTAL_CYCLES = ((F_CPU / 1000) * TOTAL_PERIOD_NS + 500000) / 1000000;

// Calculate the difference between the cycles used and the optimum
// number of cycles for each stage, ignoring negative numbers
constexpr int32_t ZERO_CYCLES_NOP =
    (ZERO_CYCLES - LOW_CYCLES_USED < 0) ? 0 : (ZERO_CYCLES - LOW_CYCLES_USED);
constexpr int32_t ONE_CYCLES_NOP =
    (ONE_CYCLES - HIGH_CYCLES_USED - (ZERO_CYCLES - LOW_CYCLES_USED) < 0) ? 0 :
    (ONE_CYCLES - HIGH_CYCLES_USED - (ZERO_CYCLES - LOW_CYCLES_USED));
constexpr int32_t FINAL_CYCLES_NOP =
    (TOTAL_CYCLES - TOTAL_CYCLES_USED - (ONE_CYCLES - HIGH_CYCLES_USED) < 0) ? 0 :
    (TOTAL_CYCLES - TOTAL_CYCLES_USED - (ONE_CYCLES - HIGH_CYCLES_USED));

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
static_assert(((ZERO_CYCLES_NOP + LOW_CYCLES_USED) * 1000000) / (F_CPU / 1000) <= 550,
              "The clock speed is too low for the WS2812");

// Here's a nice little template to produce our NOP's based on
// the values we just calculated...
template<int32_t N>
inline void nop()
{
    nop<N-2>();
    nop<2>();
}

template<>
inline void nop<0>()
{
}

template<>
inline void nop<1>()
{
    asm volatile("nop\n\t");
}

template<>
inline void nop<2>()
{
    asm volatile("rjmp .+0\n\t");
}

}  // anon namespace

void WS2812::sendRgb(RGB* values, uint16_t count)
{
    sendRgb(reinterpret_cast<uint8_t*>(values), count * sizeof(RGB));
}

void WS2812::sendRgb(uint8_t* values, uint16_t count)
{
    // Counter for the 8 bits in the byte
    uint8_t ctr;

    // Calculate the masks
    volatile uint8_t* port = &m_port;
    uint8_t masklo = *port & ~m_mask;
    uint8_t maskhi = *port |  m_mask;

    // Save the interrupt state and disable whether
    // it's set or not...
    uint8_t sreg_prev = SREG;
    cli();

    while (count--)
    {
        // One line only to evaluate to ldi rn, Z+
        // Does not get optimized to that on two lines...
        uint8_t curbyte = *values++;

        asm volatile(
            "ldi %0, 8\n\t"
            "WS2812loop:\n\t"
            "st X, %3\n\t"
            : "=&d" (ctr)
            : "r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<ZERO_CYCLES_NOP>();
        asm volatile(
            "sbrs %1, 7\n\t"
            "st X, %4\n\t"
            "lsl %1\n\t"
            : "=&d" (ctr)
            : "r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<ONE_CYCLES_NOP>();
        asm volatile(
            "brcc skipone%=\n\t"
            "st X, %4\n\t"
            "skipone%=:"
            :	"=&d" (ctr)
            :	"r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
        nop<FINAL_CYCLES_NOP>();
        asm volatile(
            "dec   %0\n\t"
            "brne  WS2812loop\n\t"
            :	"=&d" (ctr)
            :	"r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
        );
    }

    // Re-enable interrupts if they were already enabled
    SREG = sreg_prev;
}

}  // namespace led
