#ifndef MeterView_h
#define MeterView_h

#include "constants.h"

class MeterView {
public:
  MeterView();
  ~MeterView();

  void bounds(uint8_t b[4]) const;
  void setBounds(uint8_t b[4]);

  void setReading(float reading);
protected:
  void drawScale();
  void drawReading();

  uint8_t m_bounds[4] = {0, 0, 128, 64};
  float m_reading = 0;
};

#endif
