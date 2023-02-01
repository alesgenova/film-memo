#include "Action.h"

#include <WString.h>

void Action::asString(uint8_t i, char* s, uint8_t n)
{
  switch (this->type) {
    case ActionType::Edit: {
      strncpy_P(s, PSTR(" Edit"), n);
      break;
    }
    case ActionType::Delete: {
      strncpy_P(s, PSTR(" Delete"), n);
      break;
    }
    case ActionType::Cancel:
    default: {
      strncpy_P(s, PSTR(" Cancel"), n);
      break;
    }
  }

  s[n-1] = '\0';
}
