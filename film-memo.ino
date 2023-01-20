/*
  Film Memo

  Alessandro Genova, 01/2023
*/

#include "src/App.h"
#include "src/Controls.h"

void setup() {
#if __DEV
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("Ready!"));
#endif

  Controls::instance().initialize();
  App::instance().initialize();
}

ISR(PCINT0_vect) {
  Controls::onPortInterrupt();
}

void loop() {
  int t = millis();
  Controls::instance().process(t);
  App::instance().process(t);
}
