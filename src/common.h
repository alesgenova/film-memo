#ifndef common_h
#define common_h

#include "stdint.h"

struct IOEvent {
  int time = 0;
  uint8_t state = 0;
};

#endif
