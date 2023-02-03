#ifndef Button_h
#define Button_h

#include "common.h"
#include "constants.h"
#include "Observable.h"

class PinStream;

class Button {
public:
  Button(PinStream& pinStream, bool pullup);
  ~Button();

  void process(uint32_t t);

  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& downObservable();
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& upObservable();
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& clickObservable();
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& longpressObservable();
  
protected:
  static void processPinEvent(void* self, const IOEvent&);

  void processAction(uint8_t action, uint32_t time);

  PinStream& m_pinStream;
  bool m_pullup;

  uint8_t m_state;

  uint32_t m_lastDownTime = 0;

  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_downObservable;
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_upObservable;
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_clickObservable;
  Observable<uint32_t, MAX_OBSERVABLE_LISTENERS> m_longpressObservable;
};

#endif
