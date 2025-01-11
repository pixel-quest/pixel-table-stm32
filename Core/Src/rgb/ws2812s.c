#include "main.h"
#include "ws2812s.h"
#include "argb.h"

void Reset_Leds() {
	ARGB_Init();
	ARGB_Clear();
//	ARGB_SetRGB(0, 255, 0, 0);
//	ARGB_SetRGB(1, 0, 255, 0);
//	ARGB_SetRGB(2, 0, 0, 255);
//	ARGB_SetRGB(3, 255, 0, 255);
//	ARGB_SetRGB(4, 255, 255, 0);
//	ARGB_SetRGB(5, 0, 255, 255);
	ARGB_FillRGB(255, 0, 0);
	while (ARGB_Ready() != ARGB_READY) ;
	ARGB_Show();
}

void Led_Event_loop() {

}
