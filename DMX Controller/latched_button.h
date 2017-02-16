//
//  latched_button.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef LATCHED_BUTTON_H_
#define LATCHED_BUTTON_H_

namespace avr {
class Button;
class TimedEvent;
}  // namespace avr

namespace led {

class LatchedButton
{
  public:
    LatchedButton(avr::Button& button, avr::TimedEvent& event);
    
    void test();
    
  private:
    avr::Button& m_button;
    avr::TimedEvent& m_event;
    bool m_pressed;
};

}  // namespace led

#endif  // LATCHED_BUTTON_H_
