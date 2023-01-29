#ifndef Display_h
#define Display_h

#include "constants.h"

#define COLOR_WHITE 0
#define COLOR_BLACK 1
#define COLOR_INVERSE 2

class __FlashStringHelper;

/*
Implementation from scratch of the SSD1306 display driver, using a framebuffer.

It is ~5kb smaller than Adafruit SSD1306 library (and slightly faster)
It doesn't have the text background painting bugs of lcdgfx canvas implementation.

When sending the framebuffer data to the screen, only the bytes that actually changed are sent.
*/

class Display
{
public:
  typedef uint8_t (*Painter) (uint8_t, uint8_t);
  typedef uint8_t (*Dereferencer) (uint8_t*);

  Display();

  void begin(uint8_t addr);

  void clear();
  void render();

  constexpr static uint8_t width() { return SCREEN_WIDTH; }
  constexpr static uint8_t height() { return SCREEN_HEIGHT; }
  constexpr static uint16_t bufferSize() { return sizeof(m_buffer); }
  constexpr static uint8_t pages() { return SCREEN_HEIGHT / 8; }

  void setContrast(uint8_t value);

  void drawHLine(uint8_t x0, uint8_t y0, uint8_t x1, Painter color = &Display::whitePainter);
  void drawVLine(uint8_t x0, uint8_t y0, uint8_t y1, Painter color = &Display::whitePainter);

  void drawPixel(uint8_t x0, uint8_t y0, Painter color = &Display::whitePainter);

  void drawRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Painter color = &Display::whitePainter);
  void fillRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Painter color = &Display::whitePainter);

  void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, Painter color = &Display::whitePainter);
  void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, Painter color = &Display::whitePainter);

  void drawChar(uint8_t x0, uint8_t y0, char c, Painter color = &Display::whitePainter, uint8_t xScale = 1, uint8_t yScale = 1);
  void drawCharFast(uint8_t x0, uint8_t page, char c, Painter painter = &Display::whitePainter);

  uint8_t print(uint8_t x0, uint8_t y0, const char* s, Painter color = &Display::whitePainter, Painter bg = &Display::blackPainter, uint8_t xScale = 1, uint8_t yScale = 1);
  uint8_t print(uint8_t x0, uint8_t y0, const __FlashStringHelper* s, Painter color = &Display::whitePainter, Painter bg = &Display::blackPainter, uint8_t xScale = 1, uint8_t yScale = 1);
  uint8_t print(uint8_t x0, uint8_t y0, int num, Painter color = &Display::whitePainter, Painter bg = &Display::blackPainter, uint8_t xScale = 1, uint8_t yScale = 1);

  uint8_t printFast(uint8_t x0, uint8_t page, const char* s, Dereferencer deref, Painter color = &Display::whitePainter, Painter bg = &Display::blackPainter);

  uint8_t printEmpty(uint8_t x0, uint8_t y0, uint8_t n, Painter color = &Display::blackPainter, uint8_t xScale = 1, uint8_t yScale = 1);

  static void getBufferPage(uint8_t y0, uint8_t& page, uint8_t& page_offset);
  static void getBufferIndex(uint8_t page, uint8_t column, uint16_t& index);

  static Painter getPainter(uint8_t color);

  static uint8_t whitePainter(uint8_t current, uint8_t incoming);
  static uint8_t blackPainter(uint8_t current, uint8_t incoming);
  static uint8_t inversePainter(uint8_t current, uint8_t incoming);
  static uint8_t noopPainter(uint8_t current, uint8_t incoming);
  static uint8_t overwritePainter(uint8_t current, uint8_t incoming);

  static uint8_t memDereferencer(uint8_t* d);
  static uint8_t pgmDereferencer(uint8_t* d);

protected:
  uint8_t printInternal(uint8_t x0, uint8_t y0, const char* s, Dereferencer deref, Painter color = &Display::whitePainter, Painter bg = &Display::blackPainter, uint8_t xScale = 1, uint8_t yScale = 1);
  void repeatDraw(uint8_t page, uint8_t column, uint8_t n, uint8_t data, Painter painter);
  void repeatDrawVLine(uint8_t x0, uint8_t y0, uint8_t y1, uint8_t n, Painter color = &Display::whitePainter);

  void writeBufferUnsafe(uint8_t page, uint8_t column, uint8_t data, Painter painter = &Display::overwritePainter);
  void writeBufferSafe(uint8_t page, uint8_t column, uint8_t data, Painter painter = &Display::overwritePainter);
  bool m_touched = true;

  uint8_t m_touchedBounds[2 * SCREEN_HEIGHT / 8];

  class Writer
  {
  public:
    Writer();

    void sendCommand(uint8_t d);
    void sendData(uint8_t d);
    void sendCommandArray(uint8_t* arr, int size, Dereferencer deref = &Display::memDereferencer);
    void sendDataArray(uint8_t* arr, int size, Dereferencer deref = &Display::memDereferencer);

    uint8_t m_addr = 0x3d;

  protected:
    void sendOneInternal(uint8_t d, uint8_t rgstr);
    void sendManyInternal(uint8_t* arr, int size, uint8_t rgstr, Dereferencer deref = &Display::memDereferencer);
  };

  Writer m_writer;
  uint8_t m_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
};

#endif
