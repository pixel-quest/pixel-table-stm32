#include "main.h"

// Функция возвращает скорректированное по CRT значение для 8 бит ШИМ
int GetBrightCRT(uint8_t val) {
  return ((int)val * val + 255) >> 8;
}
