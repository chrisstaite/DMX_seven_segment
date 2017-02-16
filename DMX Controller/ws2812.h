//
//  ws2812.h
//  DMX Controller
//
//  Created by Chris Staite on 09/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef WS2812_H_
#define WS2812_H_

#include "const_pin.h"
#include "port.h"
#include <stdint.h>

namespace led {

/// Encapsulation of RGB value in WS2812 order
struct RGB
{
    RGB(uint8_t r, uint8_t g, uint8_t b) :
        g{g},
        r{r},
        b{b}
    { }

    uint8_t g;
    uint8_t r;
    uint8_t b;
};

static_assert(sizeof(RGB) == 3, "RGB struct must not be padded");

/// Class to control a string of WS2812 chips connected
/// together
class WS2812
{
  public:
    /// Construct an instance that can talk to WS2812 chips
    /// on a given pin
    ///
    /// \param pin  The pin to talk to the WS2812 on
    template<typename P, unsigned int N>
    WS2812(avr::ConstPin<P, N> pin);

    WS2812(const WS2812&) = delete;
    WS2812& operator=(const WS2812&) = delete;

    /// Set a number of WS2812 LEDs in a string to a set of values
    ///
    /// NOTE: Do not call more than once every 300uS
    ///
    /// \param values  An array of values to set
    /// \param count   The length of the values array
    void sendRgb(RGB* values, uint16_t count);

  private:
    void sendRgb(uint8_t* values, uint16_t count);

    uint8_t m_mask;
    volatile uint8_t& m_port;
};

template<typename P, unsigned int N>
WS2812::WS2812(avr::ConstPin<P, N> pin) :
    m_mask{pin.mask()},
    m_port{pin.port()}
{ }

}  // namespace led

#endif  // WS2812_H_
