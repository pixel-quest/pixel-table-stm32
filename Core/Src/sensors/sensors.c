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
}

void Reset_Sensor_Value(uint8_t p) {
	Sensors[p].Value = 0;
	Sensors[p].Dacc = 0;
	Sensors[p].Dout = 0;
	Sensors[p].DefectCnt = 0;
	Sensors[p].Click = false;
	Sensors[p].NeedSend = false;
}

void Sensors_Config() {
	bool success = false;
	for (uint8_t p=0; p<NUM_PIXELS; p++) {
		HAL_Delay(100);
		Reset_Sensor_Value(p);
		Select_Sensor(p);
		success = vcnl36821s_init();
		if (success) {
			Set_Pixel_Green_msec(p, 200);
		} else {
			Set_Pixel_Red_msec(p, 200);
		}
		Led_Event_loop();
	}
}

void Sensors_Event_loop() {
	static uint8_t p = 0;
	static uint16_t result = 0;
	static bool success = false;

	Select_Sensor(p);

	// ========= CALIBRATE_ZERO
	if (Test_Pixel_Event_Done(p, EVNT_CALIBRATE_ZERO)) {
		for (uint8_t attempts=0; attempts<10; attempts++) {
			success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
			if (success) {
				GlobalConfig.config.Sensor_Offset[p] = result;
				Save_Global_Config();
				break;
			}
		}
		if (success) {
			Set_Pixel_Green_msec(p, 200);
		} else {
			Set_Pixel_Red_msec(p, 200);
		}
	}

	// ========= CALIBRATE_SENS
	if (Test_Pixel_Event_Done(p, EVNT_CALIBRATE_SENS)) {
		for (uint8_t attempts=0; attempts<10; attempts++) {
			success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
			if (success) {
				uint16_t diff = 0;
				if (result > GlobalConfig.config.Sensor_Offset[p]) {
					diff = result - GlobalConfig.config.Sensor_Offset[p];
				}

				if (diff < MIN_SENSOR_SENS) {
					success = false;
				} else {
					GlobalConfig.config.Sensor_Coeff[p] = SENSOR_SCALE / diff;
					Save_Global_Config();
				}
				break;
			}
		}
		if (success) {
			Set_Pixel_Green_msec(p, 200);
		} else {
			Set_Pixel_Red_msec(p, 200);
		}
	}

	success = false;
	if (!GlobalConfig.config.Sensor_Defect[p]) {
		success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
	}

	if (success) {

		Sensors[p].DefectCnt = 0;
		Sensors[p].Value = 0;
		if (result > GlobalConfig.config.Sensor_Offset[p]) {
			Sensors[p].Value = (result - GlobalConfig.config.Sensor_Offset[p]) * GlobalConfig.config.Sensor_Coeff[p];
			if (Sensors[p].Value > SENSOR_SCALE) Sensors[p].Value = SENSOR_SCALE;
		}

		// RC filter
		Sensors[p].Dacc = Sensors[p].Dacc + Sensors[p].Value - Sensors[p].Dout;
		Sensors[p].Dout = Sensors[p].Dacc / GlobalConfig.config.RC_Filter_K * 10;

		if (Sensors[p].Click && Sensors[p].Dout < (GlobalConfig.config.Sensor_Click_Threshold - GlobalConfig.config.Sensor_Click_Hysteresis)) {
		   Sensors[p].Click = false;
		   Sensors[p].NeedSend = true;
		} else if (!Sensors[p].Click && Sensors[p].Dout > (GlobalConfig.config.Sensor_Click_Threshold + GlobalConfig.config.Sensor_Click_Hysteresis)) {
		   Sensors[p].Click = true;
		   Sensors[p].NeedSend = true;
		}

	} else {

		if (Sensors[p].DefectCnt < FAIL_STATUS_THRESHOLD) {
			Sensors[p].DefectCnt++;
		}

		// Autodefect
		if (!GlobalConfig.config.Sensor_Defect[p] &&
				Sensors[p].DefectCnt >= FAIL_STATUS_THRESHOLD) {
			GlobalConfig.config.Sensor_Defect[p] = true;
			Save_Global_Config();

			Sensors[p].Click = false;
			Sensors[p].NeedSend = true;
		}

	}

	if (++p >= NUM_PIXELS) { // time to send clicks and loop index
	    uint8_t msgData[8];
	    uint8_t len=0;
		msgData[len++] = COMMAND_RES_CLICK;

		for (p = 0; p < NUM_PIXELS; p++) {
			if (Sensors[p].NeedSend) {
				// Address | Defect | Click
				msgData[len++] = (p << 2) | (GlobalConfig.config.Sensor_Defect[p] << 1) | Sensors[p].Click;
				Sensors[p].NeedSend = false;
			}
			if (len == 8) {
				CAN_Send(msgData, len);
				len=1;
			}
		}
		if (len > 1) CAN_Send(msgData, len);

		p = 0;
	}
}
