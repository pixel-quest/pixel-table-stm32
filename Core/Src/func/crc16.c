#include "main.h"

uint16_t CRC16_update(uint16_t crc, uint8_t a) {
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}

uint16_t CRC16_calculate(uint8_t * buf, uint8_t len) {
  uint16_t u16CRC = 0xFFFF;
  
  for (uint8_t i = 0; i < len; i++) {
    u16CRC = CRC16_update(u16CRC, buf[i]);
  }

  return u16CRC;
}

void CRC16_append(uint8_t * buf, uint8_t * count) {
  uint8_t len = *count;    
  
  uint16_t u16CRC = CRC16_calculate(buf, len);
  
  buf[len++] = u16CRC & 0xff;
  buf[len++] = u16CRC >> 8;
  buf[len] = 0;
  
  *count = len;
}

bool CRC16_check(uint8_t * buf, uint8_t len) {
  uint16_t u16CRC = CRC16_calculate(buf, len-2);
  uint16_t u16CRCrec = (uint16_t)buf[len-1]<<8 | buf[len-2];
  
  if (u16CRC == u16CRCrec) {
    return true;
  }
  return false;
}
