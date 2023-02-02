#ifndef CameraSettingsView_h
#define CameraSettingsView_h

#include "constants.h"

class CameraSettingsView {
public:
  CameraSettingsView();
  virtual ~CameraSettingsView();

  uint8_t position() const;
  void setPosition(uint8_t y);

  void setLabel(uint8_t col, const char* label, bool invert = false);
  void setValue(uint8_t col, const char* value);

protected:
  void drawSeparators();

  uint8_t m_y = 0;
};

#endif
