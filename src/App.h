#ifndef App_h
#define App_h

#include <stdint.h>

#include "constants.h"

#include "AppState.h"
#include "Roll.h"
#include "Frame.h"

class App {
public:
  static App& instance();

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  void initialize();
  void process(int t) {}

protected:
  App();
  ~App();

#if __DEBUG
  DebugAppState m_debugState;
#elif __METER
  LightMeterState m_meterState;
#elif __MEMO
  ListState m_listState;
  EditRollState m_editRollState;
  EditFrameState m_editFrameState;
  EditModalState m_editModalState;
  AboutState m_aboutState;

  uint8_t m_activeRollId = 0;
  uint8_t m_activeFrameId = 0;

  Roll m_rolls[N_ROLLS];
  Frame m_frames[N_FRAMES];

  ListView m_listView;
#endif

  AppState* m_state;

  static App* unique_instance;

  static void onClickButtonA(void* self, const int& t);
  static void onLongpressButtonA(void* self, const int& t);

  static void onClickButtonB(void* self, const int& t);
  static void onLongpressButtonB(void* self, const int& t);

  static void onRightRotaryA(void* self, const int& t);
  static void onLeftRotaryA(void* self, const int& t);

  static void onRightRotaryB(void* self, const int& t);
  static void onLeftRotaryB(void* self, const int& t);

  static void onClickFlash(void* self, const int& t);
  static void onLongpressFlash(void* self, const int& t);

  static void onMeterReading(void* self, const int& value);

  static uint8_t frameIndex(uint8_t rollId, uint8_t frameId);

  friend class DebugAppState;
  friend class LightMeterState;
  friend class EditRollState;
  friend class ListState;
  friend class EditFrameState;
  friend class EditModalState;
  friend class AboutState;
};

#endif