//
//  seven_segment_array.cpp
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "seven_segment_array.h"

#include "pin.h"

#include <avr/interrupt.h>

namespace {

// We want to call the display roughly 600 times per second
// to get the persistence of vision.
// To get the 8-bit timer to over-flow at this rate we need
// to calculate the number we need to count up to and subtract
// that from the overflow value (0xff).
constexpr uint8_t DISPLAY_TIMER_START =
    0xff - ((F_CPU / 64) / led::SevenSegmentArray::STEPS_PS);

static_assert(
    ((F_CPU / 64) / led::SevenSegmentArray::STEPS_PS) <= 0xff,
    "CPU too fast to do display timing"
);

led::SevenSegmentArray* s_display;

constexpr uint16_t POWERS[] = { 1, 10, 100 };
static_assert(sizeof(POWERS) / sizeof(POWERS[0]) == led::SevenSegmentArray::DIGITS,
              "More powers needed in POWERS array");

}  // anon namespace

// Overflow for Timer0 to update the display
ISR(TIMER0_OVF_vect)
{
    // Re-initialise the counter
    TCNT0 = DISPLAY_TIMER_START;
    // Update the display
    s_display->displayStep();
}

namespace led {

void SevenSegmentArray::init()
{
    for (uint8_t i = 0; i < DIGITS; ++i)
    {
        m_digitSelectors[i].output();
    }
    s_display = this;
    
    // Configure Timer0 for the display
    // Set the pre-scaler to System Clock (8Mhz) / 64 = 125kHz
    TCCR0 = (1 << CS01) | (1 << CS00);
    // Configure the Timer0 overflow interrupt
    TIFR = (1 << TOV0);  // Clear the overflow flag
    TIMSK = (1 << TOIE0);  // Enable the overflow interrupt
    // Initialise the counter
    TCNT0 = DISPLAY_TIMER_START;
}

SevenSegmentArray::~SevenSegmentArray()
{
    s_display = nullptr;
}

void SevenSegmentArray::setValue(int16_t value)
{
    if (value < 0 || value > 999)
    {
        value = -1;
    }

    if (value != m_currentValue)
    {
        m_currentStep = 0;
        m_currentValue = value;
    }
}

void SevenSegmentArray::displayStep()
{
    // If this is the first of a new sequence select the digit
    uint8_t sequenceIndex = m_currentStep % SevenSegment::SEQUENCE_STEPS;
    uint8_t digit = m_currentStep / SevenSegment::SEQUENCE_STEPS;
    if (sequenceIndex == 0)
    {
        for (uint8_t i = 0; i < DIGITS; ++i)
        {
            m_digitSelectors[i] = true;
        }
        m_digitSelectors[digit] = false;
    }

    // Get the digit to display
    uint16_t value;
    uint16_t divisor = POWERS[digit];
    if ((m_currentValue == -1) ||
            (digit > 0 && static_cast<uint16_t>(m_currentValue) < divisor))
    {
        value = 10;
    }
    else
    {
        value = (m_currentValue / divisor) % 10;
    }
    value =8;

    m_segment.value(value, sequenceIndex);

    // Increment the step and reset if the final one
    ++m_currentStep;
    if (m_currentStep == SevenSegment::SEQUENCE_STEPS * DIGITS)
    {
        m_currentStep = 0;
    }
}

}  // namespace led
