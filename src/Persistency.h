#ifndef Persistency_h
#define Persistency_h

#include "Roll.h"
#include "Frame.h"

namespace Persistency {

bool readAutoShutter();
void writeAutoShutter(bool enable);

void readShutterCalibration(uint8_t shutterId, uint32_t& value);
void writeShutterCalibration(uint8_t shutterId, const uint32_t& value);

void readMeterCalibration(float& value);
void writeMeterCalibration(const float& value);

void readRoll(uint8_t rollId, Roll& roll);
void saveRoll(uint8_t rollId, const Roll& roll);

void readFrame(uint8_t rollId, uint8_t frameId, Frame& frame);
void saveFrame(uint8_t rollId, uint8_t frameId, const Frame& frame);

} // namespace Persistency

#endif