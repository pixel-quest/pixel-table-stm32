#include "main.h"
#include "vcnl36821s.h"

extern I2C_HandleTypeDef (VCNL36821S_I2C_HANDLE);

bool vcnl36821s_write(uint8_t address, uint8_t value1, uint8_t value2) {
	uint8_t data[] = {address, value1, value2};
	return HAL_I2C_Master_Transmit(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, data, 3, VCNL36821S_I2C_TIMEOUT) == HAL_OK;
}

bool vcnl36821s_init() {
	bool success = true;

	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, VCNL36821S_PS_CONF1_LOW_PS_ON | VCNL36821S_PS_CONF1_LOW_PS_RESERVERD, 0x00);
	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, VCNL36821S_PS_CONF1_LOW_PS_INIT | VCNL36821S_PS_CONF1_LOW_PS_ON | VCNL36821S_PS_CONF1_LOW_PS_RESERVERD, VCNL36821S_PS_CONF1_HIGH_RESERVED);

	return success;
}

bool vcnl36821s_read_data(uint16_t *dest) {
	uint8_t data[] = {VCNL36821S_PS_DATA};
	if (HAL_I2C_Master_Transmit(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, data, 1, VCNL36821S_I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	return HAL_I2C_Master_Receive(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, (uint8_t*)dest, 2, VCNL36821S_I2C_TIMEOUT) == HAL_OK;
}
