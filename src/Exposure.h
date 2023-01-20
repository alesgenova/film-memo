#ifndef Exposure_h
#define Exposure_h

#include <stdint.h>

enum class ShutterSpeed {
  None,
  // _1,
  _2,
  _4,
  _8,
  _15,
  _30,
  _60,
  _125,
  _250,
  _500,
  _1000,
  // _2000
};



enum class ApertureValue {
  None,
  _1_4,
  _2,
  _2_8,
  _4,
  _5_6,
  _8,
  _11,
  _16,
  _22,
  // _32
};

enum class ISOValue {
  None,
  // _50,
  _100,
  _200,
  _400,
  _800,
  _1600,
  // _3200,
};

float shutterSpeedAsFloat(ShutterSpeed shutter);
float apertureValueAsFloat(ApertureValue aperture);
float isoValueAsFloat(ISOValue iso);

void apertureValueAsString(ApertureValue aperture, char* s, uint8_t n);
void shutterSpeedAsString(ShutterSpeed shutter, char* s, uint8_t n);
void isoValueAsString(ISOValue iso, char* s, uint8_t n);

float meterValueToExposureValue(int raw);

float cameraSettingsToExposureValue(ShutterSpeed shutter, ApertureValue aperture, ISOValue iso);

#endif
