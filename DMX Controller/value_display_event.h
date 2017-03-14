//
//  value_display_event.h
//  DMX Controller
//
//  Created by Chris Staite on 14/03/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef VALUE_DISPLAY_EVENT_H_
#define VALUE_DISPLAY_EVENT_H_

#include "event.h"

namespace led {

class Controller;

class ValueDisplayEvent : public avr::Event
{
  public:
    ValueDisplayEvent(Controller& controller);

    void fire(avr::TimedEvent& timer);

  private:
    Controller& m_controller;
};

}  // namespace led

#endif  // VALUE_DISPLAY_EVENT_H_
