//
//  latched_button.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "latched_button.h"

#include "button.h"
#include "timed_event.h"

namespace led {

LatchedButton::LatchedButton(avr::Button& button, avr::TimedEvent& event) :
    m_button{button},
    m_event{event},
    m_pressed{m_button.pressed()}
{ }

void LatchedButton::test()
{
    if (m_button.pressed())
    {
        if (m_pressed == false)
        {
            m_event.reset(1);
            m_pressed = true;
        }
    }
    else
    {
        if (m_pressed == true)
        {
            m_event.cancel();
            m_pressed = false;
        }
    }
}

}  // namespace led
