#ifndef ListView_h
#define ListView_h

#include "constants.h"

class ListView {
public:
  struct ItemGetter {
    void* boundObj = nullptr;
    void (*getter) (void* boundObj, uint8_t i, char* s, uint8_t n) = nullptr;
  };

  ListView();
  virtual ~ListView();

  void setTitle(const char* title);

  void bounds(uint8_t b[4]);
  void setBounds(uint8_t b[4]);

  void setItems(ItemGetter itemGetter, uint8_t n);

  uint8_t size() const;

  uint8_t selected() const;
  void setSelected(int8_t id);
  void selectNext(uint8_t step = 1);
  void selectPrev(uint8_t step = 1);

protected:
  void drawItems();

  uint8_t itemsPerPage() const;


  uint8_t m_bounds[4] = {0, 0, 128, 64};
  uint8_t m_size = 0;

  uint8_t m_offset = 0;
  uint8_t m_selected = 0;

  ItemGetter m_itemGetter;
};

#endif
