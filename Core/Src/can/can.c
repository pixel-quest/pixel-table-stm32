#include "main.h"
#include "can.h"
#include "../protocol/protocol.h"
#include "../config/config.h"
#include "../events/events.h"
#include "../rgb/leds.h"

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
	// - значит может быть максимум 32 сегмента (а на плате пока всего 4 адресозадающих переключателя)
	// - в 16 адресов мы можем уложить:
	//		- либо 128 семицветов (по 8 в каждом адресе)
	//		- либо 32 RGB (по 2 в каждом адресе)
	//		- чего достаточно, чтобы покрыть сегмент 5х5

	if (HAL_CAN_ConfigFilter(&hcan, &canFilterConfig) != HAL_OK) { Error_Handler(); }

	// Start CAN
	if (HAL_CAN_Start(&hcan) != HAL_OK) { Error_Handler(); }
}

void CAN_ReConfig_Target_Filter(uint16_t device_address) {
	if (HAL_CAN_Stop(&hcan) != HAL_OK) { Error_Handler(); }
	CAN_Filter_and_Start(device_address);
}

void CAN_Config() {
	GlobalConfig.config.CAN_Address =
			LL_GPIO_IsInputPinSet(SW1.port, SW1.pin) |
			LL_GPIO_IsInputPinSet(SW2.port, SW2.pin) << 1 |
			LL_GPIO_IsInputPinSet(SW3.port, SW3.pin) << 2 |
			LL_GPIO_IsInputPinSet(SW4.port, SW4.pin) << 3;

	CAN_Filter_and_Start(GlobalConfig.config.CAN_Address);

	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { Error_Handler(); }
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR) != HAL_OK) { Error_Handler(); }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
	Error_Handler();
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
			msgHeader.StdId == CAN_TARGET_IDS_OFFSET + GlobalConfig.config.CAN_Address) {
		command = msgData[0];
		pixelNum = msgData[1];
	} else if (msgHeader.StdId >= CAN_COLOR_IDS_OFFSET) {
		// Тут важно будет помнить, что для семицвета обязательно 8 байт данных, даже если пикселей в линейке меньше
		if (msgHeader.DLC < 8 && msgData[0] == COMMAND_REQ_SET_COLOR_RGB) {
			command = msgData[0];
			pixelNum = msgData[1];
		} else {
			command = COMMAND_REQ_SET_COLOR_BYTE;
			pixelNum = (uint8_t)(msgHeader.StdId - CAN_COLOR_IDS_OFFSET - GlobalConfig.config.CAN_Address * 8);
		}
		if (pixelNum > 7) return;
	} else {
		return; // ignore any other cases
	}

	Reset_Idle_Mode();

	switch (command) {
		case COMMAND_REQ_PING:
			__NOP();
			break;
		case COMMAND_REQ_SET_COLOR_RGB:
			if (pixelNum == 0) { // it't for me
				if (Test_Event_Clr(EVNT_RESET_PIXEL_COLOR)) { // ignore external commands if indicate something
					// Set_Pixel_Color_RGB(msgData[2], msgData[3], msgData[4]);
				}
			}
			break;
		case COMMAND_REQ_DEFECT:
			if (msgHeader.StdId == CAN_BROADCAST_ADDRESS) break;
			if (pixelNum == 0) { // it't for me
				Defect(msgData[2]);
			}
			break;
		case COMMAND_REQ_READ_REG:
			if (msgHeader.StdId == CAN_BROADCAST_ADDRESS || GlobalConfig.config.CAN_Address == 0) break;
			// - глобальная команда чтения пока не поддерживается
			// - нулевых устройств мб несколько, нельзя отвечать

			if (pixelNum == 0) { // it't for me
				uint8_t answerData[8];
				uint8_t TransmitDataSize = FormatRespRegCommand(
					answerData,
					pixelNum,
					msgData[2]
				);
				CAN_Send(answerData, TransmitDataSize);
			}
			break;
		case COMMAND_REQ_WRITE_REG:
			if (pixelNum == 0) { // it't for me
				uint8_t answerData[8];
				uint8_t TransmitDataSize = WriteReg(
					answerData,
					pixelNum,
					msgData[2],
					(int32_t)msgData[3] << 24 | (int32_t)msgData[4] << 16 | (int32_t)msgData[5] << 8 | (int32_t)msgData[6]
				);
				if (GlobalConfig.config.CAN_Address == 0) break; // нулевых устройств мб несколько, нельзя отвечать
				CAN_Send(answerData, TransmitDataSize);
			}
			break;
		case COMMAND_REQ_RESTART:
			if (pixelNum == 0) { // it't for me
				HAL_NVIC_SystemReset();
			}
			break;
	}
}

void CAN_Send(uint8_t msgData[], uint8_t len) {
	if (GlobalConfig.config.CAN_Address == 0 &&
			msgData[0] != COMMAND_RES_CLICK_ON &&
			msgData[0] != COMMAND_RES_CLICK_OFF) {
		return;
	}

	CAN_TxHeaderTypeDef msgHeader;
	msgHeader.StdId = GlobalConfig.config.CAN_Address + CAN_DEVICE_OUT_IDS_OFFSET;
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
