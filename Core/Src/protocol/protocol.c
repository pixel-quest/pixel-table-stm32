#include "main.h"
#include "protocol.h"
#include "../config/config.h"
#include "../func/crc16.h"
#include "../events/events.h"
#include "../can/can.h"

uint8_t FormatSetColorRGB8Command(uint8_t* pData, uint8_t pixelNum, uint8_t* pColors) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_SET_COLOR_RGB8;
	pData[len++] = pixelNum;
	for (uint8_t i = 0; i < 8; i++) {
		pData[len++] = *pColors++;
	}
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatSetColorRGBCommand(uint8_t* pData, uint8_t pixelNum, uint8_t R, uint8_t G, uint8_t B) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_SET_COLOR_RGB24;
	pData[len++] = pixelNum;
	pData[len++] = R;
	pData[len++] = G;
	pData[len++] = B;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatGetClickCommand(uint8_t* pData, uint8_t pixelNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_GET_CLICK;
	pData[len++] = pixelNum;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatCalibrateZeroCommand(uint8_t* pData, uint8_t pixelNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_CALIBRATE_ZERO;
	pData[len++] = pixelNum;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatCalibrateSensCommand(uint8_t* pData, uint8_t pixelNum, int8_t pScaleKg) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_CALIBRATE_SENS;
	pData[len++] = pixelNum;
	pData[len++] = pScaleKg;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatDefectCommand(uint8_t* pData, uint8_t pixelNum, uint8_t defect) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_DEFECT;
	pData[len++] = pixelNum;
	pData[len++] = defect;
	CRC16_append(pData, &len);
	return len;
}


uint8_t FormatLineUpAddressesCommand(uint8_t* pData, uint8_t pixelNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_LINE_UP_ADDRESSES;
	pData[len++] = pixelNum;
	CRC16_append(pData, &len);
	return len;
}


uint8_t FormatReadRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_READ_REG;
	pData[len++] = pixelNum;
	pData[len++] = regNum;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatWriteRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum, uint8_t* pValue) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_WRITE_REG;
	pData[len++] = pixelNum;
	pData[len++] = regNum;
	pData[len++] = *pValue++;
	pData[len++] = *pValue++;
	pData[len++] = *pValue++;
	pData[len++] = *pValue++;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatRestartCommand(uint8_t* pData, uint8_t pixelNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_REQ_RESTART;
	pData[len++] = pixelNum;
	CRC16_append(pData, &len);
	return len;
}

uint8_t WriteReg(uint8_t* pData, uint8_t pixelNum, uint8_t regNum, int32_t value) {
	// Плохое условие, т.к у всех слейвов CAN_Address = 0...
	// Это была по сути ненужная защита от дурака, которая уже есть на фронтенде
	// тут скорее важнее, чтобы мы не ответили на это в CAN, если адрес 0, там это есть
	//if (GlobalConfig.config.CAN_Address == 0 && regNum != 0) // REGISTER_CAN_ADDRESS
	//	return 0;

	switch (regNum) {
	case 4: // REGISTER_CLICK_OFF_DUPL_MSGS
		GlobalConfig.config.Click_Off_Dupl_Msgs = (uint8_t)value;
		break;
	case 6: // REGISTER_CLICK_ON_DUPL_PER
		GlobalConfig.config.Click_Dupl_Per = (uint8_t)value;
		break;
	case 19: // REGISTER_FRAME_CLICK_THRESHOLD
		if (value < 1 || value > 255) break;
		GlobalConfig.config.Sensor_Click_Threshold = (uint8_t)value;
		break;
	case 20: // REGISTER_FRAME_CLICK_HYSTERESIS
		if (value < 1 || value > 255) break;
		GlobalConfig.config.Sensor_Click_Hysteresis = (uint8_t)value;
		break;
	}

	Save_Global_Config();
	//Set_Green_msec(250);

	return FormatRespRegCommand(pData, pixelNum, regNum);
}

void Defect(uint8_t defect) {
	if (defect >> 7 == 0) { // quiet
		//Set_Yellow_msec(250);
	}

	Save_Global_Config();
}

// Commands Responses
uint8_t FormatClickCommand(uint8_t* pData, uint8_t pixelNum, bool Click, uint8_t Status, uint8_t A, uint8_t B) {
	uint8_t len = 0;
	pData[len++] = Click ? COMMAND_RES_CLICK_ON : COMMAND_RES_CLICK_OFF;
	pData[len++] = pixelNum;
	pData[len++] = Status;
	if (Click) {
		pData[len++] = A;
		pData[len++] = B;
	}
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatLinedUpPixelCommand(uint8_t* pData, uint8_t pixelNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_RES_LINED_UP_PIXEL;
	pData[len++] = pixelNum;
	CRC16_append(pData, &len);
	return len;
}

uint8_t FormatRespRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_RES_REGISTER;
	pData[len++] = pixelNum;
	pData[len++] = regNum;

	int32_t value;
	switch (regNum) {
	case 0: // REGISTER_CAN_ADDRESS
		value = (int32_t)GlobalConfig.config.CAN_Address;
	    break;
	case 4: // REGISTER_CLICK_OFF_DUPL_MSGS
		value = (int32_t)GlobalConfig.config.Click_Off_Dupl_Msgs;
	    break;
	case 6: // REGISTER_CLICK_ON_DUPL_PER
		value = (int32_t)GlobalConfig.config.Click_Dupl_Per;
	    break;
	case 10: // REGISTER_STATUS
		//value = (int32_t)LastClicks.TotalStatus;
	    break;
	case 19: // REGISTER_FRAME_CLICK_THRESHOLD
		value = (int32_t)GlobalConfig.config.Sensor_Click_Threshold;
	    break;
	case 20: // REGISTER_FRAME_CLICK_HYSTERESIS
		value = (int32_t)GlobalConfig.config.Sensor_Click_Hysteresis;
	    break;
	}

	uint8_t* pValue = (uint8_t *)&value;

	// reversed memcpy
	uint8_t startPos = len + 3;
	for (uint8_t i = 0; i < 4; i++) {
		pData[startPos-i] = *pValue++;
		len++;
	}

	CRC16_append(pData, &len);
	return len;
}

