#include "ListView.h"

#include <Arduino.h>

#include "Controls.h"

ListView::ListView()
{}

ListView::~ListView()
{}

void ListView::setTitle(const char* title)
{
  auto& display = Controls::instance().display;

  display.fillRectangle(m_bounds[0], m_bounds[1], m_bounds[0] + m_bounds[2], m_bounds[1] + TEXT_HEIGHT, &Display::blackPainter);
  display.print(m_bounds[0], m_bounds[1], title);
  display.drawHLine(m_bounds[0], m_bounds[1] + TEXT_HEIGHT, m_bounds[0] + m_bounds[2]);
}

void ListView::bounds(uint8_t b[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    b[i] = m_bounds[i];
  }
}

void ListView::setBounds(uint8_t b[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    m_bounds[i] = b[i];
  }
}

void ListView::setItems(ItemGetter itemGetter, uint8_t n)
{
  m_itemGetter = itemGetter;
  m_size = n;
  m_offset = 0;
  m_selected = 0;

  drawItems();
}

uint8_t ListView::size() const
{
  return m_size;
}

uint8_t ListView::selected() const
{
  return m_selected;
}

void ListView::setSelected(int8_t targetId)
{
  targetId = min(size() - 1, targetId);
  targetId = max(0, targetId);

  if (targetId != m_selected) {
    if (size() <= itemsPerPage()) {
      drawSelection(); // deselect previous
      m_selected = targetId;
      drawSelection(); // select current
    } else {
      m_selected = targetId;
      m_offset = max(0, m_selected - 2);
      drawItems();
    }
  }
}

void ListView::selectNext(uint8_t step)
{
  if (step > 1) {
    setSelected(m_selected + step);
    return;
  }

  if (m_selected < size() - 1) {
    uint8_t selected = m_selected + 1;
    uint8_t offset = max(m_offset, selected - itemsPerPage() + 1);
    if (offset != m_offset) {
      m_offset = offset;
      m_selected = selected;
      drawItems();
    } else {
      drawSelection(); // deselect previous
      m_selected = selected;
      drawSelection(); // select current
    }
  }
}

void ListView::selectPrev(uint8_t step)
{
  if (step > 1) {
    setSelected(m_selected - step);
    return;
  }

  if (m_selected > 0) {
    uint8_t selected = m_selected - 1;
    uint8_t offset = min(m_offset, selected);
    if (offset != m_offset) {
      m_offset = offset;
      m_selected = selected;
      drawItems();
    } else {
      drawSelection(); // deselect previous
      m_selected = selected;
      drawSelection(); // select current
    }
  }
}

uint8_t ListView::itemsPerPage() const
{
  return (m_bounds[3] - TITLE_HEIGHT) / (TEXT_HEIGHT + MARGIN);
}

void ListView::drawItems()
{
  auto& display = Controls::instance().display;

  display.fillRectangle(m_bounds[0], m_bounds[1] + TITLE_HEIGHT, m_bounds[0] + m_bounds[2], m_bounds[1] + m_bounds[3], &Display::blackPainter);

  if (m_itemGetter.getter) {
    char label[20];

    for (uint8_t i = 0; i < itemsPerPage(); ++i) {
      uint8_t itemIdx = i + m_offset;
      
      if (itemIdx >= size()) {
        break;
      }

      uint8_t y = m_bounds[1] + TITLE_HEIGHT + MARGIN + (TEXT_HEIGHT + MARGIN) * i;

      m_itemGetter.getter(m_itemGetter.boundObj, itemIdx, label, 20);

      display.print(m_bounds[0] + 6, y, label);
    }
  }

  drawSelection();
}

void ListView::drawSelection()
{
  auto& display = Controls::instance().display;

  uint8_t y = m_bounds[1] + TITLE_HEIGHT + (TEXT_HEIGHT + MARGIN) * (m_selected - m_offset);

  display.fillRectangle(m_bounds[0], y, m_bounds[0] + m_bounds[2], y + TEXT_HEIGHT, &Display::inversePainter);
}
