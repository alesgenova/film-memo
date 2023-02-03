#ifndef AppState_h
#define AppState_h

#include <stdint.h>

#include "constants.h"

#if __MEMO
#include "Exposure.h"
#include "Roll.h"
#include "Frame.h"
#include "Action.h"
#endif

class App;

class AppState {
public:
  AppState(App& app);
  virtual ~AppState() = 0;

  AppState(const AppState&) = delete;
  AppState& operator=(const AppState&) = delete;

  virtual void activate() {}
  virtual void deactivate() {}

  virtual void onClickButtonA(uint32_t t) {}
  virtual void onLongpressButtonA(uint32_t t) {}

  virtual void onClickButtonB(uint32_t t) {}
  virtual void onLongpressButtonB(uint32_t t) {}

  virtual void onRightRotaryA(uint32_t t) {}
  virtual void onLeftRotaryA(uint32_t t) {}

  virtual void onRightRotaryB(uint32_t t) {}
  virtual void onLeftRotaryB(uint32_t t) {}

  virtual void onDownFlash(uint32_t t) {}
  virtual void onUpFlash(uint32_t t) {}

  virtual void onMeterReading(int value) {}

protected:
  App& m_app;
};

#if __DEBUG

// DebugAppState

class DebugAppState : public AppState {
public:
  DebugAppState(App& app);
  ~DebugAppState();

  DebugAppState(const DebugAppState&) = delete;
  DebugAppState& operator=(const DebugAppState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;
  virtual void onLongpressButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;
  virtual void onLongpressButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

  virtual void onDownFlash(uint32_t t) override;
  virtual void onUpFlash(uint32_t t) override;

  virtual void onMeterReading(int value) override;

protected:
  void drawButtonA();
  void drawButtonB();
  void drawRotaryA();
  void drawRotaryB();
  void drawMeter();
  bool m_buttonA = false;
  bool m_buttonB = false;
  uint8_t m_rotaryA = 7;
  uint8_t m_rotaryB = 7;
  int m_meterValue = 255;
};

#elif __MEMO

// ListState

class ListState : public AppState {
public:
  ListState(App& app);
  ~ListState();

  ListState(const ListState&) = delete;
  ListState& operator=(const ListState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  void setIsFrameTarget(bool isFrameTarget);

  virtual void onClickButtonA(uint32_t t) override;
  virtual void onLongpressButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;
  virtual void onLongpressButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

protected:
  static void rollItemGetter(void* self, uint8_t i, char* s, uint8_t n);
  static void frameItemGetter(void* self, uint8_t i, char* s, uint8_t n);

  bool m_isFrameTarget = false;
};

// EditRollState

class EditRollState : public AppState {
public:
  EditRollState(App& app);
  ~EditRollState();

  EditRollState(const EditRollState&) = delete;
  EditRollState& operator=(const EditRollState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

protected:
  Roll m_roll;

  void changeManufacturer(bool increase);
  void changeISO(bool increase);

  void drawTitle();
  void drawManufacturer();
  void drawISO();
};

// EditFrameState

class EditFrameState : public AppState {
public:
  EditFrameState(App& app);
  ~EditFrameState();

  EditFrameState(const EditFrameState&) = delete;
  EditFrameState& operator=(const EditFrameState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;
  virtual void onLongpressButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;
  virtual void onLongpressButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

  virtual void onDownFlash(uint32_t t) override;
  virtual void onUpFlash(uint32_t t) override;

  virtual void onMeterReading(int value) override;

protected:
  void changeAperture(bool increase);
  void changeShutter(bool increase);
  void changeFocal(bool increase);

  void drawTitle();
  void drawAperture();
  void drawShutter();
  void drawFocal();

  void updateSettingsExposure();

  Roll m_roll;
  Frame m_frame;
  float m_meterExposure = 10;
  float m_settingsExposure = 10;
  bool m_editFocal = false;
  uint32_t m_flashDownTime = 0;
};

// EditModalState

class EditModalState : public AppState {
public:
  EditModalState(App& app);
  ~EditModalState();

  EditModalState(const EditModalState&) = delete;
  EditModalState& operator=(const EditModalState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  void setIsFrameTarget(bool isFrameTarget);

  virtual void onClickButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

protected:
  static void itemGetter(void* self, uint8_t i, char* s, uint8_t n);

  bool m_isFrameTarget = false;

  Action m_actions[3];
};

// SettingsState

class SettingsState : public AppState {
public:
  SettingsState(App& app);
  ~SettingsState();

  SettingsState(const SettingsState&) = delete;
  SettingsState& operator=(const SettingsState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

protected:
  void drawTitle();
  void drawAutoShutter();

  void changeAutoShutter();

  static void itemGetter(void* self, uint8_t i, char* s, uint8_t n);

  Action m_actions[3];
};

// AboutState

class AboutState : public AppState {
public:
  AboutState(App& app);
  ~AboutState();

  AboutState(const AboutState&) = delete;
  AboutState& operator=(const AboutState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;
};

// LightMeterState

class LightMeterState : public AppState {
public:
  LightMeterState(App& app);
  ~LightMeterState();

  LightMeterState(const LightMeterState&) = delete;
  LightMeterState& operator=(const LightMeterState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;
  virtual void onLongpressButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;
  virtual void onLongpressButtonB(uint32_t t) override;

  virtual void onRightRotaryA(uint32_t t) override;
  virtual void onLeftRotaryA(uint32_t t) override;

  virtual void onRightRotaryB(uint32_t t) override;
  virtual void onLeftRotaryB(uint32_t t) override;

  virtual void onMeterReading(int value) override;

protected:
  void drawTitle();
  void drawAperture();
  void drawShutter();
  void drawISO();

  void changeAperture(bool increase);
  void changeShutter(bool increase);
  void changeISO(bool increase);
  void updateSettingsExposure();

  ApertureValue m_aperture = ApertureValue::_8;
  ShutterSpeed m_shutter = ShutterSpeed::_125;
  ISOValue m_iso = ISOValue::_400;
  float m_meterExposure = 10;
  float m_settingsExposure = 10;
  bool m_editISO = false;
};

// CalibrateState

class CalibrateState : public AppState {
public:
  CalibrateState(App& app);
  ~CalibrateState();

  CalibrateState(const CalibrateState&) = delete;
  CalibrateState& operator=(const CalibrateState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(uint32_t t) override;

  virtual void onClickButtonB(uint32_t t) override;

  virtual void onDownFlash(uint32_t t) override;

  virtual void onUpFlash(uint32_t t) override;

protected:
  void drawTitle();
  void drawShutter();
  void drawProgress();

  void nextShutter();

  static uint8_t shutterToIndex(ShutterSpeed shutter);

  ShutterSpeed m_shutter = ShutterSpeed::_2;

  uint8_t m_currRepetition = 0;
  uint32_t m_cumulatedTime = 0;
  uint32_t m_downTime = 0;
};

#endif

#endif
