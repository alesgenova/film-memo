#include "Rotary.h"

#include "PinStream.h"

#include <avr/pgmspace.h>

const uint8_t DIR_NONE    = 0b00000000;
const uint8_t DIR_CW      = 0b00100000;
const uint8_t DIR_CCW     = 0b01000000;
const uint8_t DIR_MASK    = 0b01100000;

const uint8_t START_STATE = 0b000;
const uint8_t CW1_STATE   = 0b001;
const uint8_t CW2_STATE   = 0b010;
const uint8_t CW3_STATE   = 0b011;
const uint8_t CCW1_STATE  = 0b100;
const uint8_t CCW2_STATE  = 0b101;
const uint8_t CCW3_STATE  = 0b110;

const uint8_t STATE_MASK  = 0b00011111;

// row is current state, col is incoming pin state
const uint8_t TRANSITION_TABLE[7 * 4] PROGMEM = {
                  //  0b00,                  0b01,        0b10,        0b11
  /* START_STATE */   START_STATE,           CW1_STATE,   CCW1_STATE,  START_STATE ,
  /* CW1_STATE   */   START_STATE,           CW1_STATE,   START_STATE, CW2_STATE   ,
  /* CW2_STATE   */   START_STATE,           CW1_STATE,   CW3_STATE,   CW2_STATE   ,
  /* CW3_STATE   */   START_STATE | DIR_CW,  START_STATE, CW3_STATE,   CW2_STATE   ,
  /* CCW1_STATE  */   START_STATE,           START_STATE, CCW1_STATE,  CCW2_STATE  ,
  /* CCW2_STATE  */   START_STATE,           CCW3_STATE,  CCW1_STATE,  CCW2_STATE  ,
  /* CCW3_STATE  */   START_STATE | DIR_CCW, CCW3_STATE,  START_STATE, CCW2_STATE  ,
};

Rotary::Rotary(PinStream& pinStream, uint8_t aMask, uint8_t bMask, bool pullup)
  : m_pinStream(pinStream)
  , m_aMask(aMask)
  , m_bMask(bMask)
  , m_pullup(pullup)
  , m_state(START_STATE)
{}

Rotary::~Rotary()
{}

Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& Rotary::rightObservable()
{
  return m_rightObservable;
}

Observable<uint32_t, MAX_OBSERVABLE_LISTENERS>& Rotary::leftObservable()
{
  return m_leftObservable;
}

void Rotary::process(uint32_t t)
{
  m_pinStream.processEvents(this, &Rotary::processPinEvent);
}

void Rotary::processPinEvent(void* _self, const IOEvent& pinEvent)
{
  auto self = static_cast<Rotary*>(_self);

  bool a = pinEvent.state & self->m_aMask;
  bool b = pinEvent.state & self->m_bMask;

  if (self->m_pullup) {
    a = !a;
    b = !b;
  }

  uint8_t pinState = b << 1 | a;

  self->m_state = pgm_read_word_near(TRANSITION_TABLE + (self->m_state & STATE_MASK) * 4 + pinState);

  uint8_t transition = self->m_state & DIR_MASK;

  if (transition == DIR_CW) {
    self->rightObservable().publish(pinEvent.time);
  } else if (transition == DIR_CCW) {
    self->leftObservable().publish(pinEvent.time);
  }
}
