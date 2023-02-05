#include "Persistency.h"

#include "constants.h"

#include <EEPROM.h>

namespace Persistency {

// Reserve a few bites in the EEPROM for future persisten settings
const uint8_t SETTINGS_BYTES = 64;

const uint16_t AUTO_SHUTTER_BYTE = 0;
const uint16_t SHUTTER_CALIBRATION_BYTE = AUTO_SHUTTER_BYTE + 1;
const uint16_t SHUTTER_BYTES = sizeof(uint32_t);

const uint16_t METER_CALIBRATION_BYTE = SHUTTER_CALIBRATION_BYTE + 10 * SHUTTER_BYTES;

const uint16_t DISPLAY_CONTRAST_BYTE = METER_CALIBRATION_BYTE + sizeof(float);

const uint16_t ROLL_BYTES = sizeof(Roll);
const uint16_t FRAME_BYTES = sizeof(Frame);

bool readAutoShutter()
{
  return EEPROM.read(AUTO_SHUTTER_BYTE);
}

void writeAutoShutter(bool enable)
{
  EEPROM.update(AUTO_SHUTTER_BYTE, enable);
}

void readShutterCalibration(uint8_t shutterId, uint32_t& value)
{
  int address = SHUTTER_CALIBRATION_BYTE + shutterId * SHUTTER_BYTES;

  EEPROM.get(address, value);
}

void writeShutterCalibration(uint8_t shutterId, const uint32_t& value)
{
  int address = SHUTTER_CALIBRATION_BYTE + shutterId * SHUTTER_BYTES;

  EEPROM.put(address, value);
}

void readMeterCalibration(float& value)
{
  EEPROM.get(METER_CALIBRATION_BYTE, value);
}

void writeMeterCalibration(const float& value)
{
  EEPROM.put(METER_CALIBRATION_BYTE, value);
}

void readDisplayContrast(uint8_t& value)
{
  value = EEPROM.read(DISPLAY_CONTRAST_BYTE);
}

void writeDisplayContrast(const uint8_t& value)
{
  EEPROM.update(DISPLAY_CONTRAST_BYTE, value);
}

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
