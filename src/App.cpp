#include "App.h"

#include "Controls.h"
#include "Persistency.h"

App::App()
#if __DEBUG
  : m_debugState(*this)
  , m_state(&m_debugState)
#elif __MEMO
  : m_listState(*this)
  , m_editRollState(*this)
  , m_editFrameState(*this)
  , m_editModalState(*this)
  , m_aboutState(*this)
  , m_meterState(*this)
  , m_settingsState(*this)
  , m_calibrateState(*this)
  , m_state(&m_listState)
#endif
{}

App::~App()
{}

App* App::unique_instance = nullptr;

App& App::instance() {
  if (!App::unique_instance) {
    App::unique_instance = new App();
  }

  return *App::unique_instance;
}

void App::initialize()
{
  auto& controls = Controls::instance();

  controls.buttonA.clickObservable().addListener(this, &App::onClickButtonA);
  controls.buttonA.longpressObservable().addListener(this, &App::onLongpressButtonA);

  controls.buttonB.clickObservable().addListener(this, &App::onClickButtonB);
  controls.buttonB.longpressObservable().addListener(this, &App::onLongpressButtonB);

  controls.rotaryA.rightObservable().addListener(this, &App::onRightRotaryA);
  controls.rotaryA.leftObservable().addListener(this, &App::onLeftRotaryA);

  controls.rotaryB.rightObservable().addListener(this, &App::onRightRotaryB);
  controls.rotaryB.leftObservable().addListener(this, &App::onLeftRotaryB);

  controls.meter.readingObservable().addListener(this, &App::onMeterReading);

  controls.flash.downObservable().addListener(this, &App::onDownFlash);
  controls.flash.upObservable().addListener(this, &App::onUpFlash);

#if __MEMO
  m_autoShutter = Persistency::readAutoShutter();
  Persistency::readMeterCalibration(m_meterCalibration);

  m_listState.setIsFrameTarget(false);
  m_editModalState.setIsFrameTarget(false);
#endif

  m_state->activate();
}

void App::onClickButtonA(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onClickButtonA(t);
}

void App::onLongpressButtonA(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onLongpressButtonA(t);
}

void App::onClickButtonB(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onClickButtonB(t);
}

void App::onLongpressButtonB(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onLongpressButtonB(t);
}

void App::onRightRotaryA(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onRightRotaryA(t);
}

void App::onLeftRotaryA(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onLeftRotaryA(t);
}

void App::onRightRotaryB(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onRightRotaryB(t);
}

void App::onLeftRotaryB(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onLeftRotaryB(t);
}

void App::onMeterReading(void* _self, const int& value)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onMeterReading(value);
}

void App::onDownFlash(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onDownFlash(t);
}

void App::onUpFlash(void* _self, const uint32_t& t)
{
  auto self = static_cast<App*>(_self);

  self->m_state->onUpFlash(t);
}
