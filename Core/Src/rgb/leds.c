#include "main.h"
#include "argb.h"
#include "leds.h"
#include "../events/events.h"
#include "../sensors/sensors.h"

#define EVENT_IDLE_MODE_TIMEOUT		5000 // ms
#define EVENT_IDLE_MODE_STEP		30 // ms
#define MAX_RAINBOW_BRIGHT			100

void Reset_Idle_Mode() {
	Set_Event(EVNT_IDLE_MODE, EVENT_IDLE_MODE_TIMEOUT);
}

// Режим простоя без связи, просто крутим радугу и подсвечиваем клики
void Idle_Rainbow_If_Needed() {
	static uint8_t RAINBOW_STAGE = 0;
	static uint8_t R = MAX_RAINBOW_BRIGHT;
	static uint8_t G = 0;
	static uint8_t B = 0;

	if (Test_Event(EVNT_IDLE_MODE)) {
		Set_Event(EVNT_IDLE_MODE, EVENT_IDLE_MODE_STEP);

//		if (LastClicks.TotalClick) {
//			Set_Pixel_Color_RGB(2*MAX_RAINBOW_BRIGHT, 2*MAX_RAINBOW_BRIGHT, 2*MAX_RAINBOW_BRIGHT);
//			return;
//		}

		switch (RAINBOW_STAGE) {
		case 0:
			if (++G >= MAX_RAINBOW_BRIGHT) RAINBOW_STAGE++;
			break;
		case 1:
			if (--R == 0) RAINBOW_STAGE++;
			break;
		case 2:
			if (++B >= MAX_RAINBOW_BRIGHT) RAINBOW_STAGE++;
			break;
		case 3:
			if (--G == 0) RAINBOW_STAGE++;
			break;
		case 4:
			if (++R >= MAX_RAINBOW_BRIGHT) RAINBOW_STAGE++;
			break;
		case 5:
			if (--B == 0) RAINBOW_STAGE = 0;
			break;
		}

		//Set_Pixel_Color_RGB(R, G, B);
	}
}

void Reset_Leds() {
	ARGB_Init();
	ARGB_Clear();
	Reset_Idle_Mode();
}

void Led_Event_loop() {
	if (ARGB_Ready() == ARGB_READY) {
		ARGB_Show();
	}
	Idle_Rainbow_If_Needed();
}
