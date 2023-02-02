#include "MeterView.h"

#include "Controls.h"

#include <Arduino.h>

MeterView::MeterView()
{}

MeterView::~MeterView()
{}

void MeterView::bounds(uint8_t b[4]) const
{
  for (uint8_t i = 0; i < 4; ++i) {
    b[i] = m_bounds[i];
  }
}

void MeterView::setBounds(uint8_t b[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    m_bounds[i] = b[i];
  }

  drawScale();
  drawReading();
}

void MeterView::setReading(float reading)
{
  m_reading = reading;
  drawReading();
}

void MeterView::drawScale()
{
  auto& display = Controls::instance().display;

  const uint8_t TICK_SIZE = 3;
  const uint8_t X = m_bounds[0];
  const uint8_t Y = m_bounds[1] + TICK_SIZE + 5;
  const uint8_t PADDING = m_bounds[2] / 16;
  const uint8_t PIXELS_PER_EV = (m_bounds[2] - PADDING * 2) / 6;

  display.drawHLine(X + PADDING, Y, X + PADDING + 6 * PIXELS_PER_EV);

  for (uint8_t i = 0; i < 7; ++i) {
    display.drawVLine(X + PADDING + i * PIXELS_PER_EV, Y - TICK_SIZE, Y + TICK_SIZE);

    uint8_t x = X + i * PIXELS_PER_EV;
    uint8_t y = Y + TICK_SIZE + 2;

    auto delta = i - 3;

    if (delta == 0) {
      x = display.printEmpty(x, y, 1);
    } else if (delta > 0) {
      x = display.print(x, y, "+");
    }

    display.print(x, y, i - 3);
  }
}

void MeterView::drawReading()
{
  auto& display = Controls::instance().display;

  const uint8_t X = m_bounds[0];
  const uint8_t Y = m_bounds[1];
  const uint8_t PADDING = m_bounds[2] / 16;
  const uint8_t PIXELS_PER_EV = (m_bounds[2] - PADDING * 2) / 6;
  const uint8_t TICK_SIZE = 3;

  float reading = m_reading;

  reading = min(3, reading);
  reading = max(-3, reading);

  const uint8_t centerX = PADDING + (reading + 3) * PIXELS_PER_EV;
  const uint8_t centerY = Y + 2;

  display.fillRectangle(X, Y, X + m_bounds[2], Y + 5, &Display::blackPainter);
  display.fillRectangle(centerX - 2, centerY - 2, centerX + 2, centerY + 2);
}
