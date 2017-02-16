//
//  event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef EVENT_H_
#define EVENT_H_

namespace avr {

class TimedEvent;

class Event
{
  public:
    virtual void fire(TimedEvent&) = 0;
};

}  // namespace avr

#endif  // EVENT_H_
