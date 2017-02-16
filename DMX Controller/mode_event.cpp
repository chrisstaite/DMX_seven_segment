//
//  mode_event.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "mode_event.h"

#include "timed_event.h"
#include "controller.h"

namespace led {

ModeEvent::ModeEvent(Controller& controller) :
    m_mode{0},
    m_controller{controller}
{ }

void ModeEvent::fire(avr::TimedEvent& timer)
{
    m_mode = (m_mode + 1) % MAX_MODE;
    m_controller.setModifierChannel(m_mode);
}

uint8_t ModeEvent::mode() const
{
    return m_mode;
}

}  // namespace led
