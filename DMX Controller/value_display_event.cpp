//
//  value_display_event.cpp
//  DMX Controller
//
//  Created by Chris Staite on 14/03/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "value_display_event.h"

#include "timed_event.h"
#include "controller.h"

namespace led {

ValueDisplayEvent::ValueDisplayEvent(Controller& controller) :
    m_controller{controller}
{ }

void ValueDisplayEvent::fire(avr::TimedEvent& timer)
{
    m_controller.showValue();
}

}  // namespace led
