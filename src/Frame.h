#ifndef Frame_h
#define Frame_h

#include "Exposure.h"

class Frame {
public:
  Frame(){}
  ApertureValue aperture() const;
  void setAperture(ApertureValue aperture);

  ShutterSpeed shutter() const;
  void setShutter(ShutterSpeed shutter);

  void asString(uint8_t i, char* s, uint8_t n);

protected:
  uint8_t m_data = 0;
};

#endif
