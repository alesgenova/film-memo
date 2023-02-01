#ifndef film_constants_h
#define film_constants_h

#include <stdint.h>

// define which app is being built
#define __DEBUG false // app to test buttons and rotaries
#define __MEMO  true  // full film memo application
#define __DEV   false // whether to enable Serial

const uint8_t PORT_B_PCINT = 0b00000001;
const uint8_t PORT_B_REGISTER = 2;

const uint8_t ROT_A_1_PIN = 15; // PCINT1
const uint8_t ROT_A_1_MASK = 1 << 1;

const uint8_t ROT_A_2_PIN = 10; // PCINT6
const uint8_t ROT_A_2_MASK = 1 << 6;

const uint8_t BTN_A_PIN = 11; // PCINT7
const uint8_t BTN_A_MASK = 1 << 7;

const uint8_t ROT_B_1_PIN = 16; // PCINT2
const uint8_t ROT_B_1_MASK = 1 << 2;

const uint8_t ROT_B_2_PIN = 14; // PCINT3
const uint8_t ROT_B_2_MASK = 1 << 3;

const uint8_t BTN_B_PIN = 9; // PCINT5
const uint8_t BTN_B_MASK = 1 << 5;

const uint8_t FLASH_PIN = 0;
const uint8_t FLASH_INTERRUPT = 2;

const uint8_t METER_PIN_ANALOG = 11;

const uint8_t MAX_OBSERVABLE_LISTENERS = 1;

const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;

const uint8_t TEXT_HEIGHT = 8;
const uint8_t MARGIN = 1;
const uint8_t TITLE_HEIGHT = TEXT_HEIGHT + MARGIN + 1;

const uint8_t N_ROLLS = 6;
const uint8_t N_FRAMES_PER_ROLL = 36;

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

#endif
