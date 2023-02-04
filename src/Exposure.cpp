#include "Exposure.h"

#include <math.h>
#include <WString.h>

#include "common_strings.h"

float log2(float x)
{
  return log(x) / log(2);
}

float meterValueToExposureValue(int raw, float calibrationConstant)
{
  return log2(raw) + calibrationConstant;
}

float cameraSettingsToExposureValue(ShutterSpeed shutter, ApertureValue aperture, ISOValue iso)
{
  auto shutterFloat = shutterSpeedAsFloat(shutter);
  auto apertureFloat = apertureValueAsFloat(aperture);
  auto isoFloat = isoValueAsFloat(iso);

  return log2((apertureFloat * apertureFloat) / shutterFloat) - log2(isoFloat / 100.);
}

float shutterSpeedAsFloat(ShutterSpeed shutter)
{
  switch(shutter) {
    case ShutterSpeed::_2 : {
      return 1./2.;
    } case ShutterSpeed::_4 : {
      return 1./4.;
    } case ShutterSpeed::_8 : {
      return 1./8.;
    } case ShutterSpeed::_15 : {
      return 1./15.;
    } case ShutterSpeed::_30 : {
      return 1./30.;
    } case ShutterSpeed::_60 : {
      return 1./60.;
    } case ShutterSpeed::_125 : {
      return 1./125.;
    } case ShutterSpeed::_250 : {
      return 1./250.;
    } case ShutterSpeed::_500 : {
      return 1./500.;
    } case ShutterSpeed::_1000 :
      default: {
      return 1./1000.;
    }
  }
}

float apertureValueAsFloat(ApertureValue aperture)
{
  switch(aperture) {
    case ApertureValue::_1_4 : {
      return 1.4;
    } case ApertureValue::_2 : {
      return 2.;
    } case ApertureValue::_2_8 : {
      return 2.8;
    } case ApertureValue::_4 : {
      return 4.;
    } case ApertureValue::_5_6 : {
      return 5.6;
    } case ApertureValue::_8 : {
      return 8.;
    } case ApertureValue::_11 : {
      return 11.;
    } case ApertureValue::_16 : {
      return 16.;
    } case ApertureValue::_22 :
      default: {
      return 22.;
    }
  }
}

float isoValueAsFloat(ISOValue iso)
{
  switch(iso) {
    case ISOValue::_100 : {
      return 100.;
    } case ISOValue::_200 : {
      return 200.;
    } case ISOValue::_400 : {
      return 400.;
    } case ISOValue::_800 : {
      return 800.;
    } case ISOValue::_1600 :
      default: {
      return 1600.;
    }
  }
}

void apertureValueAsString(ApertureValue aperture, char* s, uint8_t n)
{
  switch(aperture) {
    case ApertureValue::_1_4 : {
      strncpy_P(s, PSTR("1.4"), n);
      break;
    } case ApertureValue::_2 : {
      strncpy_P(s, PSTR("2"), n);
      break;
    } case ApertureValue::_2_8 : {
      strncpy_P(s, PSTR("2.8"), n);
      break;
    } case ApertureValue::_4 : {
      strncpy_P(s, PSTR("4"), n);
      break;
    } case ApertureValue::_5_6 : {
      strncpy_P(s, PSTR("5.6"), n);
      break;
    } case ApertureValue::_8 : {
      strncpy_P(s, PSTR("8"), n);
      break;
    } case ApertureValue::_11 : {
      strncpy_P(s, PSTR("11"), n);
      break;
    } case ApertureValue::_16 : {
      strncpy_P(s, PSTR("16"), n);
      break;
    } case ApertureValue::_22 : {
      strncpy_P(s, PSTR("22"), n);
      break;
    } default: {
      strncpy_P(s, UNKNOWN_STR, n);
      break;
    }
  }

  s[n-1] = '\0';
}

void shutterSpeedAsString(ShutterSpeed shutter, char* s, uint8_t n)
{
  uint16_t value;

  switch(shutter) {
    case ShutterSpeed::_2 : {
      value = 2;
      break;
    } case ShutterSpeed::_4 : {
      value = 4;
      break;
    } case ShutterSpeed::_8 : {
      value = 8;
      break;
    } case ShutterSpeed::_15 : {
      value = 15;
      break;
    } case ShutterSpeed::_30 : {
      value = 30;
      break;
    } case ShutterSpeed::_60 : {
      value = 60;
      break;
    } case ShutterSpeed::_125 : {
      value = 125;
      break;
    } case ShutterSpeed::_250 : {
      value = 250;
      break;
    } case ShutterSpeed::_500 : {
      value = 500;
      break;
    } case ShutterSpeed::_1000 : {
      value = 1000;
      break;
    } default: {
      strncpy_P(s, UNKNOWN_STR, n);
      s[n-1] = '\0';
      return;
    }
  }

  itoa(value, s, 10);

  s[n-1] = '\0';
}

void isoValueAsString(ISOValue iso, char* s, uint8_t n)
{
  uint16_t value;

  switch(iso) {
    case ISOValue::_100 : {
      value = 100;
      break;
    } case ISOValue::_200 : {
      value = 200;
      break;
    } case ISOValue::_400 : {
      value = 400;
      break;
    } case ISOValue::_800 : {
      value = 800;
      break;
    } case ISOValue::_1600 : {
      value = 1600;
      break;
    } default: {
      strncpy_P(s, UNKNOWN_STR, n);
      s[n-1] = '\0';
      return;
    }
  }

  itoa(value, s, 10);

  s[n-1] = '\0';
}
