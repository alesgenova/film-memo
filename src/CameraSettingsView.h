#ifndef CameraSettingsView_h
#define CameraSettingsView_h

#include "constants.h"

class CameraSettingsView {
public:
  CameraSettingsView();
  virtual ~CameraSettingsView();

  uint8_t position() const;
  void setPosition(uint8_t y);

  uint8_t columns() const;
  void setColumns(uint8_t cols);

  void setLabel(uint8_t col, const char* label, bool invert = false);
  void setValue(uint8_t col, const char* value);

protected:
  void drawSeparators();

  static uint8_t columnWidth(uint8_t n);
  static uint8_t columnStart(uint8_t col, uint8_t n);

  uint8_t m_y = 0;
  uint8_t m_cols = 3;
};

#endif
