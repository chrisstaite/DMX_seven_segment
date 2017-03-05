//
//  seven_segment.cpp
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "seven_segment.h"

#include <avr/io.h>

namespace led {

namespace {

// Prepare a value to set to the whole port so that
// the 7-segment display lights up
constexpr uint8_t toValue(
    uint8_t a,
    uint8_t b,
    uint8_t c,
    uint8_t d,
    uint8_t e,
    uint8_t f,
    uint8_t g)
{
    return (a << 6) | (b << 5) | (c << 4) | (d << 3) |
        (e << 2) | (f << 1) | g;
}

constexpr uint8_t NUMBER_SEGMENTS[] = {
  // A  B  C  D  E  F  G
  toValue(1, 1, 1, 1, 1, 1, 0),  // 0
  toValue(0, 1, 1, 0, 0, 0, 0),  // 1
  toValue(1, 1, 0, 1, 1, 0, 1),  // 2
  toValue(1, 1, 1, 1, 0, 0, 1),  // 3
  toValue(0, 1, 1, 0, 0, 1, 1),  // 4
  toValue(1, 0, 1, 1, 0, 1, 1),  // 5
  toValue(1, 0, 1, 1, 1, 1, 1),  // 6
  toValue(1, 1, 1, 0, 0, 0, 0),  // 7
  toValue(1, 1, 1, 1, 1, 1, 1),  // 8
  toValue(1, 1, 1, 0, 0, 1, 1),  // 9
  toValue(0, 0, 0, 0, 0, 0, 0)   // Blank
};

}  // anon namespace

void SevenSegment::value(uint8_t digit, uint8_t sequence)
{
    m_port = NUMBER_SEGMENTS[digit] & (0b11 << (sequence * 2));
}

void SevenSegment::dp(bool on)
{
    if (on)
    {
        m_port |= (1 << 7);
    }
    else
    {
        m_port &= ~(1 << 7);
    }
}

}  // namespace led
