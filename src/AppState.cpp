#include "AppState.h"

#include <WString.h>

#include "App.h"
#include "Controls.h"

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

void DebugAppState::onClickButtonA(int t)
{
  m_buttonA = !m_buttonA;
  drawButtonA();
}

void DebugAppState::onLongpressButtonA(int t)
{
  m_buttonA = !m_buttonA;
  drawButtonA();
}

void DebugAppState::onClickButtonB(int t)
{
  m_buttonB = !m_buttonB;
  drawButtonB();

  auto& meter = Controls::instance().meter;
  if (meter.mode() == MeterMode::Single) {
    meter.takeReading();
  }
}

void DebugAppState::onLongpressButtonB(int t)
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

void DebugAppState::onRightRotaryA(int t)
{
  if (m_rotaryA == 16) {
    return;
  }

  m_rotaryA += 1;
  drawRotaryA();
}

void DebugAppState::onLeftRotaryA(int t)
{
  if (m_rotaryA == 0) {
    return;
  }

  m_rotaryA -= 1;
  drawRotaryA();
}

void DebugAppState::onRightRotaryB(int t)
{
  if (m_rotaryB == 16) {
    return;
  }

  m_rotaryB += 1;
  drawRotaryB();
}

void DebugAppState::onLeftRotaryB(int t)
{
  if (m_rotaryB == 0) {
    return;
  }

  m_rotaryB -= 1;
  drawRotaryB();
}

void DebugAppState::onClickFlash(int t)
{}

void DebugAppState::onLongpressFlash(int t)
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

void ListState::onClickButtonA(int t)
{
  if (m_isFrameTarget) {
    m_app.m_state->deactivate();
    m_app.m_listState.setIsFrameTarget(false);
    m_app.m_state = &m_app.m_listState;
    m_app.m_state->activate();
  } else {
    m_app.m_state->deactivate();
    m_app.m_state = &m_app.m_meterState;
    m_app.m_state->activate();
  }
}

void ListState::onLongpressButtonA(int t)
{
  if (!m_isFrameTarget) {
    m_app.m_state->deactivate();
    m_app.m_state = &m_app.m_aboutState;
    m_app.m_state->activate();
  }
}

void ListState::onClickButtonB(int t)
{
  if (m_isFrameTarget) {
    m_app.m_activeFrameId = m_app.m_listView.selected();

    m_app.m_state->deactivate();
    m_app.m_state = &m_app.m_editFrameState;
    m_app.m_state->activate();

  } else {
    m_app.m_activeRollId = m_app.m_listView.selected();
    m_app.m_activeFrameId = 0;

    Roll selectedRoll;
    Persistency::readRoll(m_app.m_activeRollId, selectedRoll);

    if (selectedRoll.manufacturer() == RollManufacturer::None) {
      m_app.m_state->deactivate();
      m_app.m_state = &m_app.m_editRollState;
      m_app.m_state->activate();
    } else {
      m_app.m_state->deactivate();
      m_app.m_listState.setIsFrameTarget(true);
      m_app.m_state = &m_app.m_listState;
      m_app.m_state->activate();
    }
  }
}

void ListState::onLongpressButtonB(int t)
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

void ListState::onRightRotaryA(int t)
{
  m_app.m_listView.selectNext();
}

void ListState::onLeftRotaryA(int t)
{
  m_app.m_listView.selectPrev();
}

void ListState::onRightRotaryB(int t)
{
  m_app.m_listView.selectNext(10);
}

void ListState::onLeftRotaryB(int t)
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

  Roll activeRoll;
  Persistency::readRoll(m_app.m_activeRollId, activeRoll);

  if (activeRoll.manufacturer() == RollManufacturer::None) {
    m_roll.setManufacturer(RollManufacturer::Fuji);
  } else {
    m_roll.setManufacturer(activeRoll.manufacturer());
  }

  if (activeRoll.iso() == ISOValue::None) {
    m_roll.setIso(ISOValue::_400);
  } else {
    m_roll.setIso(activeRoll.iso());
  }

  drawTitle();
  drawManufacturer();
  drawISO();
}

void EditRollState::deactivate()
{}

void EditRollState::onClickButtonA(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditRollState::onClickButtonB(int t)
{
  Persistency::saveRoll(m_app.m_activeRollId, m_roll);

  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditRollState::onRightRotaryA(int t)
{
  changeManufacturer(true);
}

void EditRollState::onLeftRotaryA(int t)
{
  changeManufacturer(false);
}

void EditRollState::onRightRotaryB(int t)
{
  changeISO(true);
}

void EditRollState::onLeftRotaryB(int t)
{
  changeISO(false);
}

void EditRollState::changeManufacturer(bool increase)
{
  auto manufacturer = m_roll.manufacturer();
  if (increase) {
    if (manufacturer < RollManufacturer::Other) {
      m_roll.setManufacturer((RollManufacturer)(1 + (int)manufacturer));
      drawManufacturer();
    }
  } else {
    if (manufacturer > RollManufacturer::Fuji) {
      m_roll.setManufacturer((RollManufacturer)((int)manufacturer - 1));
      drawManufacturer();
    }
  }
}

void EditRollState::changeISO(bool increase)
{
  auto iso = m_roll.iso();

  if (increase) {
    if (iso < ISOValue::_1600) {
      m_roll.setIso((ISOValue)(1 + (int)iso));
      drawISO();
    }
  } else {
    if (iso > ISOValue::_100) {
      m_roll.setIso((ISOValue)((int)iso - 1));
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

  meter.takeReading();

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

  updateSettingsExposure();
  drawTitle();
  drawSeparators();
  drawAperture();
  drawShutter();
  drawFocal();
  // drawEV();
  drawScale();
  drawReading();
}

void EditFrameState::deactivate()
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
}

void EditFrameState::onClickButtonA(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(true);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditFrameState::onClickButtonB(int t)
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

  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(true);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditFrameState::onLongpressButtonA(int t)
{
  auto& meter = Controls::instance().meter;
  meter.takeReading();
}

void EditFrameState::onLongpressButtonB(int t)
{
  m_editFocal = !m_editFocal;
  drawFocal();
}

void EditFrameState::onRightRotaryA(int t)
{
  changeShutter(true);
}

void EditFrameState::onLeftRotaryA(int t)
{
  changeShutter(false);
}

void EditFrameState::onRightRotaryB(int t)
{
  if (m_editFocal) {
    changeFocal(true);
  } else {
    changeAperture(true);
  }
}

void EditFrameState::onLeftRotaryB(int t)
{
  if (m_editFocal) {
    changeFocal(false);
  } else {
    changeAperture(false);
  }
}

void EditFrameState::onClickFlash(int t)
{
  onClickButtonB(t);
}

void EditFrameState::onLongpressFlash(int t)
{
  onClickButtonB(t);
}

void EditFrameState::onMeterReading(int value)
{
  m_meterExposure = meterValueToExposureValue(value);
  
  // drawEV();
  drawReading();
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

void EditFrameState::drawSeparators()
{
  auto& display = Controls::instance().display;

  const uint8_t y0 = TITLE_HEIGHT + MARGIN * 2;
  const uint8_t y1 = y0 + 3 * TEXT_HEIGHT + MARGIN;

  display.drawVLine(49, y0, y1);
  display.drawVLine(90, y0, y1);
}

void EditFrameState::drawAperture()
{
  auto& display = Controls::instance().display;

  const uint8_t HALF_WIDTH = 71;

  uint8_t x = HALF_WIDTH - 6;
  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  display.print(x, y, F("f/"));

  char label[4];
  apertureValueAsString(m_frame.aperture(), label, 4);

  x = HALF_WIDTH - 6 * strlen(label);
  y += TEXT_HEIGHT + MARGIN;

  display.printEmpty(HALF_WIDTH - 18, y, 3, &Display::blackPainter, 2, 2);
  display.print(x, y, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

void EditFrameState::drawShutter()
{
  auto& display = Controls::instance().display;

  uint8_t x = 0;
  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  display.print(x, y, F("Shutter"));

  char label[5];
  shutterSpeedAsString(m_frame.shutter(), label, 5);

  y += TEXT_HEIGHT + MARGIN;

  display.printEmpty(0, y, 4, &Display::blackPainter, 2, 2);
  display.print(x, y, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

void EditFrameState::drawFocal()
{
  auto& display = Controls::instance().display;

  uint8_t x = display.width() - 24 + 1;
  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  Display::Painter color = &Display::whitePainter;
  Display::Painter bg = &Display::blackPainter;

  if (m_editFocal) {
    color = &Display::blackPainter;
    bg = &Display::whitePainter;
  }

  display.print(x, y, F("Lens"), color, bg);

  char label[4];
  focalLengthAsString(m_frame.focal(), label, 4);

  x = display.width() - 12 * strlen(label) + 2;
  y += TEXT_HEIGHT + MARGIN;

  display.printEmpty(display.width() - 12 * 3 + 2, y, 3, &Display::blackPainter, 2, 2);
  display.print(x, y, label, &Display::whitePainter, &Display::blackPainter, 2, 2);
}

// void EditFrameState::drawEV()
// {
//   auto& display = Controls::instance().display;

//   display.setCursor(116, 20);
//   display.print("EV");
//   display.setCursor(116,30);
//   display.print("  ");
//   display.setCursor(116,30);
//   display.print(int(round(m_meterExposure)));

//   display.display();
// }

void EditFrameState::drawScale()
{
  auto& display = Controls::instance().display;

  const uint8_t Y = 50;
  const uint8_t PADDING = 8;
  const uint8_t PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const uint8_t TICK_SIZE = 3;

  display.drawHLine(PADDING, Y, PADDING + 6 * PIXELS_PER_EV);

  for (uint8_t i = 0; i < 7; ++i) {
    display.drawVLine(PADDING + i * PIXELS_PER_EV, Y - TICK_SIZE, Y + TICK_SIZE);

    uint8_t x = i * PIXELS_PER_EV;
    uint8_t y = Y + TICK_SIZE + 2;

    auto delta = i - 3;

    if (delta == 0) {
      display.printEmpty(x, y, 1);
      x += 8;
    } else if (delta > 0) {
      display.print(x, y, "+");
      x += 8;
    }

    display.print(x, y, i - 3);
  }
}

void EditFrameState::drawReading()
{
  auto& display = Controls::instance().display;

  const int Y = 50;
  const int PADDING = 8;
  const int PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const int TICK_SIZE = 3;

  float exposure_diff = m_meterExposure - m_settingsExposure;

  exposure_diff = min(3, exposure_diff);
  exposure_diff = max(-3, exposure_diff);

  const uint8_t centerX = PADDING + (exposure_diff + 3) * PIXELS_PER_EV;
  const uint8_t centerY = Y - TICK_SIZE - 3;

  display.fillRectangle(0, Y - TICK_SIZE - 5, display.width(), Y - TICK_SIZE, &Display::blackPainter);
  display.fillRectangle(centerX - 2, centerY - 2, centerX + 2, centerY + 2);
}

void EditFrameState::changeAperture(bool increase)
{
  auto aperture = m_frame.aperture();

  if (increase) {
    if (aperture < ApertureValue::_22) {
      m_frame.setAperture((ApertureValue)(1 + (int)aperture));
      updateSettingsExposure();
      drawAperture();
    }
  } else {
    if (aperture > ApertureValue::_1_4) {
      m_frame.setAperture((ApertureValue)((int)aperture - 1));
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
      m_frame.setShutter((ShutterSpeed)(1 + (int)shutter));
      updateSettingsExposure();
      drawShutter();
    }
  } else {
    if (shutter > ShutterSpeed::_2) {
      m_frame.setShutter((ShutterSpeed)((int)shutter - 1));
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
      m_frame.setFocal((FocalLength)(1 + (int)focal));
      drawFocal();
    }
  } else {
    if (focal > FocalLength::_20) {
      m_frame.setFocal((FocalLength)((int)focal - 1));
      drawFocal();
    }
  }
}

void EditFrameState::updateSettingsExposure()
{
  m_settingsExposure = cameraSettingsToExposureValue(m_frame.shutter(), m_frame.aperture(), m_roll.iso());
  drawReading();
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

void EditModalState::onClickButtonA(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(m_isFrameTarget);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditModalState::onClickButtonB(int t)
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

void EditModalState::onRightRotaryA(int t)
{
  m_app.m_listView.selectNext();
}

void EditModalState::onLeftRotaryA(int t)
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

void AboutState::onClickButtonB(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
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
  Controls::instance().display.clear();

  updateSettingsExposure();
  drawAperture();
  drawShutter();
  drawEV();
  drawISO();
  drawScale();
  drawReading();
}

void LightMeterState::deactivate()
{}

void LightMeterState::onClickButtonA(int t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Single);
  meter.takeReading();
}

void LightMeterState::onLongpressButtonA(int t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Continuous);
}

void LightMeterState::onClickButtonB(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void LightMeterState::onLongpressButtonB(int t)
{
  m_editISO = !m_editISO;
  drawISO();
}

void LightMeterState::onRightRotaryA(int t)
{
  changeAperture(true);
}

void LightMeterState::onLeftRotaryA(int t)
{
  changeAperture(false);
}

void LightMeterState::onRightRotaryB(int t)
{
  if (m_editISO) {
    changeISO(true);
  } else {
    changeShutter(true);
  }
}

void LightMeterState::onLeftRotaryB(int t)
{
  if (m_editISO) {
    changeISO(false);
  } else {
    changeShutter(false);
  }
}

void LightMeterState::onMeterReading(int value)
{
  m_meterExposure = meterValueToExposureValue(value);
  
  drawEV();
  drawReading();
}

void LightMeterState::drawAperture()
{
  auto& display = Controls::instance().display;

  display.print(0, 0, F("Aperture"));

  char label[4];
  apertureValueAsString(m_aperture, label, 4);

  display.printEmpty(0, 10, 4);
  display.print(0, 10, label);
}

void LightMeterState::drawShutter()
{
  auto& display = Controls::instance().display;

  display.print(0, 20, F("Shutter"));

  char label[5];
  shutterSpeedAsString(m_shutter, label, 5);

  display.printEmpty(0, 30, 5);
  display.print(0, 30, label);
}

void LightMeterState::drawISO()
{
  auto& display = Controls::instance().display;

  Display::Painter color = &Display::whitePainter;
  Display::Painter bg = &Display::blackPainter;

  if (m_editISO) {
    color = &Display::blackPainter;
    bg = &Display::whitePainter;
  }

  display.print(110, 0, F("ISO"), color, bg);

  char label[5];
  isoValueAsString(m_iso, label, 5);

  display.printEmpty(100, 10, 5);
  display.print(100, 10, label);  
}

void LightMeterState::drawEV()
{
  auto& display = Controls::instance().display;

  display.print(116, 20, F("EV"));

  char label[4];
  itoa(int(round(m_meterExposure)), label, 10);
  display.printEmpty(116, 30, 4);
  display.print(116, 30, label);
}

void LightMeterState::drawScale()
{
  auto& display = Controls::instance().display;

  const uint8_t Y = 50;
  const uint8_t PADDING = 8;
  const uint8_t PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const uint8_t TICK_SIZE = 3;

  display.drawHLine(PADDING, Y, PADDING + 6 * PIXELS_PER_EV);

  for (uint8_t i = 0; i < 7; ++i) {
    display.drawVLine(PADDING + i * PIXELS_PER_EV, Y - TICK_SIZE, Y + TICK_SIZE);

    uint8_t x = i * PIXELS_PER_EV;
    uint8_t y = Y + TICK_SIZE + 2;

    auto delta = i - 3;

    if (delta == 0) {
      display.printEmpty(x, y, 1);
      x += 8;
    } else if (delta > 0) {
      display.print(x, y, "+");
      x += 8;
    }

    display.print(x, y, i - 3);
  }
}

void LightMeterState::drawReading()
{
  auto& display = Controls::instance().display;

  const int Y = 50;
  const int PADDING = 8;
  const int PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const int TICK_SIZE = 3;

  float exposure_diff = m_meterExposure - m_settingsExposure;

  exposure_diff = min(3, exposure_diff);
  exposure_diff = max(-3, exposure_diff);

  const uint8_t centerX = PADDING + (exposure_diff + 3) * PIXELS_PER_EV;
  const uint8_t centerY = Y - TICK_SIZE - 3;

  display.fillRectangle(0, Y - TICK_SIZE - 5, display.width(), Y - TICK_SIZE, &Display::blackPainter);
  display.fillRectangle(centerX - 2, centerY - 2, centerX + 2, centerY + 2);
}

void LightMeterState::changeAperture(bool increase)
{
  if (increase) {
    if (m_aperture < ApertureValue::_22) {
      m_aperture = (ApertureValue)(1 + (int)m_aperture);
      updateSettingsExposure();
      drawAperture();
    }
  } else {
    if (m_aperture > ApertureValue::_1_4) {
      m_aperture = (ApertureValue)((int)m_aperture - 1);
      updateSettingsExposure();
      drawAperture();
    }
  }
}

void LightMeterState::changeShutter(bool increase)
{
  if (increase) {
    if (m_shutter < ShutterSpeed::_1000) {
      m_shutter = (ShutterSpeed)(1 + (int)m_shutter);
      updateSettingsExposure();
      drawShutter();
    }
  } else {
    if (m_shutter > ShutterSpeed::_2) {
      m_shutter = (ShutterSpeed)((int)m_shutter - 1);
      updateSettingsExposure();
      drawShutter();
    }
  }
}

void LightMeterState::changeISO(bool increase)
{
  if (increase) {
    if (m_iso < ISOValue::_1600) {
      m_iso = (ISOValue)(1 + (int)m_iso);
      updateSettingsExposure();
      drawISO();
    }
  } else {
    if (m_iso > ISOValue::_100) {
      m_iso = (ISOValue)((int)m_iso - 1);
      updateSettingsExposure();
      drawISO();
    }
  }
}

void LightMeterState::updateSettingsExposure()
{
  m_settingsExposure = cameraSettingsToExposureValue(m_shutter, m_aperture, m_iso);
  drawReading();
}

#endif
