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

  void process(int t);

  Observable<int, MAX_OBSERVABLE_LISTENERS>& downObservable();
  Observable<int, MAX_OBSERVABLE_LISTENERS>& upObservable();
  Observable<int, MAX_OBSERVABLE_LISTENERS>& clickObservable();
  Observable<int, MAX_OBSERVABLE_LISTENERS>& longpressObservable();
  
protected:
  static void processPinEvent(void* self, const IOEvent&);

  void processAction(uint8_t action, int time);

  PinStream& m_pinStream;
  bool m_pullup;

  uint8_t m_state;

  int m_lastTransitionTime = 0;

  Observable<int, MAX_OBSERVABLE_LISTENERS> m_downObservable;
  Observable<int, MAX_OBSERVABLE_LISTENERS> m_upObservable;
  Observable<int, MAX_OBSERVABLE_LISTENERS> m_clickObservable;
  Observable<int, MAX_OBSERVABLE_LISTENERS> m_longpressObservable;
};

#endif
