//
//  timed_event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef TIMED_EVENT_H_
#define TIMED_EVENT_H_

#include <stdint.h>

namespace avr {

class Event;

class TimedEvent
{
  public:
    TimedEvent(Event& event);

    void reset(uint16_t timeout);

    void cancel();

    bool tick();
    
    bool active() const;
    
  private:
    uint16_t m_timeout;
    Event& m_event;
};

}  // namespace avr

#endif  // TIMED_EVENT_H_
