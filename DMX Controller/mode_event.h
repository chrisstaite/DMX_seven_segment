//
//  mode_event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef MODE_EVENT_H_
#define MODE_EVENT_H_

#include "event.h"

#include <stdint.h>

namespace led {

class Controller;

class ModeEvent : public avr::Event
{
  public:
    static constexpr uint8_t MAX_MODE = 4;

    ModeEvent(Controller& controller);

    void fire(avr::TimedEvent& timer);
    
    uint8_t mode() const;

  private:
    uint8_t m_mode;
    Controller& m_controller;
};

}  // namespace led

#endif  // MODE_EVENT_H_
