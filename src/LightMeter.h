#ifndef LightMeter_h
#define LightMeter_h

#include "constants.h"

#include "Observable.h"

const uint8_t WINDOW_SIZE = 10;

enum class MeterMode {
  Single,
  Continuous
};

class LightMeter {
public:
  LightMeter(uint8_t analogPin);
  ~LightMeter();

  void process(uint32_t t);

  Observable<int, MAX_OBSERVABLE_LISTENERS>& readingObservable();

  MeterMode mode() const;
  void setMode(MeterMode mode);

  void takeReading();

protected:
  uint8_t m_analogPin;

  Observable<int, MAX_OBSERVABLE_LISTENERS> m_readingObservable;

  int m_values[WINDOW_SIZE];
  int m_avgValue = 0;
  uint8_t m_valueIdx = 0;
  MeterMode m_mode = MeterMode::Single;
};

#endif
