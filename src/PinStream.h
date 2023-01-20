#ifndef PinStream_h
#define PinStream_h

#include "common.h"

class PinStream {
public:
  PinStream() {}
  virtual ~PinStream() {}

  virtual void pushEvent(const IOEvent& event) = 0;
  virtual void processEvents(void* boundObj, void (*fn) (void* boundObj, const IOEvent&)) = 0;
};


class BasePinStream : public PinStream {
public:
  BasePinStream(IOEvent eventsBuffer[], uint8_t bufferSize);
  virtual ~BasePinStream() = 0;

  void pushEvent(const IOEvent& event) override;
  void processEvents(void* boundObj, void (*fn) (void* boundObj, const IOEvent&)) override;

protected:
  IOEvent* m_eventsBuffer;
  uint8_t m_bufferSize;

  uint8_t m_readPointer = 0;
  uint8_t m_writePointer = 0;
};

#endif
