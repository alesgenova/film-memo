#include "CameraSettingsView.h"

#include <WString.h>

#include "Controls.h"


const uint8_t COLUMN_WIDTH = 42;
const uint8_t COLUMN_START[3] = {0, COLUMN_WIDTH + 1, 2 * COLUMN_WIDTH + 2};

CameraSettingsView::CameraSettingsView()
{}

CameraSettingsView::~CameraSettingsView()
{}

uint8_t CameraSettingsView::position() const
{
  return m_y;
}

void CameraSettingsView::setPosition(uint8_t y)
{
  m_y = y;

  drawSeparators();
}

void CameraSettingsView::drawSeparators()
{
  auto& display = Controls::instance().display;

  const uint8_t y0 = m_y;
  const uint8_t y1 = y0 + 3 * TEXT_HEIGHT + MARGIN;

  display.drawVLine(COLUMN_START[1] - 1, y0, y1);
  display.drawVLine(COLUMN_START[2] - 1, y0, y1);
}

void CameraSettingsView::setLabel(uint8_t col, const char* label, bool invert = false)
{
  if (col > 2) {
    return;
  }

  auto& display = Controls::instance().display;

  const uint8_t y = m_y;

  display.fillRectangle(COLUMN_START[col], y, COLUMN_START[col] + COLUMN_WIDTH - 1, y + TEXT_HEIGHT, &Display::blackPainter);

  const uint8_t n = strlen(label);

  const uint8_t textWidth = n * 6;

  const uint8_t x = COLUMN_START[col] + COLUMN_WIDTH / 2 - textWidth / 2;

  Display::Painter color = &Display::whitePainter;
  Display::Painter bg = &Display::blackPainter;

  if (invert) {
    color = &Display::blackPainter;
    bg = &Display::whitePainter;
  }

  display.print(x, y, label, color, bg, 1, 1);
}

void CameraSettingsView::setValue(uint8_t col, const char* value)
{
  if (col > 2) {
    return;
  }

  auto& display = Controls::instance().display;

  const uint8_t y = m_y + TEXT_HEIGHT + MARGIN;

  display.fillRectangle(COLUMN_START[col], y, COLUMN_START[col] + COLUMN_WIDTH - 1, y + 2 * TEXT_HEIGHT, &Display::blackPainter);

  const uint8_t n = strlen(value);

  const uint8_t xScale = n > 3 ? 1 : 2;

  const uint8_t textWidth = n * 6 * xScale;

  const uint8_t x = COLUMN_START[col] + COLUMN_WIDTH / 2 - textWidth / 2;

  display.print(x, y, value, &Display::whitePainter, &Display::blackPainter, xScale, 2);
}
