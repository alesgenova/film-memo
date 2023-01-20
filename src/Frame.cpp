#include "Frame.h"

#include <WString.h>

const uint8_t APERTURE_MASK = 0b00001111;
const uint8_t SHUTTER_MASK  = 0b11110000;

ApertureValue Frame::aperture() const
{
  return (ApertureValue)(m_data & APERTURE_MASK);
}

void Frame::setAperture(ApertureValue aperture)
{
  m_data = m_data & SHUTTER_MASK;
  m_data = m_data ^ (uint8_t)aperture;
}

ShutterSpeed Frame::shutter() const
{
  return (ShutterSpeed)((m_data & SHUTTER_MASK) >> 4);
}

void Frame::setShutter(ShutterSpeed shutter)
{
  m_data = m_data & APERTURE_MASK;
  m_data = m_data ^ ((uint8_t)shutter << 4);
}

void Frame::asString(uint8_t i, char* s, uint8_t n)
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

  strncpy_P((s + p), (const char*)F(" f"), n - p);
  p += 2;

  auto aperture = this->aperture();

  if (aperture == ApertureValue::None) {
    strncpy_P((s + p - 1), (const char*)F("<empty>"), n - p);
    return;
  }

  {
    char tmp[4];
    apertureValueAsString(aperture, tmp, 4);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  auto shutter = this->shutter();

  strncpy_P((s + p), (const char*)F(" 1/"), n - p);
  p += 3;

  {
    char tmp[5];
    shutterSpeedAsString(shutter, tmp, 5);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }
}
