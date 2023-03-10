#include "Controls.h"

#include <Arduino.h>

#include "constants.h"

Controls::Controls()
  : portStream(PORT_B_PCINT, PORT_B_REGISTER, PCMSK0)
  , pinStreamButtonA(BTN_A_MASK, portStream, eventBufferButtonA, 2)
  , pinStreamButtonB(BTN_B_MASK, portStream, eventBufferButtonB, 2)
  , pinStreamRotaryA(ROT_A_1_MASK | ROT_A_2_MASK, portStream, eventBufferRotaryA, 4)
  , pinStreamRotaryB(ROT_B_1_MASK | ROT_B_2_MASK, portStream, eventBufferRotaryB, 4)
  , pinStreamFlash(FLASH_PIN, eventBufferFlash, 2)
  , buttonA(pinStreamButtonA, true)
  , buttonB(pinStreamButtonB, true)
  , rotaryA(pinStreamRotaryA, ROT_A_1_MASK, ROT_A_2_MASK, true)
  , rotaryB(pinStreamRotaryB, ROT_B_1_MASK, ROT_B_2_MASK, true)
  , flash(pinStreamFlash, true)
  , display()
  , meter(METER_PIN_ANALOG)
{}

Controls* Controls::unique_instance = nullptr;

Controls& Controls::instance() {
  if (!Controls::unique_instance) {
    Controls::unique_instance = new Controls();
  }

  return *Controls::unique_instance;
}

void Controls::initialize()
{
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(ROT_A_1_PIN, INPUT_PULLUP);
  pinMode(ROT_A_2_PIN, INPUT_PULLUP);
  pinMode(ROT_B_1_PIN, INPUT_PULLUP);
  pinMode(ROT_B_2_PIN, INPUT_PULLUP);
  pinMode(FLASH_PIN, INPUT_PULLUP);

  display.begin(SCREEN_ADDRESS);
  display.setContrast(200);

  display.clear();
  display.render();

  attachInterrupt(FLASH_INTERRUPT, &Controls::onFlashInterrupt, CHANGE);
}

void Controls::process(uint32_t t)
{
  buttonA.process(t);
  buttonB.process(t);
  rotaryA.process(t);
  rotaryB.process(t);
  flash.process(t);
  meter.process(t);

  display.render();
}

void Controls::onPortInterrupt()
{
  Controls::instance().portStream.onChange();
}

void Controls::onFlashInterrupt()
{
  Controls::instance().pinStreamFlash.onChange();
}
