#include "main.h"
#include "ws2812.h"
#include "argb.h"
#include "../events/events.h"

void Reset_Leds() {
	ARGB_Init();
	ARGB_Clear();
	Set_Event(EVNT_TIME_TO_SET_PIXELS, 100);
}

void Led_Event_loop() {
	static uint8_t pos = 0;
	static uint8_t stage = 0;
	static uint8_t colorR = 255;
	static uint8_t colorG = 0;
	static uint8_t colorB = 0;

	if (Test_Event_Done(EVNT_TIME_TO_SET_PIXELS)) {
		for (uint8_t i = 0; i < NUM_PIXELS; i++) {
			if (i == pos) {
				ARGB_SetRGB(i, colorR, colorG, colorB);
			} else {
				ARGB_SetRGB(i, 0, 0, 0);
			}
		}

		while (ARGB_Ready() != ARGB_READY) ;
		ARGB_Show();

		if (++pos >= NUM_PIXELS) {
			pos = 0;
			switch (stage) {
			case 0:
				colorG = 255;
				break;
			case 1:
				colorR = 0;
				break;
			case 2:
				colorB = 255;
				break;
			case 3:
				colorG = 0;
				break;
			case 4:
				colorR = 255;
				break;
			case 5:
				colorB = 0;
				break;
			}
			if (++stage > 5) stage = 0;
		}

		Set_Event(EVNT_TIME_TO_SET_PIXELS, 100);
	}
}
