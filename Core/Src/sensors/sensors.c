#include "main.h"
#include "sensors.h"
#include "vcnl36821s.h"
#include "../protocol/protocol.h"
#include "../events/events.h"
#include "../config/config.h"
#include "../can/can.h"
#include "../rgb/argb.h"

// Defines
const IOPin_t S[] = {
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
		GPIO_InitStruct.Pin = S[i].pin;
		LL_GPIO_Init(S[i].port, &GPIO_InitStruct);
	}
}

void Sensors_Config() {
	for (uint8_t i=0; i<NUM_PIXELS; i++) {
		HAL_Delay(5);
		Select_Sensor(i);
		vcnl36821s_init();
	}
}

uint16_t distances[NUM_PIXELS];

void Sensors_Event_loop() {
	static uint8_t i = 0;
	Select_Sensor(i);
	uint16_t result;
	if (vcnl36821s_read(VCNL36821S_PS_DATA, &result)) {
		distances[i] = result;

		if (result < 10) result = 0;
		result *= 10;
		if (result > 255) result = 255;

		ARGB_SetRGB(i, 0, result, 0);
	}
	if (++i >= NUM_PIXELS) i = 0;
}
