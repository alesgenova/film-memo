#ifndef Frame_h
#define Frame_h

#include "Exposure.h"

enum class FocalLength : uint8_t {
  None,
  _20,
  _24,
  _28,
  _35,
  _40,
  _50,
  _85,
  _135,
  Other
};

void focalLengthAsString(FocalLength focal, char* s, uint8_t n);

class Frame {
public:
  Frame(){}
  ApertureValue aperture() const;
  void setAperture(ApertureValue aperture);

  ShutterSpeed shutter() const;
  void setShutter(ShutterSpeed shutter);

  FocalLength focal() const;
  void setFocal(FocalLength focal);

  void asString(uint8_t i, char* s, uint8_t n);

protected:
  ShutterSpeed m_shutter = ShutterSpeed::None;
  ApertureValue m_aperture = ApertureValue::None;
  FocalLength m_focal = FocalLength::None;
};

#endif
