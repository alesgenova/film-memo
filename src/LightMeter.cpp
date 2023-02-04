#include "LightMeter.h"

#include "Arduino.h"

LightMeter::LightMeter(uint8_t analogPin)
  : m_analogPin(analogPin)
{}

LightMeter::~LightMeter()
{}

void LightMeter::process(uint32_t t)
{
  // Make sure the reading stabilizes
  if (t - m_lastProcessTime < 10000) {
    return;
  }

  m_lastProcessTime = t;

  if (m_valueIdx < WINDOW_SIZE) {
    int value = analogRead(m_analogPin);
    m_avgValue +=value;
    m_valueIdx += 1;

    if (m_valueIdx == WINDOW_SIZE) {
      m_readingObservable.publish(m_avgValue / WINDOW_SIZE);
      m_avgValue = 0;

      if (m_mode == MeterMode::Continuous) {
        m_valueIdx = 0;
      }
    }
  }
}

Observable<int, MAX_OBSERVABLE_LISTENERS>& LightMeter::readingObservable()
{
  return m_readingObservable;
}

MeterMode LightMeter::mode() const
{
  return m_mode;
}

void LightMeter::setMode(MeterMode mode)
{
  m_mode = mode;
  m_valueIdx = 0;
  m_avgValue = 0;
}
