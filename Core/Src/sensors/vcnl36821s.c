#include "main.h"
#include "vcnl36821s.h"
#include "i2c_er.h"

extern I2C_HandleTypeDef (hi2c);

bool vcnl36821s_write(uint8_t reg, uint8_t value1, uint8_t value2) {
	uint8_t data[] = {reg, value1, value2};

	HAL_StatusTypeDef status;
	for (uint8_t i = 0; i < 10; i++) { // I2C_ClearBusyFlagErratum() срабатывает не с первого раза
		status = HAL_I2C_Master_Transmit(&hi2c, VCNL36821S_SLAVE_ADDRESS, data, sizeof(data), VCNL36821S_I2C_TIMEOUT);
		if (status == HAL_BUSY)
			I2C_ClearBusyFlagErratum(&hi2c, VCNL36821S_I2C_TIMEOUT);
		else break;
	}

	return status == HAL_OK;
}

bool vcnl36821s_init() {
	bool success = true;

	// Default values
	//	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, 0x01, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_CONF2, 0x00, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_CONF34, 0x00, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_THDL, 0x00, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_THDH, 0x00, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_CANC, 0x00, 0x00);
	//	success &= vcnl36821s_write(VCNL36821S_PS_AC_L, 0x00, 0x00);

	// Lets start
	uint8_t PS_CONF1_LOW = 0x01; // must be always stays 1
	uint8_t PS_CONF1_HIGH = 0x00;

	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, PS_CONF1_LOW, PS_CONF1_HIGH);

	PS_CONF1_LOW |= VCNL36821S_PS_CONF1_LOW_PS_ON;
	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, PS_CONF1_LOW, PS_CONF1_HIGH);

	PS_CONF1_LOW |= VCNL36821S_PS_CONF1_LOW_PS_INIT;
	PS_CONF1_HIGH |= VCNL36821S_PS_CONF1_HIGH_RESERVED;
	success &= vcnl36821s_write(VCNL36821S_PS_CONF1, PS_CONF1_LOW, PS_CONF1_HIGH);

	success &= vcnl36821s_write(VCNL36821S_PS_CONF2, VCNL36821S_PS_ST_START | VCNL36821S_PS_PERIOD_20ms, 0x00);

	success &= vcnl36821s_write(VCNL36821S_PS_CONF34, 0x00, VCNL36821S_PS_SC | VCNL36821S_LED_I_156ma);

	return success;
}

bool vcnl36821s_read(uint8_t reg, uint16_t *dest) {
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(&hi2c, VCNL36821S_SLAVE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)dest, 2, VCNL36821S_I2C_TIMEOUT);
	if (status == HAL_BUSY)
		I2C_ClearBusyFlagErratum(&hi2c, VCNL36821S_I2C_TIMEOUT);
	else if (status != HAL_OK) {
		return false;
	}

	return status == HAL_OK;
}
