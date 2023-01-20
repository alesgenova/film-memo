#include "PinStreamINT.h"

#include "Arduino.h"

PinStreamINT::PinStreamINT(uint8_t pin, IOEvent eventsBuffer[], uint8_t bufferSize)
  : BasePinStream(eventsBuffer, bufferSize)
  , m_pin(pin)
{}

PinStreamINT::~PinStreamINT()
{}

void PinStreamINT::onChange()
{
  IOEvent event;
  event.time = millis();
  event.state = digitalRead(m_pin);

  pushEvent(event);
}
