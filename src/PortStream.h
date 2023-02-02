#ifndef PortStream_h
#define PortStream_h

#include <stdint.h>

#include "common.h"

struct PortListener {
  uint8_t pinMask = 0;
  void* boundObj = nullptr;
  void (*callback) (void* boundObj, const IOEvent&) = nullptr;
};

const uint8_t MAX_LISTENERS = 8;

class PortStream {
public:
  PortStream(uint8_t portMask, uint8_t portRegister, volatile uint8_t& activePinRegister);
  ~PortStream();

  volatile uint8_t& activePinRegister();

  void pushEvent(const IOEvent& event);

  void onChange();

  int8_t registerListener(uint8_t pinMask, void* boundObj, void (*fn) (void* boundObj, const IOEvent&));
  void unregisterListener(int8_t listenerId);

protected:
  uint8_t m_portMask;
  uint8_t m_portRegister;
  uint8_t m_lastState = 0;
  volatile uint8_t& m_activePinRegister;
  PortListener m_listeners[MAX_LISTENERS];
};

#endif
