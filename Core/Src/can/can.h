#ifndef __CAN_H
#define __CAN_H

// ================= PROTOCOL =================
// Full ID range 0 - 2047 (0x7FF)
// Address spaces:
// 		Broadcast command 	  0   (0x000) / 6 bytes data
// 		Target commands 	  512 (0x200)  - 1023 (0x3FF) = target ID filter (512 + address)
// 		Device outputs 		  1024 (0x400) - 1535 (0x5FF) = ignore / 7 bytes data
// 		Target colors 		  1536 (0x600) - 2047 (0x7FF) = target ID filter (1536 + address) / 8 bytes data
//
// В исходящих сообщениях (Device outputs 1024 (0x400) - 1535 (0x5FF)) обязателен формат:
// [Команда] [N пикселя] [N регистра] [1..4 байта данных]

// CAN address spaces
#define CAN_BROADCAST_ADDRESS			0
#define CAN_TARGET_IDS_OFFSET			512
#define CAN_DEVICE_OUT_IDS_OFFSET		1024
#define CAN_COLOR_IDS_OFFSET			1536

// Commands Requests
#define COMMAND_REQ_SET_COLOR_RGB8 		21 // 0x15 установка цвета RGB8, без ответа
#define COMMAND_REQ_SET_COLOR_RGB12		22 // 0x16 установка цвета RGB12, без ответа
#define COMMAND_REQ_SET_COLOR_RGB24		23 // 0x17 установка цвета RGB24, без ответа
#define COMMAND_REQ_RESTART 		    24 // 0x18 перезагрузка, без ответа

#define COMMAND_REQ_CALIBRATE_ZERO  	26 // 0x1A команда калибровки нуля, ответа нет, статус придет в ближайшем клике
#define COMMAND_REQ_CALIBRATE_SENS 		27 // 0x1B команда калибровки шкалы, ответа нет, статус придет в ближайшем клике
#define COMMAND_REQ_DEFECT              28 // 0x1C дефектовка, ответа нет, статус придет в ближайшем клике

#define COMMAND_REQ_READ_REG 			30 // 0x1E чтение регистра, овтет COMMAND_RES_REGISTER
#define COMMAND_REQ_WRITE_REG	 		31 // 0x1F запись регистра, ответ COMMAND_RES_REGISTER

// Commands Responses
#define COMMAND_RES_CLICK_ON 			32 // 0x20 ответ Status + Click data
#define COMMAND_RES_CLICK_OFF 			33 // 0x21 ответ Status

#define COMMAND_RES_REGISTER			35 // 0x23 ответ регистра

void CAN_Config();
void CAN_Send(uint8_t msgData[], uint8_t len);

#endif /* __CAN_H */
