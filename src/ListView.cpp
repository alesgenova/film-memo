#include "ListView.h"

#include "Controls.h"

ListView::ListView()
{}

ListView::~ListView()
{}

void ListView::setTitle(const char* title)
{
  auto& display = Controls::instance().display;

  display.fillRect(m_bounds[0], m_bounds[1], m_bounds[2], TEXT_HEIGHT, SSD1306_BLACK);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setCursor(m_bounds[0], m_bounds[1]);
  display.print(title);
  display.drawFastHLine(m_bounds[0], m_bounds[1] + TEXT_HEIGHT + MARGIN, m_bounds[2], SSD1306_WHITE);

  display.display();
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
    m_selected = targetId;
    m_offset = max(0, m_selected - 2);
    drawItems();
  }
}

void ListView::selectNext(uint8_t step)
{
  if (step > 1) {
    setSelected(m_selected + step);
    return;
  }

  if (m_selected < size() - 1) {
    ++m_selected;
    m_offset = max(m_offset, m_selected - itemsPerPage() + 1);
    drawItems();
  }
}

void ListView::selectPrev(uint8_t step)
{
  if (step > 1) {
    setSelected(m_selected - step);
    return;
  }

  if (m_selected > 0) {
    --m_selected;
    m_offset = min(m_offset, m_selected);
    drawItems();
  }
}

uint8_t ListView::itemsPerPage() const
{
  return (m_bounds[3] - TITLE_HEIGHT) / (TEXT_HEIGHT + MARGIN);
}

void ListView::drawItems()
{
  auto& display = Controls::instance().display;

  display.fillRect(m_bounds[0], m_bounds[1] + TITLE_HEIGHT, m_bounds[2], m_bounds[3] - TITLE_HEIGHT, SSD1306_BLACK);

  if (m_itemGetter.getter) {
    char label[20];

    for (uint8_t i = 0; i < itemsPerPage(); ++i) {
      uint8_t itemIdx = i + m_offset;
      
      if (itemIdx >= size()) {
        break;
      }

      uint8_t color = itemIdx == m_selected ? SSD1306_BLACK : SSD1306_WHITE;
      uint8_t bg_color = itemIdx == m_selected ? SSD1306_WHITE : SSD1306_BLACK;

      display.fillRect(m_bounds[0], m_bounds[1] + TITLE_HEIGHT + MARGIN + (TEXT_HEIGHT + MARGIN) * i, m_bounds[2], TEXT_HEIGHT, bg_color);
      display.setTextColor(color, bg_color);
      display.setCursor(m_bounds[0], m_bounds[1] + TITLE_HEIGHT + MARGIN + (TEXT_HEIGHT + MARGIN) * i);

      m_itemGetter.getter(m_itemGetter.boundObj, itemIdx, label, 20);

      display.print(label);
    }
  }

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

  display.display();
}
