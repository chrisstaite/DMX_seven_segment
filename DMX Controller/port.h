//
//  port.h
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef PORT_H_
#define PORT_H_

#include "avr_io_cpp.h"
#include <stdint.h>

namespace avr {

template<typename PORT, typename DDR, typename PIN>
class Port
{
  public:
    static constexpr PORT port()
    {
        return {};
    }
    
    static constexpr DDR ddr()
    {
        return {};
    }
    
    static constexpr PIN pin()
    {
        return {};
    }
};

class PortA : public Port<AVR_IOR_PARAM(PORTA),
                          AVR_IOR_PARAM(DDRA),
                          AVR_IOR_PARAM(PINA)>
{ };

class PortB : public Port<AVR_IOR_PARAM(PORTB),
                          AVR_IOR_PARAM(DDRB),
                          AVR_IOR_PARAM(PINB)>
{ };

class PortC : public Port<AVR_IOR_PARAM(PORTC),
                          AVR_IOR_PARAM(DDRC),
                          AVR_IOR_PARAM(PINC)>
{ };

class PortD : public Port<AVR_IOR_PARAM(PORTD),
                          AVR_IOR_PARAM(DDRD),
                          AVR_IOR_PARAM(PIND)>
{ };

}  // namespace avr

#endif  // PORT_H_
