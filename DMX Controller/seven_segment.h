//
//  seven_segment.h
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef SEVEN_SEGMENT_H_
#define SEVEN_SEGMENT_H_

#include <stdint.h>

namespace led {

/// Encapsulation around a 7-segment display
/// with the following stipulations:
///  - Display pins a-g on one port
///  - dp on pin 7
///  - A on pin 6, B on pin 5 etc.
class SevenSegment
{
  public:
    /// The number of sequence steps to call value() with
    static constexpr unsigned int SEQUENCE_STEPS = 4;

    /// Construct an encapsulation of a 7-segment display
    ///
    /// \param port  The port that the 7-segment is on
    template<typename Port>
    SevenSegment(Port port);

    SevenSegment(const SevenSegment&) = delete;
    SevenSegment& operator=(const SevenSegment&) = delete;
    
    enum Letters : uint8_t
    {
        Blank = 10,
        A,
        C,
        E,
        H,
        L,
        U,
        MAX = U
    };

    /// Display part of a digit in order to limit the amount
    /// of current sunk by the common cathode pin.  Since a
    /// pin is not recommended to sink more than 20mA and we
    /// assume that each segment uses 8mA, we only turn on
    /// 2 segments at a time.  A seven segment is therefore
    /// turned on in a sequence of four.
    ///
    /// \param digit     The digit to display, 0-9 or a value
    ///                  from Letters - unchecked
    /// \param sequence  A value between 0 and 3 - unchecked
    void value(uint8_t digit, uint8_t sequence);

    /// Display the decimal point for this digit.  This should
    /// only be used when sequence is 3 or the digit 10 is set
    /// (i.e. it's blank) due to current draw.  However, in
    /// practice it's probably ok.
    ///
    /// \param on  True to turn it on, false for off
    void dp(bool on);

  private:
    volatile uint8_t& m_port;
};

template<typename Port>
SevenSegment::SevenSegment(Port port) :
    m_port(Port::port())
{
    // Set pins as output
    Port::ddr() = ~0;
}

}  // namespace led

#endif  // SEVEN_SEGMENT_H_
