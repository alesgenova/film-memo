#include "Frame.h"

#include <WString.h>

#include "common_strings.h"

void focalLengthAsString(FocalLength focal, char* s, uint8_t n)
{
  uint8_t value;

  switch(focal) {
    case FocalLength::_20 : {
      value = 20;
      break;
    } case FocalLength::_24 : {
      value = 24;
      break;
    } case FocalLength::_28 : {
      value = 28;
      break;
    } case FocalLength::_35 : {
      value = 35;
      break;
    } case FocalLength::_40 : {
      value = 40;
      break;
    } case FocalLength::_50 : {
      value = 50;
      break;
    } case FocalLength::_85 : {
      value = 85;
      break;
    } case FocalLength::_135 : {
      value = 135;
      break;
    } case FocalLength::Other : {
      strncpy_P(s, PSTR("Other"), n);
      s[n-1] = '\0';
      return;
    } default: {
      strncpy_P(s, UNKNOWN_STR, n);
      s[n-1] = '\0';
      return;
    }
  }

  itoa(value, s, 10);

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

  strncpy_P(s, PSTR("#"), n);
  p += 1;

  {
    char tmp[4];
    itoa(i, tmp, 10);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  strncpy_P((s + p), PSTR(" f"), n - p);
  p += 2;

  auto aperture = this->aperture();

  if (aperture == ApertureValue::None) {
    strncpy_P((s + p - 1), PSTR("<empty>"), n - p);
    return;
  }

  {
    char tmp[4];
    apertureValueAsString(aperture, tmp, 4);
    strncpy((s + p), tmp, n - p);
    p += strlen(tmp);
  }

  auto shutter = this->shutter();

  strncpy_P((s + p), PSTR(" 1/"), n - p);
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
