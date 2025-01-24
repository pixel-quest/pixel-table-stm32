#ifndef SRC_PROTOCOL_PROTOCOL_H_
#define SRC_PROTOCOL_PROTOCOL_H_

// ================= PROTOCOL =================
// Full ID range 0 - 2047 (0x7FF)
// Address spaces:
// 		Broadcast command 	  0   (0x000) / 6 bytes data
// 		Target commands 	  512 (0x200)  - 1023 (0x3FF) = target ID filter (512 + address)
// 		Device outputs 		  1024 (0x400) - 1535 (0x5FF) = ignore / 7 bytes data
// 		Target colors 		  1536 (0x600) - 2047 (0x7FF) = target ID filter (1536 + address) / 8 bytes data
//
// Входящие:
//    Первый байт данных - номер команды (для преемственности с первым протоколом)
//    Второй – адрес пикселя
//	  Третий - номер регистра
//    Четвертый-Седьмой - данные
//
// В исходящих сообщениях (Device outputs 1024 (0x400) - 1535 (0x5FF)) обязателен формат:
// [Команда] [N пикселя] [N регистра] [1..4 байта данных]
//
// Master -> Slave сообщения:
// [Команда цвета] [N пикселя] [3 байта данных]
// [Команда клик запрос] [N пикселя]
// [Команда клик ответ] [N пикселя] [4 байта данных]
// [Команда установки адреса] [Адрес]

// CAN address spaces
#define CAN_BROADCAST_ADDRESS			0
#define CAN_TARGET_IDS_OFFSET			512
#define CAN_DEVICE_OUT_IDS_OFFSET		1024
#define CAN_COLOR_IDS_OFFSET			1536

// Commands Requests
#define COMMAND_REQ_PING                20 // 0x14 пинг для активации мастера, без ответа
#define COMMAND_REQ_SET_COLOR_RGB8 		21 // 0x15 установка цвета семицвета, без ответа
#define COMMAND_REQ_SET_COLOR_RGB24		22 // 0x16 установка цвета RGB, без ответа
#define COMMAND_REQ_RESTART 		    23 // 0x17 перезагрузка, без ответа

#define COMMAND_REQ_GET_CLICK			24 // 0x18 запрос клика, ответ COMMAND_CLICK_ON || COMMAND_CLICK_OFF
#define COMMAND_REQ_CALIBRATE_ZERO  	25 // 0x19 команда калибровки нуля, ответа нет, статус придет в ближайшем клике
#define COMMAND_REQ_CALIBRATE_SENS 		26 // 0x1A команда калибровки шкалы, ответа нет, статус придет в ближайшем клике
#define COMMAND_REQ_DEFECT              27 // 0x1B дефектовка, ответа нет, статус придет в ближайшем клике
#define COMMAND_REQ_LINE_UP_ADDRESSES   28 // 0x1C команда выстраивания адресов, ответ COMMAND_LINED_UP_PIXEL
#define COMMAND_REQ_READ_REG 			29 // 0x1D чтение регистра, овтет COMMAND_RES_REGISTER
#define COMMAND_REQ_WRITE_REG	 		30 // 0x1E запись регистра, ответ COMMAND_RES_REGISTER

// Commands Responses
#define COMMAND_RES_CLICK_ON 			31 // 0x1F ответ Status + AB ch data
#define COMMAND_RES_CLICK_OFF 			32 // 0x20 ответ Status
#define COMMAND_RES_LINED_UP_PIXEL		33 // 0x21 ответ пиксель зафиксировал адрес
#define COMMAND_RES_REGISTER			34 // 0x22 ответ регистра

// Helpers
uint8_t FormatSetColorRGB8Command(uint8_t* pData, uint8_t pixelNum, uint8_t* pColors);
uint8_t FormatSetColorRGBCommand(uint8_t* pData, uint8_t pixelNum, uint8_t R, uint8_t G, uint8_t B);
uint8_t FormatGetClickCommand(uint8_t* pData, uint8_t pixelNum);
uint8_t FormatCalibrateZeroCommand(uint8_t* pData, uint8_t pixelNum);
uint8_t FormatCalibrateSensCommand(uint8_t* pData, uint8_t pixelNum, int8_t pScaleKg);
uint8_t FormatDefectCommand(uint8_t* pData, uint8_t pixelNum, uint8_t defect);
uint8_t FormatClickCommand(uint8_t* pData, uint8_t pixelNum, bool Click, uint8_t Status, uint8_t A, uint8_t B);
uint8_t FormatLineUpAddressesCommand(uint8_t* pData, uint8_t pixelNum);
uint8_t FormatLinedUpPixelCommand(uint8_t* pData, uint8_t pixelNum);
uint8_t FormatReadRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum);
uint8_t FormatWriteRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum, uint8_t* pValue);
uint8_t FormatRespRegCommand(uint8_t* pData, uint8_t pixelNum, uint8_t regNum);
uint8_t FormatRestartCommand(uint8_t* pData, uint8_t pixelNum);
uint8_t WriteReg(uint8_t* pData, uint8_t pixelNum, uint8_t regNum, int32_t value);
void Defect(uint8_t defect);

#endif /* SRC_PROTOCOL_PROTOCOL_H_ */
