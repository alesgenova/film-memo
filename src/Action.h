#ifndef Action_h
#define Action_h

#include <stdint.h>

enum class ActionType : uint8_t {
  // General actions
  Cancel,
  // Modify entity actions
  Edit,
  Delete,
  // Settings menu actions
  EnableHotShoeShutter,
  CalibrateHotShoeShutter,
  CalibrateMeter,
  DisplayContrast,
  About
};

class Action {
public:
  Action(){}
  
  ActionType type = ActionType::Cancel;

  void asString(uint8_t i, char* s, uint8_t n);
};

#endif
