//
//  ms_timer.cpp
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "ms_timer.h"

#include <avr/interrupt.h>
#include <stdint.h>

namespace {

avr::MsTimer* s_timer;

}  // anon namespace

// Overflow for Timer2 every millisecond (ish)
ISR(TIMER2_COMP_vect)
{
    s_timer->tick();
}

namespace avr {

namespace {

// The number of cycles that each clock tick uses
constexpr uint32_t CYCLES = 4;

// Calculate the number we need to match against for each of
// the available pre-scalers in order to get 1ms resolution
constexpr uint32_t TARGET_COUNTER         = F_CPU / (1000/ CYCLES);
constexpr uint32_t TARGET_COUNTER_DIV8    = TARGET_COUNTER / 8;
constexpr uint32_t TARGET_COUNTER_DIV64   = TARGET_COUNTER / 64;
constexpr uint32_t TARGET_COUNTER_DIV256  = TARGET_COUNTER / 256;
constexpr uint32_t TARGET_COUNTER_DIV1024 = TARGET_COUNTER / 1024;

// The maximum comparisson value we can have on the timer
constexpr uint32_t MAX_COUNTER = 0xff;

/// Get the timer scale that doesn't overflow
constexpr uint8_t bestScale()
{
    return
        (TARGET_COUNTER        <= MAX_COUNTER ?  (1 << CS20)                :
        (TARGET_COUNTER_DIV8   <= MAX_COUNTER ?  (1 << CS21)                :
        (TARGET_COUNTER_DIV64  <= MAX_COUNTER ? ((1 << CS20) | (1 << CS21)) :
        (TARGET_COUNTER_DIV256 <= MAX_COUNTER ?  (1 << CS22)                :
                                                ((1 << CS22) | (1 << CS20))
        ))));
}

/// Get the best comparisson value that matches the pre-scale
/// from bestScale()
constexpr uint8_t bestCompare()
{
    static_assert(TARGET_COUNTER_DIV1024 <= MAX_COUNTER,
                  "The clock is too fast for millisecond accuracy");
    return
        (TARGET_COUNTER        <= MAX_COUNTER ? TARGET_COUNTER         :
        (TARGET_COUNTER_DIV8   <= MAX_COUNTER ? TARGET_COUNTER_DIV8    :
        (TARGET_COUNTER_DIV64  <= MAX_COUNTER ? TARGET_COUNTER_DIV64   :
        (TARGET_COUNTER_DIV256 <= MAX_COUNTER ? TARGET_COUNTER_DIV256  :
                                                TARGET_COUNTER_DIV1024
        ))));
}

}  // anon namespace

MsTimer::MsTimer() :
    m_millis{0}
{
    s_timer = this;
    // Configure Timer2 for system tick, as close to 1ms as possible
    // Set the pre-scaler to System Clock (8Mhz) / 256 = 31.25kHz
    TCCR2 = bestScale() | (1 << WGM21);  // Interrupt on OCR2 match
    // Configure the Timer2 compare interrupt
    TIFR = (1 << TOV2) | (1 << OCF2);  // Clear interrupt flags
    OCR2 = bestCompare();  // Set the compare value
    TIMSK |= (1 << OCIE2);  // Enable the compare match interrupt
}

MsTimer::~MsTimer()
{
    TIMSK &= ~(1 << OCIE2);  // Disable the compare match interrupt
    s_timer = nullptr;
}

void MsTimer::tick()
{
    ++m_millis;
}

uint16_t MsTimer::millis() const
{
    return m_millis;
}

}  // namespace avr
