//
//  controller.h
//  DMX Controller
//
//  Created by Chris Staite on 12/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdint.h>

namespace dmx {
class Dmx;
}  // namespace dmx

namespace led {

class SevenSegmentArray;

class Controller
{
  public:
    Controller(dmx::Dmx& dmx, SevenSegmentArray& display);

    void setModifierChannel(uint8_t channel);

    uint8_t& red();
    
    uint8_t& green();
    
    uint8_t& blue();
    
    void increase();
    
    void decrease();

  private:
    dmx::Dmx& m_dmx;
    SevenSegmentArray& m_display;
    uint8_t m_currentChannel;
};

}  // namespace led

#endif  // CONTROLLER_H_
