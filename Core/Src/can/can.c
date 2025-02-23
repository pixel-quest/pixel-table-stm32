#include "main.h"
#include "can.h"
#include "../config/config.h"
#include "../events/events.h"
#include "../rgb/leds.h"
#include "../sensors/sensors.h"

uint8_t CAN_Address = 0;
const IOPin_t SW1 = {GPIOA, LL_GPIO_PIN_14};
const IOPin_t SW2 = {GPIOA, LL_GPIO_PIN_10};
const IOPin_t SW3 = {GPIOA, LL_GPIO_PIN_9};
const IOPin_t SW4 = {GPIOA, LL_GPIO_PIN_8};

void CAN_Filter_and_Start(uint16_t device_address) {
	CAN_FilterTypeDef canFilterConfig;

	// Common part
	canFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	canFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	canFilterConfig.FilterActivation = ENABLE;
	canFilterConfig.SlaveStartFilterBank = 0;

	// Filter #0 ID List mode
	canFilterConfig.FilterBank = 0;
	canFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	canFilterConfig.FilterIdHigh = (uint32_t)(CAN_BROADCAST_ADDRESS); 							// ID #1
	canFilterConfig.FilterIdLow = (uint32_t)(CAN_TARGET_IDS_OFFSET + device_address) << 5; 		// ID #2
	canFilterConfig.FilterMaskIdHigh = 0x0000;													// ID #3 reserve
	canFilterConfig.FilterMaskIdLow = 0x0000; 													// ID #4 reserve
	if (HAL_CAN_ConfigFilter(&hcan, &canFilterConfig) != HAL_OK) { Error_Handler(); }

	// Filter #1 ID Mask mode
	// Тут внимание: т.к указан CAN_FILTERSCALE_16BIT, то это не старшая и младшая части, а два фильтра в одном банке!
	// т.е нельзя указывать FilterIdLow & FilterMaskIdLow = 0x0000, т.к тогда будет приниматься всё
	canFilterConfig.FilterBank = 1;
	canFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilterConfig.FilterIdHigh = (uint32_t)(CAN_COLOR_IDS_OFFSET + (device_address << 4)) << 5; 	// ID #1
	canFilterConfig.FilterIdLow = (uint32_t)(CAN_COLOR_IDS_OFFSET + (device_address << 4)) << 5;    // ID #2
	canFilterConfig.FilterMaskIdHigh = 0x07F0<<5;   												// Mask #1
	canFilterConfig.FilterMaskIdLow = 0x07F0<<5;    												// Mask #2
	// Принимаем 16 адресов, это значит:
	// - адресное пространство 512 адресов делится на 16 по 32
	// - значит может быть максимум 32 сегмента (а на плате пока всего 4 адресозадающих переключателя, т.е не больше 16)
	// - в 16 адресов мы можем уложить:
	//		- либо 128 RGB8 (по 8 в каждом адресе)
	//		- либо 64 RGB12 (по 4 в каждом адресе)
	//		- либо 32 RGB24 (по 2 в каждом адресе)
	//		- чего всегда достаточно, чтобы покрыть сегмент 5х5, вопрос лишь в скорости

	if (HAL_CAN_ConfigFilter(&hcan, &canFilterConfig) != HAL_OK) { Error_Handler(); }

	// Start CAN
	if (HAL_CAN_Start(&hcan) != HAL_OK) { Error_Handler(); }
}

void CAN_ReConfig_Target_Filter(uint16_t device_address) {
	if (HAL_CAN_Stop(&hcan) != HAL_OK) { Error_Handler(); }
	CAN_Filter_and_Start(device_address);
}

void CAN_Config() {
	CAN_Address = 	LL_GPIO_IsInputPinSet(SW1.port, SW1.pin) |
					LL_GPIO_IsInputPinSet(SW2.port, SW2.pin) << 1 |
					LL_GPIO_IsInputPinSet(SW3.port, SW3.pin) << 2 |
					LL_GPIO_IsInputPinSet(SW4.port, SW4.pin) << 3;

	CAN_Filter_and_Start(CAN_Address);

	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { Error_Handler(); }
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR) != HAL_OK) { Error_Handler(); }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
	Error_Handler();
}

uint8_t FormatRespRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum) {
	uint8_t len = 0;
	pData[len++] = COMMAND_RES_REGISTER;
	pData[len++] = pixelNum;
	pData[len++] = regNum;

	int32_t value;
	switch (regNum) {
	case 1: // REGISTER_RC_FILTER_K
		value = (int32_t)GlobalConfig.config.RC_Filter_K;
		break;
	case 2: // REGISTER_CLICK_THRESHOLD
		value = (int32_t)GlobalConfig.config.Sensor_Click_Threshold;
		break;
	case 3: // REGISTER_CLICK_HYSTERESIS
		value = (int32_t)GlobalConfig.config.Sensor_Click_Hysteresis;
		break;
	case 7: // REGISTER_SENSOR_OFFSET
		value = (int32_t)GlobalConfig.config.Sensor_Offset[pixelNum];
		break;
	case 8: // REGISTER_SENSOR_COEFF
		value = (int32_t)GlobalConfig.config.Sensor_Coeff[pixelNum];
		break;
	}

	uint8_t* pValue = (uint8_t *)&value;

	// reversed memcpy
	uint8_t startPos = len + 3;
	for (uint8_t i = 0; i < 4; i++) {
		pData[startPos-i] = *pValue++;
		len++;
	}

	return len;
}

uint8_t WriteReg(uint8_t* pData, uint8_t pixelNum, uint8_t regNum, int32_t value) {
	switch (regNum) {
	case 1: // REGISTER_RC_FILTER_K
		if (value < 10 || value > 50) break;
		GlobalConfig.config.RC_Filter_K = (uint8_t)value;
		break;
	case 2: // REGISTER_CLICK_THRESHOLD
		if (value < 1 || value >= SENSOR_SCALE) break;
		GlobalConfig.config.Sensor_Click_Threshold = (uint16_t)value;
		break;
	case 3: // REGISTER_CLICK_HYSTERESIS
		if (value < 1 || value > SENSOR_SCALE/2) break;
		GlobalConfig.config.Sensor_Click_Hysteresis = (uint16_t)value;
		break;
	case 8: // REGISTER_SENSOR_COEFF
		if (value < 1 || value > 255) break;
		GlobalConfig.config.Sensor_Coeff[pixelNum] = (uint16_t)value;
		break;
	}

	Save_Global_Config();
	//Set_Green_msec(250);

	return FormatRespRegCommand(pData, pixelNum, regNum);
}

void Defect(uint8_t p, uint8_t defect) {
	if (defect >> 7 == 0) { // quiet
		Set_Pixel_Yellow_msec(p, 250);
	}

	GlobalConfig.config.Sensor_Defect[p] = defect & 1;
	Save_Global_Config();

	Reset_Sensor_Value(p);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef msgHeader;
	uint8_t msgData[8];

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msgHeader, msgData) != HAL_OK) {
		Error_Handler();
	}

	if (msgHeader.IDE != CAN_ID_STD) {
		return;
	}

	uint8_t command;
	uint8_t pixelNum;

	if (msgHeader.StdId == CAN_BROADCAST_ADDRESS ||
			msgHeader.StdId == CAN_TARGET_IDS_OFFSET + CAN_Address) {
		command = msgData[0];
		pixelNum = msgData[1];
	} else if (msgHeader.StdId >= CAN_COLOR_IDS_OFFSET) {
		// Тут важно будет помнить, что для RGB8 обязательно 8 байт данных, даже если пикселей в меньше
		if (msgHeader.DLC < 8) {
			command = msgData[0]; // COMMAND_REQ_SET_COLOR_RGB12 or COMMAND_REQ_SET_COLOR_RGB24
		} else {
			command = COMMAND_REQ_SET_COLOR_RGB8;
		}

		pixelNum = (uint8_t)(msgHeader.StdId - CAN_COLOR_IDS_OFFSET - CAN_Address * 16);

		if (pixelNum > 15) return;
	} else {
		return; // ignore any other cases
	}

	Reset_Idle_Mode();

	switch (command) {
		case COMMAND_REQ_SET_COLOR_RGB8:
			if (Test_Common_Event_Clr(EVNT_RESET_FILLING)) { // ignore external commands if indicate something
				for (uint8_t i=0; i<8; i++) { // 8 pixels starts from pixelNum*8
					Set_Pixel_Color_RGB8(pixelNum*8+i, msgData[i]);
				}
			}
			break;
		case COMMAND_REQ_SET_COLOR_RGB12:
			if (Test_Common_Event_Clr(EVNT_RESET_FILLING)) { // ignore external commands if indicate something
				uint8_t R = 0, G = 0, B = 0;
				for (uint8_t i=0; i<4; i++) { // 4 pixels starts from pixelNum*4
					switch (i) {
					case 0:
						R = msgData[1] >> 4;
						G = msgData[1] & 0xF;
						B = msgData[2] >> 4;
						break;
					case 1:
						R = msgData[2] & 0xF;
						G = msgData[3] >> 4;
						B = msgData[3] & 0xF;
						break;
					case 2:
						R = msgData[4] >> 4;
						G = msgData[4] & 0xF;
						B = msgData[5] >> 4;
						break;
					case 3:
						R = msgData[5] & 0xF;
						G = msgData[6] >> 4;
						B = msgData[6] & 0xF;
						break;
					}
					Set_Pixel_Color_RGB12(pixelNum*4+i, R, G, B);
				}
			}
			break;
		case COMMAND_REQ_SET_COLOR_RGB24:
			if (Test_Common_Event_Clr(EVNT_RESET_FILLING)) { // ignore external commands if indicate something
				for (uint8_t i=0; i<2; i++) { // 2 pixels starts from pixelNum*2
					Set_Pixel_Color_RGB24(pixelNum*2+i, msgData[1+i*3], msgData[2+i*3], msgData[3+i*3]);
				}
			}
			break;
		case COMMAND_REQ_CALIBRATE_ZERO:
			for (uint8_t p=0; p<NUM_PIXELS; p++) {
				Set_Pixel_Event(p, EVNT_CALIBRATE_ZERO, 100*p);
			}
			break;
		case COMMAND_REQ_CALIBRATE_SENS:
			for (uint8_t p=0; p<NUM_PIXELS; p++) {
				Set_Pixel_Event(p, EVNT_CALIBRATE_SENS, 100*p);
			}
			break;
		case COMMAND_REQ_DEFECT:
			if (msgHeader.StdId == CAN_BROADCAST_ADDRESS) break;
			Defect(pixelNum, msgData[2]);
			break;
		case COMMAND_REQ_READ_REG:
			if (msgHeader.StdId == CAN_BROADCAST_ADDRESS || CAN_Address == 0) break;
			// - глобальная команда чтения пока не поддерживается
			// - нулевых устройств мб несколько, нельзя отвечать

			uint8_t answerData[8];
			uint8_t TransmitDataSize = FormatRespRegCommand(
				answerData,
				pixelNum,
				msgData[2]
			);
			CAN_Send(answerData, TransmitDataSize);
			break;
		case COMMAND_REQ_WRITE_REG: {
				uint8_t answerData[8];
				uint8_t TransmitDataSize = WriteReg(
					answerData,
					pixelNum,
					msgData[2],
					(int32_t)msgData[3] << 24 | (int32_t)msgData[4] << 16 | (int32_t)msgData[5] << 8 | (int32_t)msgData[6]
				);
				if (CAN_Address == 0) break; // нулевых устройств мб несколько, нельзя отвечать
				CAN_Send(answerData, TransmitDataSize);
			}
			break;
		case COMMAND_REQ_RESTART:
			HAL_NVIC_SystemReset();
			break;
	}
}

void CAN_Send(uint8_t msgData[], uint8_t len) {
	if (CAN_Address == 0 && msgData[0] != COMMAND_RES_CLICK) {
		return;
	}

	CAN_TxHeaderTypeDef msgHeader;
	msgHeader.StdId = CAN_Address + CAN_DEVICE_OUT_IDS_OFFSET;
	msgHeader.DLC = len;
	msgHeader.TransmitGlobalTime = DISABLE;
	msgHeader.RTR = CAN_RTR_DATA;
	msgHeader.IDE = CAN_ID_STD;

	if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 0) {
		uint32_t mailBoxNum = 0;

		if (HAL_CAN_AddTxMessage(&hcan, &msgHeader, msgData, &mailBoxNum) != HAL_OK) {
			Error_Handler();
		}
	}
}
