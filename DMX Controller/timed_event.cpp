//
//  timed_event.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "timed_event.h"
#include "event.h"

namespace avr {

TimedEvent::TimedEvent(Event& event) :
    m_timeout{0},
    m_event{event}
{ }

void TimedEvent::reset(uint16_t timeout)
{
    if (timeout == 0)
    {
        timeout = 1;
    }
    m_timeout = timeout;
}

void TimedEvent::cancel()
{
    m_timeout = 0;
}

bool TimedEvent::tick()
{
    bool result = false;
    if (m_timeout > 0)
    {
        --m_timeout;
        if (m_timeout == 0)
        {
            m_event.fire(*this);
            result = true;
        }
    }
    return result;
}

bool TimedEvent::active() const
{
    return m_timeout > 0;
}

}  // namespace avr
