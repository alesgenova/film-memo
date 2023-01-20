#include "PortStream.h"

#include "Arduino.h"

PortStream::PortStream(uint8_t portMask, uint8_t portRegister, volatile uint8_t& activePinRegister)
  : m_portMask(portMask)
  , m_portRegister(portRegister)
  , m_activePinRegister(activePinRegister)
  , m_lastState(*portInputRegister(m_portRegister))
{
  PCICR |= m_portMask; // Activate PC Interrupts on port B
}

PortStream::~PortStream()
{
  PCICR ^= m_portMask;
}

volatile uint8_t& PortStream::activePinRegister()
{
  return m_activePinRegister;
}

void PortStream::pushEvent(const IOEvent& _event)
{
  uint8_t stateChange = m_lastState ^ _event.state;

  m_lastState = _event.state;

  for (uint8_t i = 0; i < MAX_LISTENERS; ++i) {
    auto& listener = m_listeners[i];

    if (stateChange & listener.pinMask) {
      listener.callback(listener.boundObj, _event);
    }
  }
}

void PortStream::onChange()
{
  IOEvent event;
  uint8_t r = (*portInputRegister(m_portRegister));
  event.time = millis();
  event.state = r;

  pushEvent(event);
}

int8_t PortStream::registerListener(uint8_t pinMask, void* boundObj, void (*fn) (void* boundObj, const IOEvent&))
{
  for (uint8_t i = 0; i < MAX_LISTENERS; ++i) {
    auto& listener = m_listeners[i];

    if (listener.pinMask == 0) {
      listener.pinMask = pinMask;
      listener.boundObj = boundObj;
      listener.callback = fn;

      return i;
    }
  }

  return -1;  
}

void PortStream::unregisterListener(int8_t listenerId)
{
  if (listenerId < 0 || listenerId >= MAX_LISTENERS) {
    return;
  }

  auto& listener = m_listeners[listenerId];
  listener.pinMask = 0;
  listener.callback = nullptr;
  listener.boundObj = nullptr;
}
