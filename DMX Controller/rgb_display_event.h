//
//  rgb_display_event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef RGB_DISPLAY_EVENT_H_
#define RGB_DISPLAY_EVENT_H_

#include "event.h"

#include <stdint.h>

namespace led {

class WS2812;
class Controller;

class RgbDisplayEvent : public avr::Event
{
  public:
    RgbDisplayEvent(WS2812& display, Controller& controller);

    void fire(avr::TimedEvent& timer);
    
  private:
    WS2812& m_display;
    uint8_t& m_red;
    uint8_t& m_green;
    uint8_t& m_blue;
};

}  // namespace led

#endif  // RGB_DISPLAY_EVENT_H_
