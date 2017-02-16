//
//  save_event.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef SAVE_EVENT_H_
#define SAVE_EVENT_H_

#include "event.h"

namespace avr {
class Eeprom;
}  // namespace avr

namespace led {

class Controller;

class SaveEvent : public avr::Event
{
  public:
    SaveEvent(avr::Eeprom& eeprom, Controller& controller);

    void fire(avr::TimedEvent& timer);
    
  private:
    avr::Eeprom& m_eeprom;
    Controller& m_controller;
};

}  // namespace led

#endif  // SAVE_EVENT_H_
