#ifndef Rotary_h
#define Rotary_h

#include "common.h"
#include "constants.h"
#include "Observable.h"

class PinStream;

class Rotary {
public:
  Rotary(PinStream& pinStream, uint8_t aMask, uint8_t bMask, bool pullup);
  virtual ~Rotary();

  void process(int t);

  Observable<int, MAX_OBSERVABLE_LISTENERS>& rightObservable();
  Observable<int, MAX_OBSERVABLE_LISTENERS>& leftObservable();
  
protected:
  static void processPinEvent(void* self, const IOEvent&);

  PinStream& m_pinStream;
  uint8_t m_aMask;
  uint8_t m_bMask;

  uint8_t m_state;

  bool m_pullup;

  Observable<int, MAX_OBSERVABLE_LISTENERS> m_rightObservable;
  Observable<int, MAX_OBSERVABLE_LISTENERS> m_leftObservable;
};

#endif
