#ifndef Rotary_h
#define Rotary_h

#include "common.h"
#include "constants.h"
#include "Observable.h"

class PinStream;

class Rotary {
public:
  Rotary(PinStream& pinStream, uint8_t aMask, uint8_t bMask, bool pullup);
  ~Rotary();

  void process(uint32_t t);

  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& rightObservable();
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& leftObservable();
  
protected:
  static void processPinEvent(void* self, const IOEvent&);

  PinStream& m_pinStream;
  uint8_t m_aMask;
  uint8_t m_bMask;

  uint8_t m_state;

  bool m_pullup;

  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_rightObservable;
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_leftObservable;
};

#endif
