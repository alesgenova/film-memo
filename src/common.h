#ifndef common_h
#define common_h

#include "stdint.h"

struct IOEvent {
  uint32_t time = 0;
  uint8_t state = 0;
};

#endif
