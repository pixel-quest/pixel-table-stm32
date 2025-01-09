#ifndef SRC_FUNC_CRC16_H_
#define SRC_FUNC_CRC16_H_

uint16_t CRC16_calculate(uint8_t * buf, uint8_t len);
void CRC16_append(uint8_t *buf, uint8_t *count);
bool CRC16_check(uint8_t *buf, uint8_t count);

#endif /* SRC_FUNC_CRC16_H_ */
