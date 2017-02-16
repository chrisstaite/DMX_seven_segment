//
//  controller.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "controller.h"

#include "dmx.h"
#include "seven_segment_array.h"
#include "timed_event.h"

namespace led {

Controller::Controller(dmx::Dmx& dmx, SevenSegmentArray& display) :
    m_dmx{dmx},
    m_display{display},
    m_currentChannel{0}
{
    setModifierChannel(m_currentChannel);
}

void Controller::setModifierChannel(uint8_t channel)
{
    m_currentChannel = channel;

    // Change the current mode
    uint8_t& value = m_dmx[m_currentChannel];
    m_display.setValue(value);
}

void Controller::increase()
{
    uint8_t& value = m_dmx[m_currentChannel];
    if (value < 0xff)
    {
        ++value;
        m_display.setValue(value);
    }
}

void Controller::decrease()
{
    uint8_t& value = m_dmx[m_currentChannel];
    if (value > 0)
    {
        --value;
        m_display.setValue(value);
    }
}

uint8_t& Controller::red()
{
    return m_dmx[0];
}

uint8_t& Controller::green()
{
    return m_dmx[1];
}

uint8_t& Controller::blue()
{
    return m_dmx[2];
}

}  // namespace led
