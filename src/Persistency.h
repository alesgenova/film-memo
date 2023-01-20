#ifndef Persistency_h
#define Persistency_h

#include "Roll.h"
#include "Frame.h"

namespace Persistency {

void readRoll(uint8_t rollId, Roll& roll);
void saveRoll(uint8_t rollId, const Roll& roll);

void readFrame(uint8_t rollId, uint8_t frameId, Frame& frame);
void saveFrame(uint8_t rollId, uint8_t frameId, const Frame& frame);

} // namespace Persistency

#endif