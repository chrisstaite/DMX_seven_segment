//
//  button.cpp
//  DMX Controller
//
//  Created by Chris Staite on 08/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "button.h"

#include "pin.h"

namespace avr {

Button::Button(Pin& pin) :
    m_pin{pin},
    m_lastStateCount{0}
{
    m_pin.input();
    m_pin = true;  // Activate pull-up resistor
}

bool Button::pressed()
{
    bool result = (m_pin == false);

    if (result)
    {
        if (m_lastStateCount > 0)
        {
            m_lastStateCount = 0;
        }
        else if (m_lastStateCount > -DEBOUNCE_COUNT)
        {
            --m_lastStateCount;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        if (m_lastStateCount < 0)
        {
            m_lastStateCount = 0;
        }
        else if (m_lastStateCount < DEBOUNCE_COUNT)
        {
            ++m_lastStateCount;
        }
        else
        {
            result = true;
        }
    }

    return result;
}

}  // namespace avr
