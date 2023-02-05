#include "Roll.h"

#include <WString.h>

#include "common_strings.h"

RollManufacturer Roll::manufacturer() const
{
  return m_manufacturer;
}

void Roll::setManufacturer(RollManufacturer manufacturer)
{
  m_manufacturer = manufacturer;
}

ISOValue Roll::iso() const
{
  return m_iso;
}

void Roll::setIso(ISOValue iso)
{
  m_iso = iso;
}


void rollManufacturerAsString(RollManufacturer manufacturer, char* s, uint8_t n)
{
  switch(manufacturer) {
    case RollManufacturer::Fuji : {
      strncpy_P(s, PSTR("Fuji"), n);
      break;
    } case RollManufacturer::Kodak : {
      strncpy_P(s, PSTR("Kodak"), n);
      break;
    } case RollManufacturer::Other : {
      strncpy_P(s, PSTR("Other"), n);
      break;
    } default: {
      strncpy_P(s, UNKNOWN_STR, n);
    }
  }

  s[n-1] = '\0';
}

void Roll::asString(uint8_t i, char* s, uint8_t n)
{
  uint8_t p = 0;

  strncpy_P(s, PSTR("#"), n);
  p += 1;

  {
    char tmp[4];
    itoa(i, tmp, 10);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), PSTR(" "), n - p);
  p += 1;

  auto manufacturer = this->manufacturer();

  if (manufacturer == RollManufacturer::None) {
    strncpy_P((s + p), PSTR("<empty>"), n - p);
    return;
  }

  {
    char tmp[6];
    rollManufacturerAsString(manufacturer, tmp, 6);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), PSTR(" "), n - p);
  p += 1;

  {
    char tmp[5];
    isoValueAsString(this->iso(), tmp, 5);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }
}
