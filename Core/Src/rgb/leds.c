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

// 7-color bitmasks
#define SET_COLOR_R_BITMASK    		(0x1U << 0)
#define SET_COLOR_G_BITMASK  		(0x1U << 1)
#define SET_COLOR_B_BITMASK  		(0x1U << 2)
#define SET_COLOR_BRIGHT_Pos		(3U)
#define SET_BRIGHT_BITMASK			(0x7U)
// 6..7 RESERVE

void Set_Pixel_Color_RGB8(uint8_t p, uint8_t color) {
	uint8_t R = 0, G = 0, B = 0;
	uint8_t bright = (color >> SET_COLOR_BRIGHT_Pos) & SET_BRIGHT_BITMASK;

	if (bright > 0) {
		bright = getBrightCRT(bright * 36 + 3); // 7 -> 255
		if ((color & SET_COLOR_R_BITMASK) != 0) R = bright;
		if ((color & SET_COLOR_G_BITMASK) != 0) G = bright;
		if ((color & SET_COLOR_B_BITMASK) != 0) B = bright;
	}

	ARGB_SetRGB(p, R, G, B);
}

// RRRR GGGG BBBB - 12 bit color
void Set_Pixel_Color_RGB12(uint8_t p, uint8_t R, uint8_t G, uint8_t B) {
	ARGB_SetRGB(p, getBrightCRT(R*17), getBrightCRT(G*17), getBrightCRT(B*17));
}

// R-byte G-byte B-byte - 24 bit color
void Set_Pixel_Color_RGB24(uint8_t p, uint8_t R, uint8_t G, uint8_t B) {
	ARGB_SetRGB(p, getBrightCRT(R), getBrightCRT(G), getBrightCRT(B));
}

// Timouted colors
void Set_Pixel_Green_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color_RGB24(p, 0, DEFAULT_BRIGHT, 0);
	Set_Pixel_Event(p, EVNT_RESET_PIXEL_COLOR, msec);
}

void Set_Pixel_Red_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color_RGB24(p, 0, DEFAULT_BRIGHT, 0);
	Set_Pixel_Event(p, EVNT_RESET_PIXEL_COLOR, msec);
}

void Set_Pixel_Yellow_msec(uint8_t p, uint32_t msec) {
	Set_Pixel_Color_RGB24(p, 0, DEFAULT_BRIGHT, 0);
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
				Set_Pixel_Color_RGB24(p, 0, 255, 0);
			} else {
				Set_Pixel_Color_RGB24(p, 0, 0, 0);
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
			Set_Pixel_Color_RGB24(p, 0, 0, 0);
		}
	}
	if (ARGB_Ready() == ARGB_READY) {
		ARGB_Show();
	}
	Idle_Rainbow_If_Needed();
}
