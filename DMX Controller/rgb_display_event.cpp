//
//  rgb_display_event.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "rgb_display_event.h"

#include "timed_event.h"
#include "ws2812.h"
#include "controller.h"

namespace led {

RgbDisplayEvent::RgbDisplayEvent(WS2812& display, Controller& controller) :
    m_display{display},
    m_controller{controller}
{ }

void RgbDisplayEvent::fire(avr::TimedEvent& timer)
{
    led::RGB ledColour {m_controller.red(), m_controller.green(), m_controller.blue()};
    m_display.sendRgb(&ledColour, 1);
}

}  // namespace led
