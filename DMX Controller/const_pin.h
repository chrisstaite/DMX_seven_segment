//
//  const_pin.h
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef CONST_PIN_H_
#define CONST_PIN_H_

#include "pin.h"

namespace avr {

/// An encapsulation around an AVR pin
template<typename Port, unsigned int N>
class ConstPin
{
  public:
    ConstPin()
    {
        static_assert(N < 8, "Only 8 pins on a port!");
    }
  
    /// Set the pin to input mode
    void input();
    
    /// Set the pin to output mode
    void output();

    /// Set the value on the pin
    ///
    /// \param value  True to set high, false to set low
    ///
    /// \return  A reference to this instance
    ConstPin& operator=(bool value);
    
    /// Get the value on this pin
    ///
    /// \return  True for high, false for low
    operator bool() const;

    /// Get the mask for this pin
    ///
    /// \return  The mask for this pin
    static constexpr uint8_t mask();
    
    constexpr volatile uint8_t& port() const;
    constexpr volatile uint8_t& pin() const;
    constexpr volatile uint8_t& ddr() const;
    
    /// Create a Pin instance
    ///
    /// \return  A pin instance
    static Pin toPin();
};

template<typename Port, unsigned int N>
void ConstPin<Port, N>::input()
{
    Port::ddr() |= mask();
}

template<typename Port, unsigned int N>
void ConstPin<Port, N>::output()
{
    Port::ddr() &= ~mask();
}

template<typename Port, unsigned int N>
ConstPin<Port, N>& ConstPin<Port, N>::operator=(bool value)
{
    if (value)
    {
        Port::port() |= mask();
    }
    else
    {
        Port::port() &= ~mask();
    }
    return *this;
}

template<typename Port, unsigned int N>
ConstPin<Port, N>::operator bool() const
{
    return Port::pin & mask();
}

template<typename Port, unsigned int N>
constexpr uint8_t ConstPin<Port, N>::mask()
{
    return (1 << N);
}

template<typename Port, unsigned int N>
constexpr volatile uint8_t& ConstPin<Port, N>::port() const
{
    return Port::port();
}

template<typename Port, unsigned int N>
constexpr volatile uint8_t& ConstPin<Port, N>::pin() const
{
    return Port::pin;
}

template<typename Port, unsigned int N>
constexpr volatile uint8_t& ConstPin<Port, N>::ddr() const
{
    return Port::ddr;
}

template<typename Port, unsigned int N>
Pin ConstPin<Port, N>::toPin()
{
    return Pin{Port::ddr(), Port::port(), Port::pin(), mask()};
}

}  // namespace avr

#endif  // CONST_PIN_H_
