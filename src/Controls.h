#ifndef Controls_h
#define Controls_h

#include "Display.h"
#include "PortStream.h"
#include "PinStream.h"
#include "PinStreamPCINT.h"
#include "PinStreamINT.h"
#include "Button.h"
#include "Rotary.h"
#include "LightMeter.h"

class Controls {
public:
  static Controls& instance();

  Controls(const Controls&) = delete;
  Controls& operator=(const Controls&) = delete;

  void initialize();

  void process(int t);
  
  static void onPortInterrupt();
  static void onFlashInterrupt();

  Button buttonA;
  Button buttonB;
  Rotary rotaryA;
  Rotary rotaryB;
  Button flash;
  Display display;
  LightMeter meter;

protected:
  Controls();

  static Controls* unique_instance;

  PortStream portStream;

  PinStreamPCINT pinStreamButtonA;
  PinStreamPCINT pinStreamButtonB;
  PinStreamPCINT pinStreamRotaryA;
  PinStreamPCINT pinStreamRotaryB;
  PinStreamINT pinStreamFlash;

  IOEvent eventBufferButtonA[2];
  IOEvent eventBufferButtonB[2];
  IOEvent eventBufferRotaryA[8];
  IOEvent eventBufferRotaryB[8];
  IOEvent eventBufferFlash[2];
};

#endif