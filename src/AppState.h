#ifndef AppState_h
#define AppState_h

#include <stdint.h>

#include "constants.h"

#if __METER
#include "Exposure.h"
#elif __MEMO
#include "Exposure.h"
#include "ListView.h"
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

  virtual void onClickButtonA(int t) {}
  virtual void onLongpressButtonA(int t) {}

  virtual void onClickButtonB(int t) {}
  virtual void onLongpressButtonB(int t) {}

  virtual void onRightRotaryA(int t) {}
  virtual void onLeftRotaryA(int t) {}

  virtual void onRightRotaryB(int t) {}
  virtual void onLeftRotaryB(int t) {}

  virtual void onClickFlash(int t) {}
  virtual void onLongpressFlash(int t) {}

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

  virtual void onClickButtonA(int t) override;
  virtual void onLongpressButtonA(int t) override;

  virtual void onClickButtonB(int t) override;
  virtual void onLongpressButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

  virtual void onRightRotaryB(int t) override;
  virtual void onLeftRotaryB(int t) override;

  virtual void onClickFlash(int t) override;
  virtual void onLongpressFlash(int t) override;

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

#elif __METER

// LightMeterState

class LightMeterState : public AppState {
public:
  LightMeterState(App& app);
  ~LightMeterState();

  LightMeterState(const LightMeterState&) = delete;
  LightMeterState& operator=(const LightMeterState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonA(int t) override;
  virtual void onLongpressButtonA(int t) override;

  virtual void onClickButtonB(int t) override;
  virtual void onLongpressButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

  virtual void onRightRotaryB(int t) override;
  virtual void onLeftRotaryB(int t) override;

  virtual void onMeterReading(int value) override;

protected:
  void drawAperture();
  void drawShutter();
  void drawISO();
  void drawEV();
  void drawScale();
  void drawReading();

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

  virtual void onClickButtonA(int t) override;
  virtual void onLongpressButtonA(int t) override;

  virtual void onClickButtonB(int t) override;
  virtual void onLongpressButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

  virtual void onRightRotaryB(int t) override;
  virtual void onLeftRotaryB(int t) override;

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

  virtual void onClickButtonA(int t) override;

  virtual void onClickButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

  virtual void onRightRotaryB(int t) override;
  virtual void onLeftRotaryB(int t) override;

protected:
  Roll m_roll;

  void changeManufacturer(bool increase);
  void changeISO(bool increase);

  void drawTitle(bool render = true);
  void drawManufacturer(bool render = true);
  void drawISO(bool render = true);
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

  virtual void onClickButtonA(int t) override;
  virtual void onLongpressButtonA(int t) override;

  virtual void onClickButtonB(int t) override;
  virtual void onLongpressButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

  virtual void onRightRotaryB(int t) override;
  virtual void onLeftRotaryB(int t) override;

  virtual void onClickFlash(int t) override;
  virtual void onLongpressFlash(int t) override;

  virtual void onMeterReading(int value) override;

protected:
  void changeAperture(bool increase);
  void changeShutter(bool increase);

  void drawTitle(bool render = true);
  void drawAperture(bool render = true);
  void drawShutter(bool render = true);
  // void drawEV(bool render = true);
  void drawScale(bool render = true);
  void drawReading(bool render = true);

  void updateSettingsExposure();

  Roll m_roll;
  Frame m_frame;
  float m_meterExposure = 10;
  float m_settingsExposure = 10;
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

  virtual void onClickButtonA(int t) override;

  virtual void onClickButtonB(int t) override;

  virtual void onRightRotaryA(int t) override;
  virtual void onLeftRotaryA(int t) override;

protected:
  static void itemGetter(void* self, uint8_t i, char* s, uint8_t n);

  bool m_isFrameTarget = false;

  Action m_actions[3];
};

class AboutState : public AppState {
public:
  AboutState(App& app);
  ~AboutState();

  AboutState(const AboutState&) = delete;
  AboutState& operator=(const AboutState&) = delete;

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void onClickButtonB(int t) override;
};

#endif

#endif
