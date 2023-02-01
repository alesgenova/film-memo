#include "Frame.h"

#include <WString.h>

void focalLengthAsString(FocalLength focal, char* s, uint8_t n)
{
  switch(focal) {
    case FocalLength::_20 : {
      strncpy_P(s, PSTR("20"), n);
      break;
    } case FocalLength::_24 : {
      strncpy_P(s, PSTR("24"), n);
      break;
    } case FocalLength::_28 : {
      strncpy_P(s, PSTR("28"), n);
      break;
    } case FocalLength::_35 : {
      strncpy_P(s, PSTR("35"), n);
      break;
    } case FocalLength::_40 : {
      strncpy_P(s, PSTR("40"), n);
      break;
    } case FocalLength::_50 : {
      strncpy_P(s, PSTR("50"), n);
      break;
    } case FocalLength::_85 : {
      strncpy_P(s, PSTR("85"), n);
      break;
    } case FocalLength::_135 : {
      strncpy_P(s, PSTR("135"), n);
      break;
    } case FocalLength::None : {
      strncpy_P(s, PSTR("None"), n);
      break;
    } default: {
      strncpy_P(s, PSTR("Unk"), n);
    }
  }

  s[n-1] = '\0';
}

ApertureValue Frame::aperture() const
{
  return m_aperture;
}

void Frame::setAperture(ApertureValue aperture)
{
  m_aperture = aperture;
}

ShutterSpeed Frame::shutter() const
{
  return m_shutter;
}

void Frame::setShutter(ShutterSpeed shutter)
{
  m_shutter = shutter;
}

FocalLength Frame::focal() const
{
  return m_focal;
}

void Frame::setFocal(FocalLength focal)
{
  m_focal = focal;
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

  auto focal = this->focal();

  strncpy_P((s + p), PSTR(" "), n - p);
  p += 1;

  {
    char tmp[4];
    focalLengthAsString(focal, tmp, 4);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), PSTR("mm"), n - p);
  p += 2;
}
