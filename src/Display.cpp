#include "Display.h"

#include <Wire.h>
#include <WString.h>
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "font5x7.h"

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22  
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0  
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6      
#define SSD1306_INVERTDISPLAY 0xA7      
#define SSD1306_SETMULTIPLEX 0xA8       
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF 
#define SSD1306_COMSCANINC 0xC0  
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3   
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5 
#define SSD1306_SETPRECHARGE 0xD9       
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB      

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40 


uint8_t Display::memDereferencer(uint8_t* d) {
  return *d;
}

uint8_t Display::pgmDereferencer(uint8_t* d) {
  return pgm_read_byte(d);
}

uint8_t Display::whitePainter(uint8_t current, uint8_t incoming) {
  return current | incoming;
}

uint8_t Display::blackPainter(uint8_t current, uint8_t incoming) {
  return current & (~incoming);
}

uint8_t Display::inversePainter(uint8_t current, uint8_t incoming) {
  return current ^ incoming;
}

uint8_t Display::noopPainter(uint8_t current, uint8_t incoming) {
  return current;
}

uint8_t Display::overwritePainter(uint8_t current, uint8_t incoming) {
  return incoming;
}

Display::Painter Display::getPainter(uint8_t color) {
  switch(color) {
    case COLOR_WHITE: {
      return &whitePainter;
    } case COLOR_BLACK: {
      return &blackPainter;
    } case COLOR_INVERSE: {
      return &inversePainter;
    } default: {
      return &whitePainter;
    }
  }
}

Display::Writer::Writer()
{}

void Display::Writer::sendCommand(uint8_t d) {
  this->sendOneInternal(d, 0x00);
}

void Display::Writer::sendData(uint8_t d) {
  this->sendOneInternal(d, 0xC0);
}

void Display::Writer::sendCommandArray(uint8_t* arr, int size, Dereferencer deref = &Display::memDereferencer) {
  this->sendManyInternal(arr, size, 0x00, deref);
}

void Display::Writer::sendDataArray(uint8_t* arr, int size, Dereferencer deref = &Display::memDereferencer) {
  this->sendManyInternal(arr, size, 0x40, deref);
}

void Display::Writer::sendOneInternal(uint8_t d, uint8_t rgstr) {
  Wire.beginTransmission(m_addr);
  Wire.write(rgstr);
  Wire.write(d);
  Wire.endTransmission();
}

void Display::Writer::sendManyInternal(uint8_t* arr, int size, uint8_t rgstr, Dereferencer deref = &Display::memDereferencer) {
  Wire.beginTransmission(m_addr);
  Wire.write(rgstr);
  uint8_t i = 1;

  while(size--) {
    if (i >= BUFFER_LENGTH) {
      Wire.endTransmission();
      Wire.beginTransmission(m_addr);
      Wire.write(rgstr);
      i = 1;
    }

    Wire.write(deref(arr++));
    ++i;
  }

  Wire.endTransmission();
}

Display::Display()
  : m_touchedBounds({0})
  , m_buffer({0})
{}

void Display::begin(uint8_t addr)
{
  static const uint8_t CMD_LIST[] PROGMEM =
  {
    SSD1306_DISPLAYOFF, // display off
    SSD1306_MEMORYMODE, 0x00, // Horizontal mode
    SSD1306_COMSCANDEC,             // Scan from 127 to 0 (Reverse scan)
    SSD1306_SETSTARTLINE | 0x00,    // First line to start scanning from
    SSD1306_SETCONTRAST, 0x7F,      // contast value to 0x7F according to datasheet
    SSD1306_SEGREMAP | 0x01,        // Use reverse mapping. 0x00 - is normal mapping
    SSD1306_NORMALDISPLAY,
    SSD1306_SETMULTIPLEX, 63,       // Reset to default MUX. See datasheet
    SSD1306_SETDISPLAYOFFSET, 0x00, // no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,// set to default ratio/osc frequency
    SSD1306_SETPRECHARGE, 0x22,     // switch precharge to 0x22 // 0xF1
    SSD1306_SETCOMPINS, 0x12,       // set divide ratio
    SSD1306_SETVCOMDETECT, 0x20,    // vcom deselect to 0x20 // 0x40
    SSD1306_CHARGEPUMP, 0x14,       // Enable charge pump for 3.3V supply
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_DISPLAYON,
  };

  m_writer.m_addr = addr;

  Wire.setClock(400000UL);
  m_writer.sendCommandArray(CMD_LIST, sizeof(CMD_LIST), &Display::pgmDereferencer);
  Wire.setClock(100000UL);
}

void Display::setContrast(uint8_t value)
{
  Wire.setClock(400000UL);
  m_writer.sendCommand(SSD1306_SETCONTRAST);
  m_writer.sendCommand(value);
  Wire.setClock(100000UL);
}

void Display::clear()
{
  for (int i = 0; i < sizeof(m_buffer); ++i) {
    m_buffer[i] = 0;
  }

  m_touched = true;

  for (uint8_t page = 0; page < pages(); ++page) {
    m_touchedBounds[page * 2] = 0;
    m_touchedBounds[page * 2 + 1] = width() - 1;
  }
}

void Display::writeBufferUnsafe(uint8_t page, uint8_t column, uint8_t data, Painter painter)
{
  uint16_t index;
  Display::getBufferIndex(page, column, index);

  // marked unsafe, remove this check
  // if (index >= bufferSize()) {
  //   return;
  // }

  m_buffer[index] = painter(m_buffer[index], data);

  m_touched = true;

  m_touchedBounds[page * 2] = min(column, m_touchedBounds[page * 2]);
  m_touchedBounds[page * 2 + 1] = max(column, m_touchedBounds[page * 2 + 1]);
}

void Display::writeBufferSafe(uint8_t page, uint8_t column, uint8_t data, Painter painter)
{
  if (page >= pages() || column >= width()) {
    return;
  }

  this->writeBufferUnsafe(page, column, data, painter);
}

void Display::render()
{
  if (!m_touched) {
    return;
  }

  Wire.setClock(400000UL);

  for (uint8_t page = 0; page < 8; ++page) {
    uint8_t startCol = m_touchedBounds[page * 2];
    uint8_t endCol = m_touchedBounds[page * 2 + 1];

    if (startCol > endCol) {
      continue;
    }

    uint16_t startIndex;

    Display::getBufferIndex(page, startCol, startIndex);

    m_writer.sendCommand(SSD1306_PAGEADDR);
    m_writer.sendCommand(page);
    m_writer.sendCommand(page);
    m_writer.sendCommand(SSD1306_COLUMNADDR);
    m_writer.sendCommand(startCol);
    m_writer.sendCommand(endCol);

    m_writer.sendDataArray((m_buffer + startIndex), endCol - startCol + 1);
  }

  Wire.setClock(100000UL);

  m_touched = false;

  for (uint8_t page = 0; page < pages(); ++page) {
    m_touchedBounds[page * 2] = width() - 1;
    m_touchedBounds[page * 2 + 1] = 0;
  }
}

void Display::getBufferPage(uint8_t y, uint8_t& page, uint8_t& offset)
{
  page = y / 8;
  offset = y % 8;
}

void Display::getBufferIndex(uint8_t page, uint8_t column, uint16_t& index)
{
  index = page * width() + column;
}

void Display::repeatDraw(uint8_t page, uint8_t column, uint8_t n, uint8_t data, Painter painter)
{
  if (page >= pages()) {
    return;
  }

  uint8_t stop = min(column + n, width());

  for (uint8_t col = column; col < stop; ++col) {
    writeBufferUnsafe(page, col, data, painter);
  }
}

void Display::drawHLine(uint8_t x0, uint8_t y0, uint8_t x1, Painter painter)
{
  if (y0 >= height()) {
    return;
  }

  x1 = min(x1, width() - 1);

  uint8_t page, offset;
  
  Display::getBufferPage(y0, page, offset);

  uint8_t mask = 0b00000001 << offset;

  for (uint8_t col = x0; col <= x1; ++col) {
    writeBufferUnsafe(page, col, mask, painter);
  }
}

void Display::drawVLine(uint8_t x0, uint8_t y0, uint8_t y1, Painter painter)
{
  this->repeatDrawVLine(x0, y0, y1, 1, painter);
}

void Display::repeatDrawVLine(uint8_t x0, uint8_t y0, uint8_t y1, uint8_t n, Painter painter)
{
  if (x0 >= width()) {
    return;
  }

  y1 = min(y1, height() - 1);

  uint8_t col = x0;

  const uint8_t page_start = y0 / 8;
  const uint8_t page_end = y1 / 8;
  uint8_t page = page_start;
  uint8_t mask;

  if (page_start != page_end) {
    mask = 0xff << (y0 % 8);
    this->repeatDraw(page, col, n, mask, painter);
    ++page;

    while(page_end > page) {
      mask = 0xff;
      this->repeatDraw(page, col, n, mask, painter);
      ++page;
    }

    mask = 0xff >> (7 - y1 % 8);
    this->repeatDraw(page, col, n, mask, painter);
  } else {
    mask = (0xff << y0 % 8) & (0xff >> (7 - y1 % 8));
    this->repeatDraw(page, col, n, mask, painter);
  }
}

void Display::drawPixel(uint8_t x0, uint8_t y0, Painter painter)
{
  if (x0 >= width() || y0 >= height()) {
    return;
  }

  uint8_t page;
  uint8_t offset;

  Display::getBufferPage(y0, page, offset);

  uint8_t mask = 0b00000001 << offset;

  writeBufferUnsafe(page, x0, mask, painter);
}

void Display::drawRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Painter color)
{
  this->drawHLine(x0, y0, x1, color);
  this->drawHLine(x0, y1, x1, color);
  this->drawVLine(x0, y0, y1, color);
  this->drawVLine(x1, y0, y1, color);
}

void Display::fillRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Painter color)
{
  this->repeatDrawVLine(x0, y0, y1, x1 - x0 + 1, color);
}

void Display::drawCircle(uint8_t x0, uint8_t y0, uint8_t r, Painter color = &Display::whitePainter)
{
  uint8_t j;
  uint16_t r2 = r * r;
  uint16_t i2;

  for (uint8_t i = 0; i <= r; ++i) {
    i2 = i * i;
    j = round(sqrt(r2 - i2));

    const uint8_t xPlus = x0 + i;
    const uint8_t xMinus = x0 - i;
    const uint8_t yPlus = y0 + j;
    const uint8_t yMinus = y0 - j;

    this->drawPixel(xPlus, yPlus, color);
    this->drawPixel(xPlus, yMinus, color);

    if (i > 0) {
      this->drawPixel(xMinus, yPlus, color);
      this->drawPixel(xMinus, yMinus, color);
    }
  }
}

void Display::fillCircle(uint8_t x0, uint8_t y0, uint8_t r, Painter color = &Display::whitePainter)
{
  uint8_t j;
  uint16_t r2 = r * r;
  uint16_t i2;

  for (uint8_t i = 0; i <= r; ++i) {
    i2 = i * i;
    j = round(sqrt(r2 - i2));

    const uint8_t xPlus = x0 + i;
    const uint8_t xMinus = x0 - i;
    const uint8_t yPlus = y0 + j;
    const uint8_t yMinus = y0 - j;

    this->drawVLine(xPlus, yMinus, yPlus, color);

    if (i > 0) {
      this->drawVLine(xMinus, yMinus, yPlus, color);
    }
  }
}

void Display::drawChar(uint8_t x0, uint8_t y0, char c, Painter painter, uint8_t xScale, uint8_t yScale)
{
  uint16_t charIdx = (c - 0x20) * 5;

  uint8_t page;
  uint8_t offset;

  uint8_t x;
  uint8_t y;

  uint8_t data;
  uint8_t dataPage;

  // For each byte in the char font
  for (uint8_t char_i = 0; char_i < 5; ++char_i) {
    uint8_t fontData = Display::pgmDereferencer(Font5x7 + charIdx + char_i);

    x = x0 + xScale * char_i;
    y = y0;

    Display::getBufferPage(y, page, offset);

    data = 0;
    dataPage = page;

    // For each bit in the char byte copy it vertically as many times as yScale
    for (uint8_t char_j = 0; char_j < 8; ++char_j) {
      uint8_t fontBit = ((fontData & (1 << char_j)) >> char_j);

      // For each repetition due to yScale
      for (uint8_t j = 0; j < yScale; ++j) {
        y = y0 + yScale * char_j + j;

        Display::getBufferPage(y, page, offset);

        if (page != dataPage) {
          // Moved to a new page, draw cumulated data byte to the buffer
          this->repeatDraw(dataPage, x, xScale, data, painter);
          data = 0;
          dataPage = page;
        }

        // Bit still in the current page, add it to the data byte
        data = data | (fontBit << offset);
      }
    }

    this->repeatDraw(dataPage, x, xScale, data, painter);
  }
}

uint8_t Display::print(uint8_t x0, uint8_t y0, const char* s, Painter color, Painter bg, uint8_t xScale, uint8_t yScale)
{
  return this->printInternal(x0, y0, s, &Display::memDereferencer, color, bg, xScale, yScale);
}

uint8_t Display::print(uint8_t x0, uint8_t y0, const __FlashStringHelper* s, Painter color, Painter bg, uint8_t xScale, uint8_t yScale)
{
  return this->printInternal(x0, y0, (const char*)s, &Display::pgmDereferencer, color, bg, xScale, yScale);
}

uint8_t Display::print(uint8_t x0, uint8_t y0, int num, Painter color, Painter bg, uint8_t xScale, uint8_t yScale)
{
  char str[8];
  snprintf(str, 8, "%d", num);
  str[7] = '\0';

  return this->print(x0, y0, str, color, bg, xScale, yScale);
}

uint8_t Display::printEmpty(uint8_t x0, uint8_t y0, uint8_t n, Painter color, uint8_t xScale, uint8_t yScale)
{
  const uint8_t x1 = x0 + n * 6 * xScale;

  this->fillRectangle(x0, y0, x1, y0 + 8 * yScale, color);

  return x1;
}

uint8_t Display::printInternal(uint8_t x0, uint8_t y0, const char* s, Dereferencer deref, Painter color, Painter bg, uint8_t xScale, uint8_t yScale)
{
  if (xScale == 1 && yScale == 1 && y0 % 8 == 0) {
    return this->printFast(x0, y0 / 8, s, deref, color, bg);
  }

  const uint8_t xStep = 6 * xScale;
  const uint8_t yStep = 8 * yScale;

  char c;

  while(c = deref(s++)) {
    this->fillRectangle(x0, y0, x0 + xStep - 1, y0 + yStep - 1, bg);
    this->drawChar(x0, y0, c, color, xScale, yScale);

    x0 += xStep;
  }

  return x0;
}

uint8_t Display::printFast(uint8_t x0, uint8_t page, const char* s, Dereferencer deref, Painter color, Painter bg)
{
  const uint8_t xStep = 6;
  const uint8_t yStep = 8;
  const uint8_t y0 = page * 8;

  char c;

  while(c = deref(s++)) {
    this->fillRectangle(x0, y0, x0 + xStep - 1, y0 + yStep - 1, bg);
    this->drawCharFast(x0, page, c, color);

    x0 += xStep;
  }

  return x0;
}

void Display::drawCharFast(uint8_t x0, uint8_t page, char c, Painter painter)
{
  uint16_t charIdx = (c - 0x20) * 5;

 // For each byte in the char font
  for (uint8_t char_i = 0; char_i < 5; ++char_i) {
    uint8_t fontData = Display::pgmDereferencer(Font5x7 + charIdx + char_i);
    writeBufferSafe(page, x0 + char_i, fontData, painter);
  }
}
