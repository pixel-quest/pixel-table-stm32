#include "main.h"
#include "ws2812.h"
#include "argb.h"

void Reset_Leds() {
	ARGB_Init();
	ARGB_Clear();
}

void Led_Event_loop() {
	if (ARGB_Ready() == ARGB_READY) {
		ARGB_Show();
	}
}
