/*
  Film Memo

  Alessandro Genova, 01/2023
*/

#include "src/App.h"
#include "src/Controls.h"

#if __DEV
extern char *__brkval;

int freeMemory() {
  char top;
  return &top - __brkval;
}
#endif

void setup() {
#if __DEV
  Serial.begin(9600);
  delay(2000);
  Serial.println(F("Ready!"));
#endif

  Controls::instance().initialize();
  App::instance().initialize();

#if __DEV
  Serial.println(freeMemory());
#endif
}

ISR(PCINT0_vect) {
  Controls::onPortInterrupt();
}

void loop() {
  uint32_t t = micros();
  Controls::instance().process(t);
  App::instance().process(t);
}
