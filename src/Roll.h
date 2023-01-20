#ifndef Roll_h
#define Roll_h

#include "Exposure.h"

enum class RollManufacturer {
  None,
  Fuji,
  Kodak,
  Other
};

void rollManufacturerAsString(RollManufacturer manufacturer, char* s, uint8_t n);

class Roll
{
public:
  Roll(){}

  RollManufacturer manufacturer() const;
  void setManufacturer(RollManufacturer manufacturer);

  ISOValue iso() const;
  void setIso(ISOValue iso);

  void asString(uint8_t i, char* s, uint8_t n);

protected:
  uint8_t m_data = 0;
};

#endif
