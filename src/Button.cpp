#include "Button.h"

#include "PinStream.h"

#include <avr/pgmspace.h>

// Generated events
const uint8_t NONE_EVENT      = 0b00000000;
const uint8_t DOWN_EVENT      = 0b00001000;
const uint8_t UP_EVENT        = 0b00010000;
const uint8_t CLICK_EVENT     = 0b00100000;
const uint8_t LONGPRESS_EVENT = 0b01000000;

// States
const uint8_t UP_STATE        = 0b000;
const uint8_t DOWN_STATE      = 0b001;
const uint8_t LONGPRESS_STATE = 0b010;

// Transition Actions
const uint8_t BTN_UP = 0;
const uint8_t BTN_DOWN = 1;
const uint8_t TICK = 2;
const uint8_t TICK_LONG = 3;
const uint8_t BTN_UP_LONG = 4;

const int LONGPRESS_THR = 1000;

const uint8_t STATE_MASK  = 0b00000111;
const uint8_t EVENT_MASK  = 0b11111000;

// row is current state, col is incoming pin state
const uint8_t TRANSITION_TABLE[3 * 5] PROGMEM = {
                       //   BTN_UP,                            BTN_DOWN,                  TICK,            TICK_LONG,                         BTN_UP_LONG
  /* UP_STATE          */   UP_STATE,                          DOWN_STATE | DOWN_EVENT,   UP_STATE,        UP_STATE ,                         UP_STATE,
  /* DOWN_STATE        */   UP_STATE | UP_EVENT | CLICK_EVENT, DOWN_STATE,                DOWN_STATE,      LONGPRESS_STATE | LONGPRESS_EVENT, UP_STATE | UP_EVENT | LONGPRESS_EVENT,
  /* LONGPRESS_STATE   */   UP_STATE | UP_EVENT,               LONGPRESS_STATE,           LONGPRESS_STATE, LONGPRESS_STATE,                   UP_STATE | UP_EVENT
};


Button::Button(PinStream& pinStream, bool pullup)
  : m_pinStream(pinStream)
  , m_pullup(pullup)
  , m_state(UP_STATE)
{}

Button::~Button()
{}

Observable<int, MAX_OBSERVABLE_LISTENERS>& Button::downObservable()
{
  return m_downObservable;
}

Observable<int, MAX_OBSERVABLE_LISTENERS>& Button::upObservable()
{
  return m_upObservable;
}

Observable<int, MAX_OBSERVABLE_LISTENERS>& Button::clickObservable()
{
  return m_clickObservable;
}

Observable<int, MAX_OBSERVABLE_LISTENERS>& Button::longpressObservable()
{
  return m_longpressObservable;
}

void Button::process(int t)
{
  m_pinStream.processEvents(this, &Button::processPinEvent);

  uint8_t action;

  if (t - m_lastTransitionTime > LONGPRESS_THR) {
    action = TICK_LONG;
  } else {
    action = TICK;
  }

  this->processAction(action, t);
}

void Button::processPinEvent(void* _self, const IOEvent& pinEvent)
{
  auto self = static_cast<Button*>(_self);

  bool pressed = self->m_pullup ? !pinEvent.state : pinEvent.state;

  uint8_t action;

  if (pressed) {
    action = BTN_DOWN;
  } else if (pinEvent.time - self->m_lastTransitionTime > LONGPRESS_THR) {
    action = BTN_UP_LONG;
  } else {
    action = BTN_UP;
  }

  self->processAction(action, pinEvent.time);
}

void Button::processAction(uint8_t action, int time)
{
  m_state = pgm_read_word_near(TRANSITION_TABLE + (m_state & STATE_MASK) * 5 + action);

  uint8_t event = m_state & EVENT_MASK;

  if (event != NONE_EVENT) {
    m_lastTransitionTime = time;

    if (event & DOWN_EVENT) {
      downObservable().publish(time);
    }

    if (event & UP_EVENT) {
      upObservable().publish(time);
    }

    if (event & CLICK_EVENT) {
      clickObservable().publish(time);
    }

    if (event & LONGPRESS_EVENT) {
      longpressObservable().publish(time);
    }
  }
}
