#include "Persistency.h"

#include "constants.h"

#include <EEPROM.h>

namespace Persistency {

// Reserve a few bites in the EEPROM for future persisten settings
const uint8_t SETTINGS_BYTES = 32;

const uint8_t ROLL_BYTES = sizeof(Roll);
const uint8_t FRAME_BYTES = sizeof(Roll);

void readRoll(uint8_t rollId, Roll& roll)
{
  int address = SETTINGS_BYTES + rollId * (ROLL_BYTES + FRAME_BYTES * N_FRAMES_PER_ROLL);

  EEPROM.get(address, roll);
}

void saveRoll(uint8_t rollId, const Roll& roll)
{
  int address = SETTINGS_BYTES + rollId * (ROLL_BYTES + FRAME_BYTES * N_FRAMES_PER_ROLL);

  EEPROM.put(address, roll);
}

void readFrame(uint8_t rollId, uint8_t frameId, Frame& frame)
{
  int address = SETTINGS_BYTES + rollId * (ROLL_BYTES + FRAME_BYTES * N_FRAMES_PER_ROLL) + ROLL_BYTES + frameId * FRAME_BYTES;

  EEPROM.get(address, frame);
}

void saveFrame(uint8_t rollId, uint8_t frameId, const Frame& frame)
{
  int address = SETTINGS_BYTES + rollId * (ROLL_BYTES + FRAME_BYTES * N_FRAMES_PER_ROLL) + ROLL_BYTES + frameId * FRAME_BYTES;

  EEPROM.put(address, frame);
}

} // namespace Persistency
