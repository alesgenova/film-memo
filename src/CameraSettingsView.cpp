#include "CameraSettingsView.h"

#include <WString.h>

#include "Controls.h"


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

uint8_t CameraSettingsView::columns() const
{
  return m_cols;
}

void CameraSettingsView::setColumns(uint8_t cols)
{
  m_cols = cols;
}

uint8_t CameraSettingsView::columnWidth(uint8_t n)
{
  return (128 - n - 1) / n;
}

uint8_t CameraSettingsView::columnStart(uint8_t col, uint8_t n)
{
  const uint8_t width = columnWidth(n);

  return (width + 1) * col;
}

void CameraSettingsView::drawSeparators()
{
  auto& display = Controls::instance().display;

  const uint8_t y0 = m_y;
  const uint8_t y1 = y0 + 3 * TEXT_HEIGHT + MARGIN;

  for (uint8_t i = 0; i < m_cols - 1; ++i) {
    display.drawVLine(columnStart(i + 1, m_cols) - 1, y0, y1);
  }
}

void CameraSettingsView::setLabel(uint8_t col, const char* label, bool invert = false)
{
  if (col >= m_cols) {
    return;
  }

  auto& display = Controls::instance().display;

  const uint8_t y = m_y;

  const uint8_t colWidth = columnWidth(m_cols);
  const uint8_t colStart = columnStart(col, m_cols);

  display.fillRectangle(colStart, y, colStart + colWidth - 1, y + TEXT_HEIGHT, &Display::blackPainter);

  const uint8_t n = strlen(label);

  const uint8_t textWidth = n * 6;

  const uint8_t x = colStart + colWidth / 2 - textWidth / 2;

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
  if (col >= m_cols) {
    return;
  }

  auto& display = Controls::instance().display;

  const uint8_t y = m_y + TEXT_HEIGHT + MARGIN;

  const uint8_t colWidth = columnWidth(m_cols);
  const uint8_t colStart = columnStart(col, m_cols);

  display.fillRectangle(colStart, y, colStart + colWidth - 1, y + 2 * TEXT_HEIGHT, &Display::blackPainter);

  const uint8_t n = strlen(value);

  const uint8_t xScale = n * 12 > colWidth ? 1 : 2;

  const uint8_t textWidth = n * 6 * xScale;

  const uint8_t x = colStart + colWidth / 2 - textWidth / 2;

  display.print(x, y, value, &Display::whitePainter, &Display::blackPainter, xScale, 2);
}
