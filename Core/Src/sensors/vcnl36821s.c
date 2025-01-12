#include "main.h"
#include "vcnl36821s.h"

extern I2C_HandleTypeDef (VCNL36821S_I2C_HANDLE);

bool vcnl36821s_write(uint8_t address, uint8_t value1, uint8_t value2) {
	uint8_t data[] = {address, value1, value2};
	return HAL_I2C_Master_Transmit(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, data, sizeof(data), VCNL36821S_I2C_TIMEOUT) == HAL_OK;
}

bool vcnl36821s_init() {
	bool success = true;
	uint8_t PS_CONF1_LOW = 0x01; // must be always stays 1
	uint8_t PS_CONF1_HIGH = 0x00;

	PS_CONF1_LOW |= VCNL36821S_PS_CONF1_LOW_PS_ON;
	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, PS_CONF1_LOW, PS_CONF1_HIGH);

	PS_CONF1_LOW |= VCNL36821S_PS_CONF1_LOW_PS_INIT;
	PS_CONF1_HIGH |= VCNL36821S_PS_CONF1_HIGH_RESERVED;
	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, PS_CONF1_LOW, PS_CONF1_HIGH);

	success &= vcnl36821s_write(VCNL36821S_PS_CONF2, VCNL36821S_PS_ST_START, 0x00);

	return success;
}

bool vcnl36821s_read(uint16_t *dest) {
	uint8_t data[] = {VCNL36821S_PS_DATA};
	if (HAL_I2C_Master_Transmit(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, data, 1, VCNL36821S_I2C_TIMEOUT) != HAL_OK) {
		return false;
	}

	return HAL_I2C_Master_Receive(&VCNL36821S_I2C_HANDLE, VCNL36821S_SLAVE_ADDRESS, (uint8_t*)dest, 2, VCNL36821S_I2C_TIMEOUT) == HAL_OK;
}
