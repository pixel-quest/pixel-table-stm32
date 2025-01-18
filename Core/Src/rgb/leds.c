#include "main.h"
#include "argb.h"
#include "leds.h"
#include "../events/events.h"
#include "../sensors/sensors.h"

void Init_Leds() {
	ARGB_Init();
	ARGB_Clear();
	Reset_Idle_Mode();
}

// CRT adjustment for 8 bit PWM
int getBrightCRT(uint8_t val) {
  return ((int)val * val + 255) >> 8;
}

// Set individual pixel color
void Set_Pixel_Color(uint8_t p, uint8_t R, uint8_t G, uint8_t B) {
	ARGB_SetRGB(p, getBrightCRT(R), getBrightCRT(G), getBrightCRT(B));
}

void Set_Pixel_Green_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color(p, 0, DEFAULT_BRIGHT, 0);
	Set_Pixel_Event(p, EVNT_RESET_PIXEL_COLOR, msec);
}

void Set_Pixel_Red_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color(p, 0, DEFAULT_BRIGHT, 0);
	Set_Pixel_Event(p, EVNT_RESET_PIXEL_COLOR, msec);
}

void Set_Pixel_Yellow_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color(p, 0, DEFAULT_BRIGHT, 0);
	Set_Pixel_Event(p, EVNT_RESET_PIXEL_COLOR, msec);
}

// Fill all pixels
void Fill_Color(uint8_t R, uint8_t G, uint8_t B) {
	ARGB_FillRGB(getBrightCRT(R), getBrightCRT(G), getBrightCRT(B));
}

void Fill_Green_msec(uint32_t msec) {
	Fill_Color(0, DEFAULT_BRIGHT, 0);
	Set_Common_Event(EVNT_RESET_FILLING, msec);
}

void Fill_Red_msec(uint32_t msec) {
	Fill_Color(DEFAULT_BRIGHT, 0, 0);
	Set_Common_Event(EVNT_RESET_FILLING, msec);
}

void Fill_Yellow_msec(uint32_t msec) {
	Fill_Color(DEFAULT_BRIGHT, DEFAULT_BRIGHT, 0);
	Set_Common_Event(EVNT_RESET_FILLING, msec);
}

// Iddle mode without CAN actions
void Reset_Idle_Mode() {
	Set_Common_Event(EVNT_IDLE_MODE, EVENT_IDLE_MODE_TIMEOUT);
}

void Idle_Rainbow_If_Needed() {
	static uint8_t RAINBOW_STAGE = 0;
	static uint8_t R = DEFAULT_BRIGHT;
	static uint8_t G = 0;
	static uint8_t B = 0;

	if (Test_Common_Event(EVNT_IDLE_MODE)) {
		Set_Common_Event(EVNT_IDLE_MODE, EVENT_IDLE_MODE_STEP);

		switch (RAINBOW_STAGE) {
		case 0:
			if (++G >= DEFAULT_BRIGHT) RAINBOW_STAGE++;
			break;
		case 1:
			if (--R == 0) RAINBOW_STAGE++;
			break;
		case 2:
			if (++B >= DEFAULT_BRIGHT) RAINBOW_STAGE++;
			break;
		case 3:
			if (--G == 0) RAINBOW_STAGE++;
			break;
		case 4:
			if (++R >= DEFAULT_BRIGHT) RAINBOW_STAGE++;
			break;
		case 5:
			if (--B == 0) RAINBOW_STAGE = 0;
			break;
		}

		for (uint8_t p=0; p<NUM_PIXELS; p++) {
			if (Sensors[p].Click) {
				ARGB_SetRGB(p, 0, 0, 0);
			} else {
				ARGB_SetRGB(p, R, G, B);
			}
		}
	}
}

void Led_Event_loop() {
	if (Test_Common_Event_Done(EVNT_RESET_FILLING)) {
		Fill_Color(0, 0, 0);
	}
	for (uint8_t p=0; p<NUM_PIXELS; p++) {
		if (Test_Pixel_Event_Done(p, EVNT_RESET_PIXEL_COLOR)) {
			Set_Pixel_Color(p, 0, 0, 0);
		}
	}
	if (ARGB_Ready() == ARGB_READY) {
		ARGB_Show();
	}
	Idle_Rainbow_If_Needed();
}
