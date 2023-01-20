#ifndef PinStreamINT_h
#define PinStreamINT_h

#include "PinStream.h"

class PinStreamINT : public BasePinStream {
public:
  PinStreamINT(uint8_t pin, IOEvent eventsBuffer[], uint8_t bufferSize);
  virtual ~PinStreamINT();

  void onChange();

protected:
  uint8_t m_pin;
};

#endif
