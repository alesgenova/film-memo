#ifndef Action_h
#define Action_h

#include <stdint.h>

enum class ActionType {
  Cancel,
  Edit,
  Delete
};

class Action {
public:
  Action(){}
  
  ActionType type = ActionType::Cancel;

  void asString(uint8_t i, char* s, uint8_t n);
};

#endif
