#ifndef App_h
#define App_h

#include <stdint.h>

#include "constants.h"

#include "AppState.h"
#include "Roll.h"
#include "Frame.h"

#if __MEMO
#include "ListView.h"
#include "MeterView.h"
#include "CameraSettingsView.h"
#endif

class App {
public:
  static App& instance();

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  void initialize();
  void process(const uint32_t& t) {}

protected:
  App();
  ~App();

#if __DEBUG
  DebugAppState m_debugState;
#elif __MEMO
  ListState m_listState;
  EditRollState m_editRollState;
  EditFrameState m_editFrameState;
  EditModalState m_editModalState;
  AboutState m_aboutState;
  LightMeterState m_meterState;
  SettingsState m_settingsState;
  CalibrateState m_calibrateState;

  uint8_t m_activeRollId = 0;
  uint8_t m_activeFrameId = 0;

  bool m_autoShutter;

  float m_meterCalibration;

  ListView m_listView;
  MeterView m_meterView;
  CameraSettingsView m_cameraSettingsView;
#endif

  AppState* m_state;

  static App* unique_instance;

  static void onClickButtonA(void* self, const uint32_t& t);
  static void onLongpressButtonA(void* self, const uint32_t& t);

  static void onClickButtonB(void* self, const uint32_t& t);
  static void onLongpressButtonB(void* self, const uint32_t& t);

  static void onRightRotaryA(void* self, const uint32_t& t);
  static void onLeftRotaryA(void* self, const uint32_t& t);

  static void onRightRotaryB(void* self, const uint32_t& t);
  static void onLeftRotaryB(void* self, const uint32_t& t);

  static void onDownFlash(void* self, const uint32_t& t);
  static void onUpFlash(void* self, const uint32_t& t);

  static void onMeterReading(void* self, const int& value);

  friend class DebugAppState;
  friend class LightMeterState;
  friend class EditRollState;
  friend class ListState;
  friend class EditFrameState;
  friend class EditModalState;
  friend class AboutState;
  friend class SettingsState;
  friend class CalibrateState;
};

#endif