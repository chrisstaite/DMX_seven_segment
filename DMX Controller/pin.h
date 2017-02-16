//
//  pin.h
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

namespace avr {

/// An encapsulation around an AVR pin
class Pin
{
  public:
    Pin(volatile uint8_t& ddr, volatile uint8_t& port,
        volatile uint8_t& pin, uint8_t mask);
  
    /// Set the pin to input mode
    void input();
    
    /// Set the pin to output mode
    void output();

    /// Set the value on the pin
    ///
    /// \param value  True to set high, false to set low
    ///
    /// \return  A reference to this instance
    Pin& operator=(bool value);
    
    /// Get the value on this pin
    ///
    /// \return  True for high, false for low
    operator bool() const;

    /// Get the mask for this pin
    ///
    /// \return  The mask for this pin
    uint8_t mask() const;
    
    /// Get the port for this pin
    ///
    /// \return  The port for this pin
    volatile uint8_t& port();
    
    /// Get the port for this pin
    ///
    /// \return  The port for this pin
    volatile uint8_t& pin();
    
    /// Get the pin for this pin
    ///
    /// \return  The value for this pin
    volatile uint8_t pin() const;
    
    /// Get the ddr for this pin
    ///
    /// \return  The ddr for this pin
    volatile uint8_t& ddr();
    
  private:
    volatile uint8_t& m_ddr;
    volatile uint8_t& m_port;
    volatile uint8_t& m_pin;
    uint8_t m_mask;
};

}  // namespace avr

#endif  // PIN_H_
