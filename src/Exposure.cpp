#include "Exposure.h"

#include <math.h>
#include <WString.h>

float log2(float x)
{
  return log(x) / log(2);
}

float meterValueToExposureValue(int raw)
{
  const float CALIBRATION_CONSTANT = 6.3;
  return log2(raw) + CALIBRATION_CONSTANT;
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
      strncpy_P(s, (const char*)F("1.4\0"), n);
      break;
    } case ApertureValue::_2 : {
      strncpy_P(s, (const char*)F("2\0"), n);
      break;
    } case ApertureValue::_2_8 : {
      strncpy_P(s, (const char*)F("2.8\0"), n);
      break;
    } case ApertureValue::_4 : {
      strncpy_P(s, (const char*)F("4\0"), n);
      break;
    } case ApertureValue::_5_6 : {
      strncpy_P(s, (const char*)F("5.6\0"), n);
      break;
    } case ApertureValue::_8 : {
      strncpy_P(s, (const char*)F("8\0"), n);
      break;
    } case ApertureValue::_11 : {
      strncpy_P(s, (const char*)F("11\0"), n);
      break;
    } case ApertureValue::_16 : {
      strncpy_P(s, (const char*)F("16\0"), n);
      break;
    } case ApertureValue::_22 : {
      strncpy_P(s, (const char*)F("22\0"), n);
      break;
    } default: {
      strncpy_P(s, (const char*)F("Unk\0"), n);
      break;
    }
  }

  s[n-1] = '\0';
}

void shutterSpeedAsString(ShutterSpeed shutter, char* s, uint8_t n)
{
  switch(shutter) {
    case ShutterSpeed::_2 : {
      strncpy_P(s, (const char*)F("2\0"), n);
      break;
    } case ShutterSpeed::_4 : {
      strncpy_P(s, (const char*)F("4\0"), n);
      break;
    } case ShutterSpeed::_8 : {
      strncpy_P(s, (const char*)F("8\0"), n);
      break;
    } case ShutterSpeed::_15 : {
      strncpy_P(s, (const char*)F("15\0"), n);
      break;
    } case ShutterSpeed::_30 : {
      strncpy_P(s, (const char*)F("30\0"), n);
      break;
    } case ShutterSpeed::_60 : {
      strncpy_P(s, (const char*)F("60\0"), n);
      break;
    } case ShutterSpeed::_125 : {
      strncpy_P(s, (const char*)F("125\0"), n);
      break;
    } case ShutterSpeed::_250 : {
      strncpy_P(s, (const char*)F("250\0"), n);
      break;
    } case ShutterSpeed::_500 : {
      strncpy_P(s, (const char*)F("500\0"), n);
      break;
    } case ShutterSpeed::_1000 : {
      strncpy_P(s, (const char*)F("1000\0"), n);
      break;
    } default: {
      strncpy_P(s, (const char*)F("Unk\0"), n);
      break;
    }
  }

  s[n-1] = '\0';
}

void isoValueAsString(ISOValue iso, char* s, uint8_t n)
{
  switch(iso) {
    case ISOValue::_100 : {
      strncpy_P(s, (const char*)F("100\0"), n);
      break;
    } case ISOValue::_200 : {
      strncpy_P(s, (const char*)F("200\0"), n);
      break;
    } case ISOValue::_400 : {
      strncpy_P(s, (const char*)F("400\0"), n);
      break;
    } case ISOValue::_800 : {
      strncpy_P(s, (const char*)F("800\0"), n);
      break;
    } case ISOValue::_1600 : {
      strncpy_P(s, (const char*)F("1600\0"), n);
      break;
    } default: {
      strncpy_P(s, (const char*)F("Unk\0"), n);
      break;
    }
  }

  s[n-1] = '\0';
}
