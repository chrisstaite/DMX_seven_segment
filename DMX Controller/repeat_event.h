//
//  repeat_event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef REPEAT_EVENT_H_
#define REPEAT_EVENT_H_

#include "event.h"

#include <stdint.h>

namespace avr {

template<typename T, void (T::*Function)()>
class RepeatEvent : public Event
{
  public:
    /// The number of milliseconds for a repeat button press
    static constexpr uint16_t REPEAT_TIME = 100;

    RepeatEvent(T& instance) :
        m_instance{instance}
    { }

    void fire(TimedEvent& timer)
    {
        (m_instance.*Function)();
        timer.reset(REPEAT_TIME);
    }

  private:
    T& m_instance;
};

}  // namespace avr

#endif  // REPEAT_EVENT_H_
