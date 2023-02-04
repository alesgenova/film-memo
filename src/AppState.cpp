#include "AppState.h"

#include <WString.h>

#include "App.h"
#include "Controls.h"

#if __DEV
#include <Arduino.h>
#endif

#if __MEMO
#include <Arduino.h>
#include "Persistency.h"
#endif


AppState::AppState(App& app)
  : m_app(app)
{}

AppState::~AppState()
{}

#if __DEBUG

// DebugAppState

DebugAppState::DebugAppState(App& app)
  : AppState(app)
{}

DebugAppState::~DebugAppState()
{}

void DebugAppState::activate()
{
  drawButtonA();
  drawButtonB();
  drawRotaryA();
  drawRotaryB();
  drawMeter();
}

void DebugAppState::deactivate()
{
  Controls::instance().display.clear();
}

void DebugAppState::onClickButtonA(uint32_t t)
{
  m_buttonA = !m_buttonA;
  drawButtonA();
}

void DebugAppState::onLongpressButtonA(uint32_t t)
{
  m_buttonA = !m_buttonA;
  drawButtonA();
}

void DebugAppState::onClickButtonB(uint32_t t)
{
  m_buttonB = !m_buttonB;
  drawButtonB();

  auto& meter = Controls::instance().meter;
  if (meter.mode() == MeterMode::Single) {
    meter.setMode(MeterMode::Single);
  }
}

void DebugAppState::onLongpressButtonB(uint32_t t)
{
  m_buttonB = !m_buttonB;
  drawButtonB();

  auto& meter = Controls::instance().meter;
  if (meter.mode() == MeterMode::Single) {
    meter.setMode(MeterMode::Continuous);
  } else {
    meter.setMode(MeterMode::Single);
  }
}

void DebugAppState::onRightRotaryA(uint32_t t)
{
  if (m_rotaryA == 16) {
    return;
  }

  m_rotaryA += 1;
  drawRotaryA();
}

void DebugAppState::onLeftRotaryA(uint32_t t)
{
  if (m_rotaryA == 0) {
    return;
  }

  m_rotaryA -= 1;
  drawRotaryA();
}

void DebugAppState::onRightRotaryB(uint32_t t)
{
  if (m_rotaryB == 16) {
    return;
  }

  m_rotaryB += 1;
  drawRotaryB();
}

void DebugAppState::onLeftRotaryB(uint32_t t)
{
  if (m_rotaryB == 0) {
    return;
  }

  m_rotaryB -= 1;
  drawRotaryB();
}

void DebugAppState::onDownFlash(uint32_t t)
{}

void DebugAppState::onUpFlash(uint32_t t)
{}

void DebugAppState::onMeterReading(int value)
{
  m_meterValue = value;
  drawMeter();
}

void DebugAppState::drawButtonA()
{
  auto& display = Controls::instance().display;

  Display::Painter color = Display::whitePainter;
  Display::Painter bg = Display::blackPainter;

  if (m_buttonA) {
    color = Display::blackPainter;
    bg = Display::whitePainter;
  }

  display.print(0, 0, F("Button A"), color, bg);
}

void DebugAppState::drawButtonB()
{
  auto& display = Controls::instance().display;

  Display::Painter color = &Display::whitePainter;
  Display::Painter bg = &Display::blackPainter;

  if (m_buttonB) {
    color = &Display::blackPainter;
    bg = &Display::whitePainter;
  }

  display.print(0, 20, F("Button B"), color, bg);
}

void DebugAppState::drawRotaryA()
{
  auto& display = Controls::instance().display;

  display.drawHLine(0, 12, m_rotaryA * 8, &Display::whitePainter);
  display.drawHLine(m_rotaryA * 8, 12, display.width() - 1, &Display::blackPainter);
}

void DebugAppState::drawRotaryB()
{
  auto& display = Controls::instance().display;

  display.drawHLine(0, 32, m_rotaryB * 8, &Display::whitePainter);
  display.drawHLine(m_rotaryB * 8, 32, display.width() - 1, &Display::blackPainter);
}

void DebugAppState::drawMeter()
{
  auto& display = Controls::instance().display;

  display.print(0, 40, F("Meter"));

  auto meterX = (float(m_meterValue) / 1023.) * display.width();

  display.drawHLine(0, 52, meterX);
  display.drawHLine(meterX, 52, display.width() - 1, &Display::blackPainter);

  display.printEmpty(0, 57, 4);
  display.print(0, 57, m_meterValue);
}

#elif __MEMO

// ListState

ListState::ListState(App& app)
  : AppState(app)
{}

ListState::~ListState()
{}

void ListState::activate()
{
  auto& display = Controls::instance().display;

  display.clear();

  uint8_t bounds[4] = {0, 0, display.width(), display.height()};
  m_app.m_listView.setBounds(bounds);

  char title[22];
  ListView::ItemGetter itemGetter;
  itemGetter.boundObj = this;
  uint8_t size;
  uint8_t activeItem;

  if (m_isFrameTarget) {
    strncpy_P(title, PSTR("Roll"), 22);
    Roll roll;
    Persistency::readRoll(m_app.m_activeRollId, roll);
    roll.asString(m_app.m_activeRollId, title + 4, 18);
    itemGetter.getter = &ListState::frameItemGetter;
    size = N_FRAMES_PER_ROLL;
    activeItem = m_app.m_activeFrameId;
  } else {
    strncpy_P(title, PSTR("Rolls"), 22);
    itemGetter.getter = &ListState::rollItemGetter;
    size = N_ROLLS;
    activeItem = m_app.m_activeRollId;
  }

  m_app.m_listView.setTitle(title);
  m_app.m_listView.setItems(itemGetter, size);
  m_app.m_listView.setSelected(activeItem);
}

void ListState::deactivate()
{}

void ListState::setIsFrameTarget(bool isFrameTarget)
{
  m_isFrameTarget = isFrameTarget;
}

void ListState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();

  if (m_isFrameTarget) {
    m_app.m_listState.setIsFrameTarget(false);
    m_app.m_state = &m_app.m_listState;
  } else {
    m_app.m_state = &m_app.m_meterState;
  }

  m_app.m_state->activate();
}

void ListState::onLongpressButtonA(uint32_t t)
{
  if (!m_isFrameTarget) {
    m_app.m_state->deactivate();
    m_app.m_state = &m_app.m_settingsState;
    m_app.m_state->activate();
  }
}

void ListState::onClickButtonB(uint32_t t)
{
  m_app.m_state->deactivate();

  if (m_isFrameTarget) {
    m_app.m_activeFrameId = m_app.m_listView.selected();

    m_app.m_state = &m_app.m_editFrameState;

  } else {
    m_app.m_activeRollId = m_app.m_listView.selected();
    m_app.m_activeFrameId = 0;

    Roll selectedRoll;
    Persistency::readRoll(m_app.m_activeRollId, selectedRoll);

    if (selectedRoll.manufacturer() == RollManufacturer::None) {

      m_app.m_state = &m_app.m_editRollState;

    } else {

      m_app.m_listState.setIsFrameTarget(true);
      m_app.m_state = &m_app.m_listState;

    }
  }

  m_app.m_state->activate();
}

void ListState::onLongpressButtonB(uint32_t t)
{
  auto activeId = m_app.m_listView.selected();

  if (m_isFrameTarget) {
    Frame selectedFrame;
    Persistency::readFrame(m_app.m_activeRollId, activeId, selectedFrame);

    if (selectedFrame.aperture() != ApertureValue::None) {
      m_app.m_activeFrameId = activeId;
      m_app.m_state->deactivate();
      m_app.m_editModalState.setIsFrameTarget(true);
      m_app.m_state = &m_app.m_editModalState;
      m_app.m_state->activate();
    }
  } else {
    Roll selectedRoll;
    Persistency::readRoll(activeId, selectedRoll);

    if (selectedRoll.manufacturer() != RollManufacturer::None) {
      m_app.m_activeRollId = activeId;
      m_app.m_activeFrameId = 0;
      m_app.m_state->deactivate();
      m_app.m_editModalState.setIsFrameTarget(false);
      m_app.m_state = &m_app.m_editModalState;
      m_app.m_state->activate();
    }
  }
}

void ListState::onRightRotaryA(uint32_t t)
{
  m_app.m_listView.selectNext();
}

void ListState::onLeftRotaryA(uint32_t t)
{
  m_app.m_listView.selectPrev();
}

void ListState::onRightRotaryB(uint32_t t)
{
  m_app.m_listView.selectNext(10);
}

void ListState::onLeftRotaryB(uint32_t t)
{
  m_app.m_listView.selectPrev(10);
}

void ListState::rollItemGetter(void* _self, uint8_t i, char* s, uint8_t n)
{
  Roll roll;
  Persistency::readRoll(i, roll);
  roll.asString(i, s, n);
}

void ListState::frameItemGetter(void* _self, uint8_t i, char* s, uint8_t n)
{
  auto self = static_cast<ListState*>(_self);
  Frame frame;
  Persistency::readFrame(self->m_app.m_activeRollId, i, frame);
  frame.asString(i, s, n);
}

// EditRollState

EditRollState::EditRollState(App& app)
  : AppState(app)
{}

EditRollState::~EditRollState()
{}

void EditRollState::activate()
{
  auto& display = Controls::instance().display;

  display.clear();

  Persistency::readRoll(m_app.m_activeRollId, m_roll);

  if (m_roll.manufacturer() == RollManufacturer::None) {
    m_roll.setManufacturer(RollManufacturer::Fuji);
  }

  if (m_roll.iso() == ISOValue::None) {
    m_roll.setIso(ISOValue::_400);
  }

  drawTitle();
  drawManufacturer();
  drawISO();
}

void EditRollState::deactivate()
{}

void EditRollState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditRollState::onClickButtonB(uint32_t t)
{
  Persistency::saveRoll(m_app.m_activeRollId, m_roll);

  onClickButtonA(t);
}

void EditRollState::onRightRotaryA(uint32_t t)
{
  changeManufacturer(true);
}

void EditRollState::onLeftRotaryA(uint32_t t)
{
  changeManufacturer(false);
}

void EditRollState::onRightRotaryB(uint32_t t)
{
  changeISO(true);
}

void EditRollState::onLeftRotaryB(uint32_t t)
{
  changeISO(false);
}

void EditRollState::changeManufacturer(bool increase)
{
  auto manufacturer = m_roll.manufacturer();
  if (increase) {
    if (manufacturer < RollManufacturer::Other) {
      m_roll.setManufacturer((RollManufacturer)(1 + (uint8_t)manufacturer));
      drawManufacturer();
    }
  } else {
    if (manufacturer > RollManufacturer::Fuji) {
      m_roll.setManufacturer((RollManufacturer)((uint8_t)manufacturer - 1));
      drawManufacturer();
    }
  }
}

void EditRollState::changeISO(bool increase)
{
  auto iso = m_roll.iso();

  if (increase) {
    if (iso < ISOValue::_1600) {
      m_roll.setIso((ISOValue)(1 + (uint8_t)iso));
      drawISO();
    }
  } else {
    if (iso > ISOValue::_100) {
      m_roll.setIso((ISOValue)((uint8_t)iso - 1));
      drawISO();
    }
  }
}

void EditRollState::drawTitle()
{
  auto& display = Controls::instance().display;

  uint8_t x = 0;
  x = display.print(x, 0, F("Roll #"));
  display.print(x, 0, m_app.m_activeRollId);
  display.drawHLine(0, TEXT_HEIGHT + MARGIN, display.width());
}

void EditRollState::drawManufacturer()
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  display.print(0, y, F("Brand"));

  char label[6];
  rollManufacturerAsString(m_roll.manufacturer(), label, 6);

  y += TEXT_HEIGHT + MARGIN;

  display.printEmpty(0, y, 6, &Display::blackPainter, 2, 2);
  display.print(0, y, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

void EditRollState::drawISO()
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + TEXT_HEIGHT * 3 + MARGIN * 6;

  display.print(0, y, F("ISO"));

  char label[5];
  isoValueAsString(m_roll.iso(), label, 5);

  y += TEXT_HEIGHT + MARGIN;

  display.printEmpty(0, y, 5, &Display::blackPainter, 2, 2);
  display.print(0, y, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

// EditFrameState

EditFrameState::EditFrameState(App& app)
  : AppState(app)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

EditFrameState::~EditFrameState()
{}

void EditFrameState::activate()
{
  auto& display = Controls::instance().display;
  auto& meter = Controls::instance().meter;

  display.clear();

  meter.setMode(MeterMode::Single);

  Persistency::readRoll(m_app.m_activeRollId, m_roll);
  Persistency::readFrame(m_app.m_activeRollId, m_app.m_activeFrameId, m_frame);

  if (m_frame.aperture() == ApertureValue::None) {
    m_frame.setAperture(ApertureValue::_8);
  }

  if (m_frame.shutter() == ShutterSpeed::None) {
    m_frame.setShutter(ShutterSpeed::_125);
  }

  if (m_frame.focal() == FocalLength::None) {
    m_frame.setFocal(FocalLength::_50);
  }

  m_editFocal = false;

  m_app.m_cameraSettingsView.setPosition(TITLE_HEIGHT + MARGIN * 2);

  updateSettingsExposure();
  drawTitle();
  drawAperture();
  drawShutter();
  drawFocal();

  uint8_t bounds[4] = {0, 42, display.width(), 20};
  m_app.m_meterView.setBounds(bounds);
}

void EditFrameState::deactivate()
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

void EditFrameState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(true);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditFrameState::onClickButtonB(uint32_t t)
{
  Persistency::saveFrame(m_app.m_activeRollId, m_app.m_activeFrameId, m_frame);

  // Upon saving go to the next frame in the roll, if it's empty
  {
    uint8_t nextFrameId = m_app.m_activeFrameId + 1;
    if (nextFrameId < N_FRAMES_PER_ROLL) {
      Frame nextFrame;
      Persistency::readFrame(m_app.m_activeRollId, nextFrameId, nextFrame);
      if (nextFrame.aperture() == ApertureValue::None) {
        m_app.m_activeFrameId = nextFrameId;

        // Give a subtle feedback to the user that the frame was saved
        auto& display = Controls::instance().display;
        display.fillRectangle(0, 0, display.width(), TEXT_HEIGHT, &Display::inversePainter);
        display.render();
        delay(100);
        drawTitle();

        return;
      }
    }
  }

  // If the next frame is not empty, return to list

  onClickButtonA(t);
}

void EditFrameState::onLongpressButtonA(uint32_t t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

void EditFrameState::onLongpressButtonB(uint32_t t)
{
  m_editFocal = !m_editFocal;
  drawFocal();
}

void EditFrameState::onRightRotaryA(uint32_t t)
{
  changeShutter(true);
}

void EditFrameState::onLeftRotaryA(uint32_t t)
{
  changeShutter(false);
}

void EditFrameState::onRightRotaryB(uint32_t t)
{
  if (m_editFocal) {
    changeFocal(true);
  } else {
    changeAperture(true);
  }
}

void EditFrameState::onLeftRotaryB(uint32_t t)
{
  if (m_editFocal) {
    changeFocal(false);
  } else {
    changeAperture(false);
  }
}

void EditFrameState::onDownFlash(uint32_t t)
{
  m_flashDownTime = t;
}

void EditFrameState::onUpFlash(uint32_t t)
{
  if (m_app.m_autoShutter) {
    uint32_t elapsed = t - m_flashDownTime;

    uint32_t minDiff = 0 - 1;
    uint8_t minIdx;
    uint32_t reference;
    uint32_t diff;

    for (uint8_t i = 0; i < 10; ++i) {
      Persistency::readShutterCalibration(i, reference);

      diff = min(reference - elapsed, elapsed - reference);

      if (diff < minDiff) {
        minDiff = diff;
        minIdx = i;
      }
    }

    m_frame.setShutter((ShutterSpeed)(minIdx + 1));
    drawShutter();
  }

  onClickButtonB(t);
}

void EditFrameState::onMeterReading(int value)
{
  m_meterExposure = meterValueToExposureValue(value, m_app.m_meterCalibration);

  m_app.m_meterView.setReading(m_meterExposure - m_settingsExposure);
}

void EditFrameState::drawTitle()
{
  auto& display = Controls::instance().display;

  display.fillRectangle(0, 0, display.width(), TEXT_HEIGHT, &Display::blackPainter);

  uint8_t x = 0;
  x = display.print(0, 0, F("Frame #"));
  x = display.print(x, 0, m_app.m_activeFrameId);
  x = display.printEmpty(x, 0, 2);
  char l[12];
  rollManufacturerAsString(m_roll.manufacturer(), l, 12);
  x = display.print(x, 0, l);
  x = display.printEmpty(x, 0, 1);
  isoValueAsString(m_roll.iso(), l, 12);
  display.print(x, 0, l);
  display.drawHLine(0, TEXT_HEIGHT + MARGIN, display.width());
}

void EditFrameState::drawAperture()
{
  m_app.m_cameraSettingsView.setLabel(1, "f/");

  char label[4];
  apertureValueAsString(m_frame.aperture(), label, 4);

  m_app.m_cameraSettingsView.setValue(1, label);
}

void EditFrameState::drawShutter()
{
  m_app.m_cameraSettingsView.setLabel(0, "Shutt");

  char label[5];
  shutterSpeedAsString(m_frame.shutter(), label, 5);

  m_app.m_cameraSettingsView.setValue(0, label);
}

void EditFrameState::drawFocal()
{
  m_app.m_cameraSettingsView.setLabel(2, "Lens", m_editFocal);

  char label[4];
  focalLengthAsString(m_frame.focal(), label, 4);

  m_app.m_cameraSettingsView.setValue(2, label);
}

void EditFrameState::changeAperture(bool increase)
{
  auto aperture = m_frame.aperture();

  if (increase) {
    if (aperture < ApertureValue::_22) {
      m_frame.setAperture((ApertureValue)(1 + (uint8_t)aperture));
      updateSettingsExposure();
      drawAperture();
    }
  } else {
    if (aperture > ApertureValue::_1_4) {
      m_frame.setAperture((ApertureValue)((uint8_t)aperture - 1));
      updateSettingsExposure();
      drawAperture();
    }
  }
}

void EditFrameState::changeShutter(bool increase)
{
  auto shutter = m_frame.shutter();

  if (increase) {
    if (shutter < ShutterSpeed::_1000) {
      m_frame.setShutter((ShutterSpeed)(1 + (uint8_t)shutter));
      updateSettingsExposure();
      drawShutter();
    }
  } else {
    if (shutter > ShutterSpeed::_2) {
      m_frame.setShutter((ShutterSpeed)((uint8_t)shutter - 1));
      updateSettingsExposure();
      drawShutter();
    }
  }
}

void EditFrameState::changeFocal(bool increase)
{
  auto focal = m_frame.focal();

  if (increase) {
    if (focal < FocalLength::Other) {
      m_frame.setFocal((FocalLength)(1 + (uint8_t)focal));
      drawFocal();
    }
  } else {
    if (focal > FocalLength::_20) {
      m_frame.setFocal((FocalLength)((uint8_t)focal - 1));
      drawFocal();
    }
  }
}

void EditFrameState::updateSettingsExposure()
{
  m_settingsExposure = cameraSettingsToExposureValue(m_frame.shutter(), m_frame.aperture(), m_roll.iso());
  m_app.m_meterView.setReading(m_meterExposure - m_settingsExposure);
}

// EditModalState

EditModalState::EditModalState(App& app)
  : AppState(app)
{
  m_actions[0].type = ActionType::Edit;
  m_actions[1].type = ActionType::Delete;
  m_actions[2].type = ActionType::Cancel;
}

EditModalState::~EditModalState()
{}

void EditModalState::activate()
{
  auto& display = Controls::instance().display;

  const uint8_t padding = 6;

  uint8_t bounds[4] = {padding, padding, display.width() - 2 * padding, display.height() - 2 * padding};

  display.fillRectangle(bounds[0] - 4, bounds[1] - 4, bounds[0] + bounds[2] + 4, bounds[1] + bounds[3] + 4, &Display::blackPainter);
  display.drawRectangle(bounds[0] - 2, bounds[1] - 2, bounds[0] + bounds[2] + 2, bounds[1] + bounds[3] + 2);

  m_app.m_listView.setBounds(bounds);
  char title[22];

  if (m_isFrameTarget) {
    strncpy_P(title, PSTR("Frame"), 22);
    Frame frame;
    Persistency::readFrame(m_app.m_activeRollId, m_app.m_activeFrameId, frame);
    frame.asString(m_app.m_activeFrameId, title + 5, 17);
  } else {
    strncpy_P(title, PSTR("Roll"), 22);
    Roll roll;
    Persistency::readRoll(m_app.m_activeRollId, roll);
    roll.asString(m_app.m_activeRollId, title + 4, 18);
  }

  m_app.m_listView.setTitle(title);

  ListView::ItemGetter itemGetter;
  itemGetter.boundObj = this;
  itemGetter.getter = &EditModalState::itemGetter;

  m_app.m_listView.setItems(itemGetter, 3);
}

void EditModalState::deactivate()
{}

void EditModalState::setIsFrameTarget(bool isFrameTarget)
{
  m_isFrameTarget = isFrameTarget;
}

void EditModalState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(m_isFrameTarget);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditModalState::onClickButtonB(uint32_t t)
{
  auto& selectedAction = m_actions[m_app.m_listView.selected()];

  if (selectedAction.type == ActionType::Edit) {

    m_app.m_state->deactivate();
    if (m_isFrameTarget) {
      m_app.m_state = &m_app.m_editFrameState;
    } else {
      m_app.m_state = &m_app.m_editRollState;
    }
    m_app.m_state->activate();

  } else if (selectedAction.type == ActionType::Delete) {

    m_app.m_state->deactivate();

    Roll emptyRoll;
    emptyRoll.setManufacturer(RollManufacturer::None);
    emptyRoll.setIso(ISOValue::None);

    Frame emptyFrame;
    emptyFrame.setAperture(ApertureValue::None);
    emptyFrame.setShutter(ShutterSpeed::None);
    emptyFrame.setFocal(FocalLength::None);

    if (m_isFrameTarget) {
      Persistency::saveFrame(m_app.m_activeRollId, m_app.m_activeFrameId, emptyFrame);
    } else {
      Roll emptyRoll;
      Persistency::saveRoll(m_app.m_activeRollId, emptyRoll);

      for (uint8_t i = 0; i < N_FRAMES_PER_ROLL; ++i) {
        Persistency::saveFrame(m_app.m_activeRollId, i, emptyFrame);
      }
    }

    m_app.m_listState.setIsFrameTarget(m_isFrameTarget);
    m_app.m_state = &m_app.m_listState;
    m_app.m_state->activate();
  } else {

    m_app.m_state->deactivate();
    m_app.m_listState.setIsFrameTarget(m_isFrameTarget);
    m_app.m_state = &m_app.m_listState;
    m_app.m_state->activate();
  }
}

void EditModalState::onRightRotaryA(uint32_t t)
{
  m_app.m_listView.selectNext();
}

void EditModalState::onLeftRotaryA(uint32_t t)
{
  m_app.m_listView.selectPrev();
}

void EditModalState::itemGetter(void* _self, uint8_t i, char* s, uint8_t n)
{
  auto self = static_cast<EditModalState*>(_self);
  self->m_actions[i].asString(i, s, n);
}

// AboutState

AboutState::AboutState(App& app)
  : AppState(app)
{}

AboutState::~AboutState()
{}

void AboutState::activate()
{
  auto& display = Controls::instance().display;

  display.clear();

  display.print(21, 8, F("Copyright 2023"));
  display.print(12, 20, F("Alessandro Genova"));
}

void AboutState::deactivate()
{}

void AboutState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_state = &m_app.m_settingsState;
  m_app.m_state->activate();
}

// LightMeterState

LightMeterState::LightMeterState(App& app)
  : AppState(app)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

LightMeterState::~LightMeterState()
{}

void LightMeterState::activate()
{
  auto& display = Controls::instance().display;

  display.clear();

  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);

  m_editISO = false;

  m_app.m_cameraSettingsView.setPosition(TITLE_HEIGHT + MARGIN * 2);

  updateSettingsExposure();
  drawTitle();
  drawAperture();
  drawShutter();
  drawISO();

  uint8_t bounds[4] = {0, 42, display.width(), 20};
  m_app.m_meterView.setBounds(bounds);
}

void LightMeterState::deactivate()
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

void LightMeterState::onClickButtonA(uint32_t t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

void LightMeterState::onLongpressButtonA(uint32_t t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Continuous);
}

void LightMeterState::onClickButtonB(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void LightMeterState::onLongpressButtonB(uint32_t t)
{
  m_editISO = !m_editISO;
  drawISO();
}

void LightMeterState::onRightRotaryA(uint32_t t)
{
  changeShutter(true);
}

void LightMeterState::onLeftRotaryA(uint32_t t)
{
  changeShutter(false);
}

void LightMeterState::onRightRotaryB(uint32_t t)
{
  if (m_editISO) {
    changeISO(true);
  } else {
    changeAperture(true);
  }
}

void LightMeterState::onLeftRotaryB(uint32_t t)
{
  if (m_editISO) {
    changeISO(false);
  } else {
    changeAperture(false);
  }
}

void LightMeterState::onMeterReading(int value)
{
  m_meterExposure = meterValueToExposureValue(value, m_app.m_meterCalibration);

  m_app.m_meterView.setReading(m_meterExposure - m_settingsExposure);
}

void LightMeterState::drawTitle()
{
  auto& display = Controls::instance().display;

  display.print(0, 0, F("Light Meter"));
  display.drawHLine(0, TEXT_HEIGHT + MARGIN, display.width());
}

void LightMeterState::drawAperture()
{
  m_app.m_cameraSettingsView.setLabel(1, "f/");

  char label[4];
  apertureValueAsString(m_aperture, label, 4);

  m_app.m_cameraSettingsView.setValue(1, label);
}

void LightMeterState::drawShutter()
{
  m_app.m_cameraSettingsView.setLabel(0, "Shutt");

  char label[5];
  shutterSpeedAsString(m_shutter, label, 5);

  m_app.m_cameraSettingsView.setValue(0, label);
}

void LightMeterState::drawISO()
{
  m_app.m_cameraSettingsView.setLabel(2, "ISO", m_editISO);

  char label[5];
  isoValueAsString(m_iso, label, 5);

  m_app.m_cameraSettingsView.setValue(2, label);
}

void LightMeterState::changeAperture(bool increase)
{
  if (increase) {
    if (m_aperture < ApertureValue::_22) {
      m_aperture = (ApertureValue)(1 + (uint8_t)m_aperture);
      updateSettingsExposure();
      drawAperture();
    }
  } else {
    if (m_aperture > ApertureValue::_1_4) {
      m_aperture = (ApertureValue)((uint8_t)m_aperture - 1);
      updateSettingsExposure();
      drawAperture();
    }
  }
}

void LightMeterState::changeShutter(bool increase)
{
  if (increase) {
    if (m_shutter < ShutterSpeed::_1000) {
      m_shutter = (ShutterSpeed)(1 + (uint8_t)m_shutter);
      updateSettingsExposure();
      drawShutter();
    }
  } else {
    if (m_shutter > ShutterSpeed::_2) {
      m_shutter = (ShutterSpeed)((uint8_t)m_shutter - 1);
      updateSettingsExposure();
      drawShutter();
    }
  }
}

void LightMeterState::changeISO(bool increase)
{
  if (increase) {
    if (m_iso < ISOValue::_1600) {
      m_iso = (ISOValue)(1 + (uint8_t)m_iso);
      updateSettingsExposure();
      drawISO();
    }
  } else {
    if (m_iso > ISOValue::_100) {
      m_iso = (ISOValue)((uint8_t)m_iso - 1);
      updateSettingsExposure();
      drawISO();
    }
  }
}

void LightMeterState::updateSettingsExposure()
{
  m_settingsExposure = cameraSettingsToExposureValue(m_shutter, m_aperture, m_iso);
  m_app.m_meterView.setReading(m_meterExposure - m_settingsExposure);
}

// SettingsState

SettingsState::SettingsState(App& app)
  : AppState(app)
{
  m_actions[0].type = ActionType::CalibrateMeter;
  m_actions[1].type = ActionType::EnableHotShoeShutter;
  m_actions[2].type = ActionType::CalibrateHotShoeShutter;
  m_actions[3].type = ActionType::About;
}

SettingsState::~SettingsState()
{}

void SettingsState::activate()
{
  auto& display = Controls::instance().display;

  display.clear();

  const uint8_t padding = 6;
  uint8_t bounds[4] = {0, 0, 107, 64};

  display.drawHLine(107, 9, 127);

  m_app.m_listView.setBounds(bounds);

  m_app.m_listView.setTitle("Settings");

  ListView::ItemGetter itemGetter;
  itemGetter.boundObj = this;
  itemGetter.getter = &SettingsState::itemGetter;

  m_app.m_listView.setItems(itemGetter, 4);

  drawAutoShutter();
  drawMeterCalibration();

  display.print(122, 29, F("~"));
  display.print(122, 38, F("~"));
}

void SettingsState::deactivate()
{
  Persistency::readMeterCalibration(m_app.m_meterCalibration);
}

void SettingsState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void SettingsState::onClickButtonB(uint32_t t)
{
  Action selectedAction = m_actions[m_app.m_listView.selected()];

  switch (selectedAction.type) {
    case ActionType::About: {
      m_app.m_state->deactivate();
      m_app.m_state = &m_app.m_aboutState;
      m_app.m_state->activate();
      return;
    }
    case ActionType::EnableHotShoeShutter: {
      changeAutoShutter();
      return;
    }
    case ActionType::CalibrateHotShoeShutter: {
      m_app.m_state->deactivate();
      m_app.m_state = &m_app.m_calibrateState;
      m_app.m_state->activate();
      return;
    }
    case ActionType::CalibrateMeter: {
      Persistency::writeMeterCalibration(m_app.m_meterCalibration);
      return;
    }
    default: {
      return;
    }
  }
}

void SettingsState::onRightRotaryA(uint32_t t)
{
  m_app.m_listView.selectNext();
}

void SettingsState::onLeftRotaryA(uint32_t t)
{
  m_app.m_listView.selectPrev();
}

void SettingsState::onRightRotaryB(uint32_t t)
{
  Action selectedAction = m_actions[m_app.m_listView.selected()];

  if (selectedAction.type == ActionType::CalibrateMeter && m_app.m_meterCalibration <= 9.8) {
    m_app.m_meterCalibration += 0.1;
    drawMeterCalibration();
  }
}

void SettingsState::onLeftRotaryB(uint32_t t)
{
  Action selectedAction = m_actions[m_app.m_listView.selected()];

  if (selectedAction.type == ActionType::CalibrateMeter && m_app.m_meterCalibration >= 0.2) {
    m_app.m_meterCalibration -= 0.1;
    drawMeterCalibration();
  }
}

void SettingsState::drawAutoShutter()
{
  auto& display = Controls::instance().display;

  const uint8_t x0 = 120;
  const uint8_t y0 = 20;
  const uint8_t w = 7;

  display.drawRectangle(x0, y0, x0 + w, y0 + w);

  Display::Painter fill;

  if (m_app.m_autoShutter) {
    fill = &Display::whitePainter;
  } else {
    fill = &Display::blackPainter;
  }

  display.fillRectangle(x0 + 2, y0 + 2, x0 + w - 2, y0 + w - 2, fill);
}

void SettingsState::drawMeterCalibration()
{
  auto& display = Controls::instance().display;

  uint8_t x = 110;
  const uint8_t y0 = 11;

  const uint8_t integerPart = m_app.m_meterCalibration + 0.01;
  const uint8_t decimalPart = round(10 * (m_app.m_meterCalibration - integerPart));

  x = display.print(x, y0, integerPart);
  x = display.print(x, y0, F("."));
  x = display.print(x, y0, decimalPart);
}

void SettingsState::changeAutoShutter()
{
  m_app.m_autoShutter = !m_app.m_autoShutter;
  Persistency::writeAutoShutter(m_app.m_autoShutter);
  drawAutoShutter();
}

void SettingsState::itemGetter(void* _self, uint8_t i, char* s, uint8_t n)
{
  auto self = static_cast<SettingsState*>(_self);
  self->m_actions[i].asString(i, s, n);
}

// CalibrateState

CalibrateState::CalibrateState(App& app)
  : AppState(app)
{}

CalibrateState::~CalibrateState()
{}

void CalibrateState::activate()
{
  auto& display = Controls::instance().display;
  display.clear();

  m_shutter = ShutterSpeed::_2;
  m_cumulatedTime = 0;
  m_currRepetition = 0;

  drawTitle();
  drawShutter();
  drawProgress();
}

void CalibrateState::deactivate()
{}

void CalibrateState::onClickButtonA(uint32_t t)
{
  m_app.m_state->deactivate();
  m_app.m_state = &m_app.m_settingsState;
  m_app.m_state->activate();
}

void CalibrateState::onClickButtonB(uint32_t t)
{
  m_cumulatedTime = 0;
  nextShutter();
  drawProgress();
}

void CalibrateState::nextShutter()
{
  uint8_t index = shutterToIndex(m_shutter);
  Persistency::writeShutterCalibration(index, m_cumulatedTime / 3);

  if (m_shutter == ShutterSpeed::_1000) {
    onClickButtonA(0);
  } else {
    m_shutter = (ShutterSpeed)(1 + (uint8_t)m_shutter);
    m_cumulatedTime = 0;
    m_currRepetition = 0;
    drawShutter();
  }
}

void CalibrateState::onDownFlash(uint32_t t)
{
  m_downTime = t;
}

void CalibrateState::onUpFlash(uint32_t t)
{
  m_cumulatedTime += (t - m_downTime);

  ++m_currRepetition;

  if (m_currRepetition == 3) {
    nextShutter();
  }

  drawProgress();
}

void CalibrateState::drawTitle()
{
  auto& display = Controls::instance().display;

  display.print(0, 0, F("Calibrate Shutter"));

  display.drawHLine(0, 10, display.width() - 1);

  display.print(0, 16, F("Fire shutter 3 times:"));
}

void CalibrateState::drawShutter()
{
  auto& display = Controls::instance().display;

  display.printEmpty(0, 36, 4, &Display::blackPainter, 2, 2);

  char label[5];
  shutterSpeedAsString(m_shutter, label, 5);

  display.print(0, 36, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

void CalibrateState::drawProgress()
{
  auto& display = Controls::instance().display;

  const uint8_t x0 = 2;
  const uint8_t x1 = display.width() - 1 - x0;
  const uint8_t y0 = 54;
  const uint8_t h = 8;
  const uint8_t y1 = y0 + h - 1;
  const uint8_t pixelPerStep = 4;

  display.drawRectangle(x0, y0, x1, y1);

  uint8_t index = shutterToIndex(m_shutter);
  const uint8_t step = 3 * index + m_currRepetition;
  const uint8_t progressX = step * pixelPerStep;

  display.fillRectangle(x0 + 2, y0 + 2, x0 + 2 + progressX, y1 - 2);
  display.fillRectangle(x0 + 2 + progressX, y0 + 2, x1 - 2, y1 - 2, &Display::blackPainter);
}

uint8_t CalibrateState::shutterToIndex(ShutterSpeed shutter)
{
  return (uint8_t)shutter - 1;
}

#endif
