//
//  eeprom.cpp
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#include "eeprom.h"

#include <avr/eeprom.h>

namespace avr {

Eeprom::Eeprom(uint16_t base) :
    m_base{base}
{ }

void Eeprom::write(uint16_t offset, uint8_t value)
{
    eeprom_write_byte(
        reinterpret_cast<uint8_t*>(m_base + offset), value
    );
}

uint8_t Eeprom::read(uint16_t offset) const
{
    return eeprom_read_byte(
        reinterpret_cast<uint8_t*>(m_base + offset)
    );
}

uint16_t Eeprom::readWord(uint16_t offset) const
{
    return eeprom_read_word(
        reinterpret_cast<uint16_t*>(m_base + offset)
    );
}

uint32_t Eeprom::readDword(uint16_t offset) const
{
    return eeprom_read_dword(
        reinterpret_cast<uint32_t*>(m_base + offset)
    );
}

}  // namespace avr
