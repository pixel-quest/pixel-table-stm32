#include "main.h"
#include "sensors.h"
#include "vcnl36821s.h"
#include "../events/events.h"
#include "../config/config.h"
#include "../can/can.h"
#include "../rgb/leds.h"

const IOPin_t Sensor_Pins[NUM_PIXELS] = {
	{GPIOC, LL_GPIO_PIN_13},
	{GPIOB, LL_GPIO_PIN_9},
	{GPIOB, LL_GPIO_PIN_4},
	{GPIOB, LL_GPIO_PIN_3},
	{GPIOA, LL_GPIO_PIN_15},

	{GPIOC, LL_GPIO_PIN_15},
	{GPIOC, LL_GPIO_PIN_14},
	{GPIOB, LL_GPIO_PIN_5},
	{GPIOB, LL_GPIO_PIN_14},
	{GPIOB, LL_GPIO_PIN_15},

	{GPIOA, LL_GPIO_PIN_0},
	{GPIOA, LL_GPIO_PIN_3},
	{GPIOA, LL_GPIO_PIN_6},
	{GPIOB, LL_GPIO_PIN_12},
	{GPIOB, LL_GPIO_PIN_13},

	{GPIOA, LL_GPIO_PIN_1},
	{GPIOA, LL_GPIO_PIN_4},
	{GPIOA, LL_GPIO_PIN_7},
	{GPIOB, LL_GPIO_PIN_2},
	{GPIOB, LL_GPIO_PIN_11},

	{GPIOA, LL_GPIO_PIN_2},
	{GPIOA, LL_GPIO_PIN_5},
	{GPIOB, LL_GPIO_PIN_0},
	{GPIOB, LL_GPIO_PIN_1},
	{GPIOB, LL_GPIO_PIN_10},
};

Sensor_t Sensors[NUM_PIXELS] = {0};

void Select_Sensor(uint8_t p) {
	LL_GPIO_InitTypeDef GPIO_InitStruct = {
		Speed: LL_GPIO_SPEED_FREQ_LOW,
		OutputType: LL_GPIO_OUTPUT_PUSHPULL,
	};

	for (uint8_t i=0; i<NUM_PIXELS; i++) {
		if (i == p) {
			GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
		} else {
			GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
		}
		GPIO_InitStruct.Pin = Sensor_Pins[i].pin;
		LL_GPIO_Init(Sensor_Pins[i].port, &GPIO_InitStruct);
	}
	HAL_Delay(1);
}

void Reset_Sensor_Value(uint8_t p) {
	Sensors[p].Value = 0;
	Sensors[p].Dacc = 0;
	Sensors[p].Dout = 0;
	Sensors[p].DefectCnt = 0;
	Sensors[p].Click = false;
}

void Sensors_Config() {
	bool success = false;
	for (uint8_t s=0; s<NUM_PIXELS; s++) {
		HAL_Delay(100);
		Reset_Sensor_Value(s);
		Select_Sensor(s);
		success = vcnl36821s_init();
		if (success) {
			if (GlobalConfig.config.Sensor_Defect[s]) { // unDefect if it was
				GlobalConfig.config.Sensor_Defect[s] = false;
				Save_Global_Config();
			}
			Set_Pixel_Green_msec(s, 200);
		} else {
			Set_Pixel_Red_msec(s, 200);
		}
		Led_Event_loop();
	}
}

void Sensors_Event_loop() {
	static uint8_t p = 0;
	static uint16_t result = 0;
	static bool success = false;
	static bool needSend = true; // send all on start

	uint8_t s = p % NUM_PIXELS; // искусственно проредим отправку нажатий вдвое

	Select_Sensor(s);

	// ========= CALIBRATE_ZERO
	if (Test_Pixel_Event_Done(s, EVNT_CALIBRATE_ZERO)) {
		for (uint8_t attempts=0; attempts<10; attempts++) {
			success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
			if (success) {
				GlobalConfig.config.Sensor_Offset[s] = result;
				Save_Global_Config();
				break;
			}
		}
		if (success) {
			Set_Pixel_Green_msec(s, 200);
		} else {
			Set_Pixel_Red_msec(s, 200);
		}
	}

	// ========= CALIBRATE_SENS
	if (Test_Pixel_Event_Done(s, EVNT_CALIBRATE_SENS)) {
		for (uint8_t attempts=0; attempts<10; attempts++) {
			success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
			if (success) {
				uint16_t diff = SENSOR_SCALE;
				if (result > GlobalConfig.config.Sensor_Offset[s] &&
						result - GlobalConfig.config.Sensor_Offset[s] < SENSOR_SCALE) {
					diff = result - GlobalConfig.config.Sensor_Offset[s];
				}

				if (diff < MIN_SENSOR_SENS) {
					success = false;
				} else {
					GlobalConfig.config.Sensor_Coeff[s] = SENSOR_SCALE / diff;
					Save_Global_Config();
				}
				break;
			}
		}
		if (success) {
			Set_Pixel_Green_msec(s, 200);
		} else {
			Set_Pixel_Red_msec(s, 200);
		}
	}

	success = false;
	if (!GlobalConfig.config.Sensor_Defect[s]) {
		success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
	}

	if (success && result > 0) {

		Sensors[s].DefectCnt = 0;
		Sensors[s].Value = 0;
		if (result > GlobalConfig.config.Sensor_Offset[s]) {
			Sensors[s].Value = (result - GlobalConfig.config.Sensor_Offset[s]) * GlobalConfig.config.Sensor_Coeff[s];
			if (Sensors[s].Value > SENSOR_SCALE) Sensors[s].Value = SENSOR_SCALE;
		}

		// RC filter
		Sensors[s].Dacc = Sensors[s].Dacc + Sensors[s].Value - Sensors[s].Dout;
		Sensors[s].Dout = Sensors[s].Dacc / GlobalConfig.config.RC_Filter_K * 10;

		if (Sensors[s].Click && Sensors[s].Dout < (GlobalConfig.config.Sensor_Click_Threshold - GlobalConfig.config.Sensor_Click_Hysteresis)) {
		   Sensors[s].Click = false;
		   needSend = true;
		} else if (!Sensors[s].Click && Sensors[s].Dout > (GlobalConfig.config.Sensor_Click_Threshold + GlobalConfig.config.Sensor_Click_Hysteresis)) {
		   Sensors[s].Click = true;
		   needSend = true;
		}

	} else if (Sensors[s].DefectCnt == FAIL_STATUS_THRESHOLD/2) { // freezing suspicion
		vcnl36821s_init(); // try to restart
		Sensors[s].DefectCnt++;

	} else if (Sensors[s].DefectCnt < FAIL_STATUS_THRESHOLD) {

		Sensors[s].DefectCnt++;

	} else if (!GlobalConfig.config.Sensor_Defect[s]) {

		GlobalConfig.config.Sensor_Defect[s] = true; // autodefect
		Save_Global_Config();

		Sensors[s].Click = false;
		needSend = true;
	}

	// Если каждый сенсор отправлять одним байтом (адрес + дефект + клик), потребуется 3 посылки по 8 байт + 1 посылка с 1 байтом,
	// 	что очень долго и глупо, т.к у CAN большие накладные расходы, это приводит к тормозам
	// Лучше все 25 нажатий запихнуть в одну посылку 25 * 2 = 50 / 8 = 7 байт (+ байт команды) и отправлять не каждый кадр, а реже.
	// Опрос одного сенсора 1.2 msec * 25 шт = 30 мсек период, нет смысла так часто, можно смело отправлять через один
	// 	чтобы разгрузить линию для других сегментов

	if (++p >= NUM_PIXELS*2) { // искусственно проредим отправку нажатий вдвое
		if (needSend) { // send if needed
			uint8_t msgData[8] = {COMMAND_RES_CLICK,0,0,0,0,0,0,0}; // начиная с первого байта заполним посылку двойками (Defect | Click)
			uint8_t pos;
			uint8_t offset;

			for (s = 0; s < NUM_PIXELS; s++) {
				pos = s / 4 + 1; // по 4 сенсора на байт, начиная с первого
				offset = s % 4; // 3 2 1 0 7 6 5 4 ...
				msgData[pos] |= (GlobalConfig.config.Sensor_Defect[s] << (offset*2 + 1)) | (Sensors[s].Click << (offset*2)); // Defect | Click
			}

			CAN_Send(msgData, 8);
			needSend = false;
		}

	    p = 0; // loop index
	}
}
