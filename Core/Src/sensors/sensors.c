#include "main.h"
#include "sensors.h"
#include "vcnl36821s.h"
#include "../protocol/protocol.h"
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

void Select_Sensor(uint8_t num) {
	LL_GPIO_InitTypeDef GPIO_InitStruct = {
		Speed: LL_GPIO_SPEED_FREQ_LOW,
		OutputType: LL_GPIO_OUTPUT_PUSHPULL,
	};

	for (uint8_t i=0; i<NUM_PIXELS; i++) {
		if (i == num) {
			GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
		} else {
			GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
		}
		GPIO_InitStruct.Pin = Sensor_Pins[i].pin;
		LL_GPIO_Init(Sensor_Pins[i].port, &GPIO_InitStruct);
	}
}

void Sensors_Config() {
	for (uint8_t p=0; p<NUM_PIXELS; p++) {
		HAL_Delay(5);
		Select_Sensor(p);
		vcnl36821s_init();
	}
	for (uint8_t p=0; p<NUM_PIXELS; p++) {
		Set_Pixel_Event(p, EVNT_CALIBRATE_ZERO, 100*p);
	}
}

uint16_t distances[NUM_PIXELS];

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
				Sensors[p].Offset = result;
				Set_Pixel_Green_msec(p, 200);
				break;
			}
		}
		if (!success) Set_Pixel_Red_msec(p, 200);
	}

	success = vcnl36821s_read(VCNL36821S_PS_DATA, &result);
	if (success) {
		Sensors[p].Value = 0;
		if (result > Sensors[p].Offset) {
			Sensors[p].Value = result - Sensors[p].Offset;
		}

		if (Sensors[p].Value > 10) {
			Sensors[p].Click = true;
		} else {
			Sensors[p].Click = false;
		}
	}

	if (++p >= NUM_PIXELS) p = 0;
}
