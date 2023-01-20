#include "AppState.h"

#include "App.h"
#include "Controls.h"

#if __MEMO
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
  Controls::instance().display.clearDisplay();
  Controls::instance().display.display();
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

  uint8_t color = m_buttonA ? SSD1306_BLACK : SSD1306_WHITE;
  uint8_t bg_color = m_buttonA ? SSD1306_WHITE : SSD1306_BLACK;
  display.setTextColor(color, bg_color);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print("Button A");

  display.display();
}

void DebugAppState::drawButtonB()
{
  auto& display = Controls::instance().display;

  uint8_t color = m_buttonB ? SSD1306_BLACK : SSD1306_WHITE;
  uint8_t bg_color = m_buttonB ? SSD1306_WHITE : SSD1306_BLACK;
  display.setTextColor(color, bg_color);        // Draw white text
  display.setCursor(0,20);             // Start at top-left corner
  display.print("Button B");

  display.display();
}

void DebugAppState::drawRotaryA()
{
  auto& display = Controls::instance().display;

  display.drawFastHLine(0, 12, display.width(), SSD1306_BLACK);
  display.drawFastHLine(0, 12, m_rotaryA * 8, SSD1306_WHITE);

  display.display();
}

void DebugAppState::drawRotaryB()
{
  auto& display = Controls::instance().display;

  display.drawFastHLine(0, 32, display.width(), SSD1306_BLACK);
  display.drawFastHLine(0, 32, m_rotaryB * 8, SSD1306_WHITE);

  display.display();
}

void DebugAppState::drawMeter()
{
  auto& display = Controls::instance().display;

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);        // Draw white text
  display.setCursor(0,40);             // Start at top-left corner
  display.print("Meter");

  display.drawFastHLine(0, 52, display.width(), SSD1306_BLACK);
  display.drawFastHLine(0, 52, (float(m_meterValue) / 1023) * display.width(), SSD1306_WHITE);

  display.setCursor(0, 55);
  display.print(m_meterValue);
  display.print("    ");

  display.display();
}

#elif __METER

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
  updateSettingsExposure();
  drawAperture();
  drawShutter();
  drawEV();
  drawISO();
  drawScale();
  drawReading();
}

void LightMeterState::deactivate()
{
  Controls::instance().display.clearDisplay();
  Controls::instance().display.display();
}

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
  m_editISO = !m_editISO;
  drawISO();
}

void LightMeterState::onLongpressButtonB(int t)
{
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

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0,0);
  display.print("Aperture");
  display.setCursor(0,10);
  char label[4];
  apertureValueAsString(m_aperture, label, 4);

  display.print(label);

  display.display();
}

void LightMeterState::drawShutter()
{
  auto& display = Controls::instance().display;

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0,20);
  display.print("Shutter");
  display.setCursor(0,30);
  char label[5];
  shutterSpeedAsString(m_shutter, label, 5);

  display.print(label);

  display.display();
}

void LightMeterState::drawISO()
{
  auto& display = Controls::instance().display;

  uint8_t color = m_editISO ? SSD1306_BLACK : SSD1306_WHITE;
  uint8_t bg_color = m_editISO ? SSD1306_WHITE : SSD1306_BLACK;

  display.setTextColor(color, bg_color);
  display.setCursor(110, 0);
  display.print("ISO");
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(100,10);
  char label[5];
  isoValueAsString(m_iso, label, 5);

  display.print(label);

  display.display();
}

void LightMeterState::drawEV()
{
  auto& display = Controls::instance().display;

  display.setCursor(116, 20);
  display.print("EV");
  display.setCursor(116,30);
  display.print("  ");
  display.setCursor(116,30);
  display.print(int(round(m_meterExposure)));

  display.display();
}

void LightMeterState::drawScale()
{
  auto& display = Controls::instance().display;

  const int Y = 50;
  const int PADDING = 8;
  const int PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const int TICK_SIZE = 3;

  display.drawFastHLine(PADDING, Y, 6 * PIXELS_PER_EV, SSD1306_WHITE);

  for (int i = 0; i < 7; ++i) {
    display.drawFastVLine(PADDING + i * PIXELS_PER_EV, Y - TICK_SIZE, TICK_SIZE * 2, SSD1306_WHITE);
    display.setCursor(i * PIXELS_PER_EV, Y + TICK_SIZE + 2);

    auto delta = i - 3;

    if (delta == 0) {
      display.print(" ");
    } else if (delta > 0) {
      display.print("+");
    }
    display.print(i - 3);
  }

  display.display();
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

  display.fillRect(0, Y - TICK_SIZE - 5, display.width(), 5, SSD1306_BLACK);
  display.fillCircle(PADDING + (exposure_diff + 3) * PIXELS_PER_EV, Y - TICK_SIZE - 3, 2, SSD1306_WHITE);

  display.display();
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

  display.clearDisplay();

  uint8_t bounds[4] = {0, 0, display.width(), display.height()};
  m_app.m_listView.setBounds(bounds);

  char title[22];
  ListView::ItemGetter itemGetter;
  itemGetter.boundObj = this;
  uint8_t size;
  uint8_t activeItem;

  if (m_isFrameTarget) {
    strncpy_P(title, (const char*)F("Roll\0"), 22);
    auto& roll = m_app.m_rolls[m_app.m_activeRollId];
    roll.asString(m_app.m_activeRollId, title + 4, 18);
    itemGetter.getter = &ListState::frameItemGetter;
    size = N_FRAMES_PER_ROLL;
    activeItem = m_app.m_activeFrameId;
  } else {
    strncpy_P(title, (const char*)F("Rolls\0"), 22);
    itemGetter.getter = &ListState::rollItemGetter;
    size = N_ROLLS;
    activeItem = m_app.m_activeRollId;
  }

  m_app.m_listView.setTitle(title);
  m_app.m_listView.setItems(itemGetter, size);
  m_app.m_listView.setSelected(activeItem);
}

void ListState::deactivate()
{
  // Controls::instance().display.clearDisplay();
  // Controls::instance().display.display();
}

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

    auto& selectedRoll = m_app.m_rolls[m_app.m_activeRollId];

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
    auto& selectedFrame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, activeId)];

    if (selectedFrame.aperture() != ApertureValue::None) {
      m_app.m_activeFrameId = activeId;
      m_app.m_state->deactivate();
      m_app.m_editModalState.setIsFrameTarget(true);
      m_app.m_state = &m_app.m_editModalState;
      m_app.m_state->activate();
    }
  } else {
    auto& selectedRoll = m_app.m_rolls[activeId];

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
  auto self = static_cast<ListState*>(_self);
  self->m_app.m_rolls[i].asString(i, s, n);
}

void ListState::frameItemGetter(void* _self, uint8_t i, char* s, uint8_t n)
{
  auto self = static_cast<ListState*>(_self);
  self->m_app.m_frames[App::frameIndex(self->m_app.m_activeRollId, i)].asString(i, s, n);
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

  display.clearDisplay();

  auto& activeRoll = m_app.m_rolls[m_app.m_activeRollId];

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

  drawTitle(false);
  drawManufacturer(false);
  drawISO(true);
}

void EditRollState::deactivate()
{
  auto& display = Controls::instance().display;

  display.clearDisplay();
  display.display();
}

void EditRollState::onClickButtonA(int t)
{
  m_app.m_state->deactivate();
  m_app.m_listState.setIsFrameTarget(false);
  m_app.m_state = &m_app.m_listState;
  m_app.m_state->activate();
}

void EditRollState::onClickButtonB(int t)
{
  auto& activeRoll = m_app.m_rolls[m_app.m_activeRollId];
  activeRoll.setManufacturer(m_roll.manufacturer());
  activeRoll.setIso(m_roll.iso());

  Persistency::saveRoll(m_app.m_activeRollId, activeRoll);

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

void EditRollState::drawTitle(bool render)
{
  auto& display = Controls::instance().display;

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.print(F("Roll #"));
  char l[3];
  itoa(m_app.m_activeRollId, l, 10);
  display.print(l);
  display.drawFastHLine(0, TEXT_HEIGHT + MARGIN, display.width(), SSD1306_WHITE);

  if (render) {
    display.display();
  }
}

void EditRollState::drawManufacturer(bool render)
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  display.setCursor(0, y);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print(F("Brand"));

  char label[6];
  rollManufacturerAsString(m_roll.manufacturer(), label, 6);

  y += TEXT_HEIGHT + MARGIN;

  display.setTextSize(2, 2);
  display.setCursor(0, y);
  display.print(F("      "));
  display.setCursor(0, y);
  display.print(label);

  display.setTextSize(1);

  if (render) {
    display.display();
  }
}

void EditRollState::drawISO(bool render)
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + TEXT_HEIGHT * 3 + MARGIN * 6;

  display.setCursor(0, y);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print(F("ISO"));

  char label[5];
  isoValueAsString(m_roll.iso(), label, 5);

  y += TEXT_HEIGHT + MARGIN;

  display.setTextSize(2, 2);
  display.setCursor(0, y);
  display.print(F("     "));
  display.setCursor(0, y);
  display.print(label);

  display.setTextSize(1);

  if (render) {
    display.display();
  }
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

  display.clearDisplay();

  auto& activeRoll = m_app.m_rolls[m_app.m_activeRollId];
  auto& activeFrame = m_app.m_frames[m_app.m_activeFrameId];

  m_roll.setManufacturer(activeRoll.manufacturer());
  m_roll.setIso(activeRoll.iso());

  if (activeFrame.aperture() == ApertureValue::None) {
    m_frame.setAperture(ApertureValue::_8);
  } else {
    m_frame.setAperture(activeFrame.aperture());
  }

  if (activeFrame.shutter() == ShutterSpeed::None) {
    m_frame.setShutter(ShutterSpeed::_125);
  } else {
    m_frame.setShutter(activeFrame.shutter());
  }

  updateSettingsExposure();
  drawTitle(false);
  drawAperture(false);
  drawShutter(true);
  // drawEV(false);
  drawScale(false);
  drawReading(true);
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
  auto& activeFrame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, m_app.m_activeFrameId)];
  activeFrame.setAperture(m_frame.aperture());
  activeFrame.setShutter(m_frame.shutter());

  Persistency::saveFrame(m_app.m_activeRollId, m_app.m_activeFrameId, activeFrame);

  // Upon saving go to the next frame in the roll, if it's empty
  {
    uint8_t nextFrameId = m_app.m_activeFrameId + 1;
    if (nextFrameId < N_FRAMES_PER_ROLL) {
      auto& frame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, nextFrameId)];
      if (frame.aperture() == ApertureValue::None) {
        m_app.m_activeFrameId = nextFrameId;
        // Give a subtle feedback to the user that the frame was saved
        auto& display = Controls::instance().display;
        display.fillRect(0, 0, display.width(), TEXT_HEIGHT, SSD1306_INVERSE);
        display.display();
        drawTitle(true);

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
  meter.setMode(MeterMode::Single);
}

void EditFrameState::onLongpressButtonB(int t)
{
  auto& meter = Controls::instance().meter;
  meter.setMode(MeterMode::Continuous);
}

void EditFrameState::onRightRotaryA(int t)
{
  changeAperture(true);
}

void EditFrameState::onLeftRotaryA(int t)
{
  changeAperture(false);
}

void EditFrameState::onRightRotaryB(int t)
{
  changeShutter(true);
}

void EditFrameState::onLeftRotaryB(int t)
{
  changeShutter(false);
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

void EditFrameState::drawTitle(bool render)
{
  auto& display = Controls::instance().display;

  display.fillRect(0, 0, display.width(), TEXT_HEIGHT, SSD1306_BLACK);

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.print(F("Frame #"));
  char l[12];
  itoa(m_app.m_activeFrameId, l, 10);
  display.print(l);

  display.print(F("  "));
  rollManufacturerAsString(m_roll.manufacturer(), l, 12);
  display.print(l);
  display.print(F(" "));
  isoValueAsString(m_roll.iso(), l, 12);
  display.print(l);
  display.drawFastHLine(0, TEXT_HEIGHT + MARGIN, display.width(), SSD1306_WHITE);

  if (render) {
    display.display();
  }
}

void EditFrameState::drawAperture(bool render)
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  display.setCursor(0, y);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print(F("Aperture"));

  char label[4];
  apertureValueAsString(m_frame.aperture(), label, 4);

  y += TEXT_HEIGHT + MARGIN;

  display.setTextSize(2, 2);
  display.setCursor(0, y);
  display.print(F("   "));
  display.setCursor(0, y);
  display.print(label);

  display.setTextSize(1);

  if (render) {
    display.display();
  }
}

void EditFrameState::drawShutter(bool render)
{
  auto& display = Controls::instance().display;

  uint8_t y = TITLE_HEIGHT + MARGIN * 2;

  int pos[2];
  int size[2];

  display.setCursor(display.width() - 42, y);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.print(F("Shutter"));

  char label[5];
  shutterSpeedAsString(m_frame.shutter(), label, 5);

  y += TEXT_HEIGHT + MARGIN;

  size_t x = display.width() - 12 * strlen(label);

  display.setTextSize(2, 2);
  display.setCursor(display.width() - 12 * 4, y);
  display.print(F("    "));
  display.setCursor(x, y);
  display.print(label);

  display.setTextSize(1);

  if (render) {
    display.display();
  }
}

// void EditFrameState::drawEV(bool render)
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

void EditFrameState::drawScale(bool render)
{
  auto& display = Controls::instance().display;

  const int Y = 50;
  const int PADDING = 8;
  const int PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const int TICK_SIZE = 3;

  display.drawFastHLine(PADDING, Y, 6 * PIXELS_PER_EV, SSD1306_WHITE);

  for (int i = 0; i < 7; ++i) {
    display.drawFastVLine(PADDING + i * PIXELS_PER_EV, Y - TICK_SIZE, TICK_SIZE * 2, SSD1306_WHITE);
    display.setCursor(i * PIXELS_PER_EV, Y + TICK_SIZE + 2);

    auto delta = i - 3;

    if (delta == 0) {
      display.print(" ");
    } else if (delta > 0) {
      display.print("+");
    }
    display.print(i - 3);
  }

  display.display();
}

void EditFrameState::drawReading(bool render)
{
  auto& display = Controls::instance().display;

  const int Y = 50;
  const int PADDING = 8;
  const int PIXELS_PER_EV = (128 - PADDING * 2) / 6;
  const int TICK_SIZE = 3;

  float exposure_diff = m_meterExposure - m_settingsExposure;

  exposure_diff = min(3, exposure_diff);
  exposure_diff = max(-3, exposure_diff);

  display.fillRect(0, Y - TICK_SIZE - 5, display.width(), 5, SSD1306_BLACK);
  display.fillCircle(PADDING + (exposure_diff + 3) * PIXELS_PER_EV, Y - TICK_SIZE - 3, 2, SSD1306_WHITE);

  display.display();
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

  display.fillRect(bounds[0] - 4, bounds[1] - 4, bounds[2] + 8, bounds[3] + 8, SSD1306_BLACK);
  display.drawRect(bounds[0] - 2, bounds[1] - 2, bounds[2] + 4, bounds[3] + 4, SSD1306_WHITE);

  m_app.m_listView.setBounds(bounds);
  char title[22];

  if (m_isFrameTarget) {
    strncpy_P(title, (const char*)F("Frame"), 22);
    auto& frame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, m_app.m_activeFrameId)];
    frame.asString(m_app.m_activeFrameId, title + 5, 17);
  } else {
    strncpy_P(title, (const char*)F("Roll"), 22);
    auto& roll = m_app.m_rolls[m_app.m_activeRollId];
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

    Roll emptyRoll;
    Frame emptyFrame;

    m_app.m_state->deactivate();
    if (m_isFrameTarget) {
      auto& frame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, m_app.m_activeFrameId)];
      frame.setAperture(ApertureValue::None);
      frame.setShutter(ShutterSpeed::None);
      Persistency::saveFrame(m_app.m_activeRollId, m_app.m_activeFrameId, frame);
    } else {
      auto& roll = m_app.m_rolls[m_app.m_activeRollId];
      roll.setManufacturer(RollManufacturer::None);
      roll.setIso(ISOValue::None);
      Persistency::saveRoll(m_app.m_activeRollId, roll);

      for (uint8_t i = 0; i < N_FRAMES_PER_ROLL; ++i) {
        auto& frame = m_app.m_frames[App::frameIndex(m_app.m_activeRollId, i)];
        frame.setAperture(ApertureValue::None);
        frame.setShutter(ShutterSpeed::None);
        Persistency::saveFrame(m_app.m_activeRollId, i, frame);
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

  display.clearDisplay();

  display.setCursor(21, 8);
  display.print(F("Copyright 2023"));
  display.setCursor(12, 20);
  display.print(F("Alessandro Genova"));

  display.display();
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

#endif
