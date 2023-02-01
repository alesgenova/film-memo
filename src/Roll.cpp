#include "Roll.h"

#include <WString.h>

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
      strncpy_P(s, (const char*)F("Fuji\0"), n);
      break;
    } case RollManufacturer::Kodak : {
      strncpy_P(s, (const char*)F("Kodak\0"), n);
      break;
    } case RollManufacturer::Other : {
      strncpy_P(s, (const char*)F("Other\0"), n);
      break;
    } case RollManufacturer::None : {
      strncpy_P(s, (const char*)F("None\0"), n);
      break;
    } default: {
      strncpy_P(s, (const char*)F("Unk\0"), n);
    }
  }

  s[n-1] = '\0';
}

void Roll::asString(uint8_t i, char* s, uint8_t n)
{
  uint8_t p = 0;

  strncpy_P(s, (const char*)F(" #"), n);
  p += 2;

  {
    char tmp[4];
    itoa(i, tmp, 10);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), (const char*)F(" "), n - p);
  p += 1;

  auto manufacturer = this->manufacturer();

  if (manufacturer == RollManufacturer::None) {
    strncpy_P((s + p), (const char*)F("<empty>"), n - p);
    return;
  }

  {
    char tmp[6];
    rollManufacturerAsString(manufacturer, tmp, 6);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), (const char*)F(" "), n - p);
  p += 1;

  {
    char tmp[5];
    isoValueAsString(this->iso(), tmp, 5);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }
}
