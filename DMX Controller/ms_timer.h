//
//  ms_timer.h
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef MS_TIMER_H_
#define MS_TIMER_H_

#include <stdint.h>

namespace avr {

/// Encapsulation around the AVR Timer2 in order to
/// attempt to tick every millisecond
///
/// Should only have a single instance
class MsTimer
{
  public:
    MsTimer();
    
    MsTimer(const MsTimer&) = delete;
    MsTimer& operator=(const MsTimer&) = delete;
    
    ~MsTimer();

    void tick();

    uint16_t millis() const;

  private:
    uint16_t m_millis;
};

}  // namespace avr

#endif  // MS_TIMER_H_
