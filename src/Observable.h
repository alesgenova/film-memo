#ifndef Observable_h
#define Observable_h

#include <stdint.h>

template<class T, uint8_t MAX_LISTENERS>
class Observable {

public:
  struct Listener {
    void* boundObj = nullptr;
    void (*callback) (void* boundObj, const T& payload) = nullptr;
  };

  Observable();
  ~Observable();

  void publish(const T& event);

  int8_t addListener(void* boundObj, void (*fn) (void* boundObj, const T&));
  void removeListener(int8_t listenerId);

protected:
  Listener m_listeners[MAX_LISTENERS];
};

#include "Observable.hpp"

#endif
