//
//  seven_segment_array.h
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef SEVEN_SEGMENT_ARRAY_H_
#define SEVEN_SEGMENT_ARRAY_H_

#include "seven_segment.h"

namespace avr {
class Pin;
}  // namespace avr

namespace led {

/// An encapsulation around a three digit 7-segment display
/// with common cathode, operating under the same stipulations
/// as set out in SevenSegment.
/// \see SevenSegment
class SevenSegmentArray
{
  public:
    /// The number of digits in the display
    static constexpr unsigned int DIGITS = 3;
    /// The nubmer of frames per second
    static constexpr unsigned int FPS = 50;
    /// The number of calls to displayStep per second
    static constexpr unsigned int STEPS_PS = FPS * DIGITS * SevenSegment::SEQUENCE_STEPS;

    /// Construct the encapsulation, setting the pins as outputs
    ///
    /// \param digitSelectors  The selectors for each of the three digits
    /// \param digitPort       The DDR for the 7-segment output
    ///
    /// \see SevenSegment
    template<typename Port>
    SevenSegmentArray(avr::Pin (&digitSelectors)[DIGITS], Port digitPort);

    SevenSegmentArray(const SevenSegmentArray&) = delete;
    SevenSegmentArray& operator=(const SevenSegmentArray&) = delete;

    ~SevenSegmentArray();

    /// Set the current value to display.  A negative value
    /// blanks the display.
    ///
    /// The value is only actually displayed if displayStep
    /// is called.
    ///
    /// \param value  The value to display
    void setValue(int16_t value);

    /// Set an individual display value, should be between
    /// 0-9 or a value from SevenSegmentArray::Letters
    ///
    /// \param index  The index between 0 and DIGITS
    /// \param value  The value to display
    void setValue(uint8_t index, uint8_t value);

    /// Display the next bit, this wants to be performed roughly
    /// 50fps for the DIGITS * SEQUENCE_STEPS long sequence.  This
    /// equates to about 600 times a second (STEPSPS).
    void displayStep();

  private:
    /// The constructor, untemplated
    void init();

    avr::Pin (&m_digitSelectors)[DIGITS];
    SevenSegment m_segment;
    uint8_t m_currentValues[DIGITS];
    uint8_t m_currentStep;
};

template<typename Port>
SevenSegmentArray::SevenSegmentArray(
        avr::Pin (&digitSelectors)[DIGITS],
        Port digitPort) :
    m_digitSelectors{digitSelectors},
    m_segment{digitPort},
    m_currentValues{},
    m_currentStep{0}
{
    setValue(-1);
    init();
}

}  // namespace led

#endif  // SEVEN_SEGMENT_ARRAY_H_
