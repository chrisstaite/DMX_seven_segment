//
//  dmx.h
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef DMX_H_
#define DMX_H_

#include "const_pin.h"
#include <stdint.h>

namespace dmx {
    
class Dmx
{
  public:
    /// The number of channels to transmit (32 - 512)
    static constexpr int16_t MAX_CHANNEL = 32;
  
    /// Construct an instance that can talk to WS2812 chips
    /// on a given pin
    ///
    /// \param pin  The pin to talk to the WS2812 on
    template<typename P, unsigned int N>
    Dmx(avr::ConstPin<P, N> pin);

    Dmx();

    Dmx(const Dmx&) = delete;
    Dmx& operator=(const Dmx&) = delete;

    void setChannel(uint16_t channel, uint8_t value);
    uint8_t& getChannel(uint16_t channel);
    
    void txInterrupt();
    void udreInterrupt();
    
    uint8_t& operator[] (uint16_t channel);

  private:
    void initTx();
    void writeByte(uint8_t byte);
    void setBaud(uint16_t baudSetting, uint8_t format);

    uint8_t m_data[MAX_CHANNEL];
    int16_t m_currentChannel;
};

template<typename P, unsigned int N>
Dmx::Dmx(avr::ConstPin<P, N> pin) :
    Dmx{}
{
    pin.output();
    pin = true;  // Recieve mode
}

inline uint8_t& Dmx::operator[] (uint16_t channel)
{
    return getChannel(channel);
}

}  // namespace dmx

#endif  // DMX_H_
