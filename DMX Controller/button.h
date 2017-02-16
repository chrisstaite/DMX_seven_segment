//
//  button.h
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>

namespace avr {

class Pin;

/// An encapsulation around a pin to de-bounce a button
/// input on that pin
class Button
{
  public:
    /// Number of identical reads to confirm on/off action
    static constexpr int8_t DEBOUNCE_COUNT = 4;
  
    /// Construct a button around a pin
    ///
    /// \param pin  The pin to read the button input from
    Button(Pin& pin);

    /// Determines whether the button has been pressed,
    /// after the debouncing has occurred
    ///
    /// \return  True if pressed, false if released
    bool pressed();
    
  private:
    /// The pin the the button is read from
    Pin& m_pin;
    /// The number of times the button has been recorded
    /// in the value in m_lastState, counted positive for
    /// high and negative for low
    int8_t m_lastStateCount;
};

}  // namespace avr

#endif  // BUTTON_H_
