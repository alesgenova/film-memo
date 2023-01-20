#include "LightMeter.h"

#include "Arduino.h"

LightMeter::LightMeter(uint8_t analogPin)
  : m_analogPin(analogPin)
{
  for (uint8_t i = 0; i < WINDOW_SIZE; ++i) {
    m_values[i] = 0;
  }
}

LightMeter::~LightMeter()
{}

void LightMeter::process(int t)
{
  if (m_mode == MeterMode::Continuous) {
    int value = analogRead(m_analogPin);
  
    m_avgValue = m_avgValue - m_values[m_valueIdx] + value;
    m_values[m_valueIdx] = value;

    m_valueIdx = (m_valueIdx + 1) % WINDOW_SIZE;

    m_readingObservable.publish(m_avgValue / WINDOW_SIZE);

  } else if (m_mode == MeterMode::Single && m_valueIdx < WINDOW_SIZE) {
    int value = analogRead(m_analogPin);
  
    m_avgValue +=value;

    m_valueIdx += 1;

    if (m_valueIdx == WINDOW_SIZE) {
      m_readingObservable.publish(m_avgValue / WINDOW_SIZE);
    }

  } else {
    return;
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

void LightMeter::takeReading()
{
  m_valueIdx = 0;
  m_avgValue = 0;
}
