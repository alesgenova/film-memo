#include "Action.h"

#include <WString.h>

void Action::asString(uint8_t i, char* s, uint8_t n)
{
  switch (this->type) {
    case ActionType::Edit: {
      strncpy_P(s, (const char*)F(" Edit\0"), n);
      break;
    }
    case ActionType::Delete: {
      strncpy_P(s, (const char*)F(" Delete\0"), n);
      break;
    }
    case ActionType::Cancel:
    default: {
      strncpy_P(s, (const char*)F(" Cancel\0"), n);
      break;
    }
  }

  s[n-1] = '\0';
}
