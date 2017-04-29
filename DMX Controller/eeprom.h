//
//  eeprom.h
//  DMX Controller
//
//  Created by Chris Staite on 10/02/2017.
//  Copyright © 2017 Chris Staite. All rights reserved.
//

#ifndef EEPROM_H_
#define EEPROM_H_

#include "stdint.h"

namespace avr {

class EepromByteAccess;

class Eeprom
{
  public:
    Eeprom(uint8_t* base);

    void write(uint16_t offset, uint8_t value);

    uint8_t read(uint16_t offset) const;
    
    uint16_t readWord(uint16_t offset) const;
    
    uint32_t readDword(uint16_t offset) const;

    EepromByteAccess operator[] (uint16_t offset);

  private:
    uint8_t* m_base;
};

class EepromByteAccess
{
  public:
    EepromByteAccess(Eeprom& eeprom, uint16_t offset);
    
    operator uint8_t();
    
    uint8_t operator=(uint8_t value);
    
  private:
    Eeprom& m_eeprom;
    uint16_t m_offset;
};

inline EepromByteAccess Eeprom::operator[] (uint16_t offset)
{
    return EepromByteAccess{*this, offset};
}

inline EepromByteAccess::EepromByteAccess(Eeprom& eeprom, uint16_t offset) :
    m_eeprom{eeprom},
    m_offset{offset}
{ }

inline EepromByteAccess::operator uint8_t()
{
    return m_eeprom.read(m_offset);
}

inline uint8_t EepromByteAccess::operator=(uint8_t value)
{
    m_eeprom.write(m_offset, value);
    return value;
}

}  // namespace avr

#endif  // EEPROM_H_
