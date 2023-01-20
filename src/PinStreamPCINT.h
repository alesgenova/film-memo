#ifndef PinStreamPCINT_h
#define PinStreamPCINT_h

#include "PinStream.h"

class PortStream;

class PinStreamPCINT : public BasePinStream {
public:
  PinStreamPCINT(uint8_t pinMaskInPort, PortStream& portStream, IOEvent eventsBuffer[], uint8_t bufferSize);
  virtual ~PinStreamPCINT();

protected:
  static void onPortEvent(void* self, const IOEvent& event);

  uint8_t m_pinMaskInPort;
  PortStream& m_portStream;
  int8_t m_listenerId;
};

#endif
