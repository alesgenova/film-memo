#ifndef Observable_hpp
#define Observable_hpp

#include "Observable.h"

template<class T, uint8_t N>
Observable<T,N>::Observable()
{}

template<class T, uint8_t N>
Observable<T,N>::~Observable()
{}

template<class T, uint8_t N>
void Observable<T,N>::publish(const T& payload)
{
  for (uint8_t i = 0; i < N; ++i) {
    auto& listener = m_listeners[i];

    if (listener.callback) {
      listener.callback(listener.boundObj, payload);
    }
  }  
}

template<class T, uint8_t N>
int8_t Observable<T,N>::addListener(void* boundObj, void (*fn) (void* boundObj, const T&))
{
  if (!fn) {
    return -1;
  }
  
  for (uint8_t i = 0; i < N; ++i) {
    auto& listener = m_listeners[i];

    if (!listener.callback) {
      listener.boundObj = boundObj;
      listener.callback = fn;

      return i;
    }
  }

  return -1;  
}

template<class T, uint8_t N>
void Observable<T,N>::removeListener(int8_t listenerId)
{
  if (listenerId < 0 || listenerId >= N) {
    return;
  }

  auto& listener = m_listeners[listenerId];
  listener.callback = nullptr;
  listener.boundObj = nullptr;  
}

#endif
