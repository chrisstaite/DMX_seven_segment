//
//  save_event.cpp
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "save_event.h"

#include "timed_event.h"
#include "eeprom.h"
#include "controller.h"

namespace led {

SaveEvent::SaveEvent(avr::Eeprom& eeprom, Controller& controller) :
    m_eeprom{eeprom},
    m_controller{controller}
{ }

void SaveEvent::fire(avr::TimedEvent& timer)
{
    m_eeprom[0] = m_controller.red();
    m_eeprom[1] = m_controller.green();
    m_eeprom[2] = m_controller.blue();
}

}  // namespace led
