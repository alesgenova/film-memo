#include "PinStreamPCINT.h"

#include "PortStream.h"

PinStreamPCINT::PinStreamPCINT(uint8_t pinMaskInPort, PortStream& portStream, IOEvent eventsBuffer[], uint8_t bufferSize)
  : BasePinStream(eventsBuffer, bufferSize)
  , m_pinMaskInPort(pinMaskInPort)
  , m_portStream(portStream)
{
  m_portStream.activePinRegister() |= m_pinMaskInPort;
  m_listenerId = m_portStream.registerListener(m_pinMaskInPort, this, &PinStreamPCINT::onPortEvent);
}

PinStreamPCINT::~PinStreamPCINT()
{
  m_portStream.unregisterListener(m_listenerId);
}

void PinStreamPCINT::onPortEvent(void* _self, const IOEvent& event)
{
  auto self = static_cast<PinStreamPCINT*>(_self);

  IOEvent pinEvent;
  pinEvent.time = event.time;
  pinEvent.state = event.state & self->m_pinMaskInPort;

  self->pushEvent(pinEvent);
}
