#include "PinStream.h"

BasePinStream::BasePinStream(IOEvent eventsBuffer[], uint8_t bufferSize)
  : m_eventsBuffer(eventsBuffer)
  , m_bufferSize(bufferSize)
{}

BasePinStream::~BasePinStream()
{}

void BasePinStream::pushEvent(const IOEvent& _event)
{
  auto& event = m_eventsBuffer[m_writePointer];
  event.time = _event.time;
  event.state = _event.state;
  
  m_writePointer = (m_writePointer + 1) % m_bufferSize;
}

void BasePinStream::processEvents(void* boundObj, void (*fn) (void* boundObj, const IOEvent&))
{
  while(m_readPointer != m_writePointer) {
    auto& event = m_eventsBuffer[m_readPointer];

    if (fn) {
      fn(boundObj, event);
    }

    m_readPointer = (m_readPointer + 1) % m_bufferSize;
  }
}
