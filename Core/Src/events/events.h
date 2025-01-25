#ifndef __EVENTS_H
#define __EVENTS_H

#include "main.h"

#define MEASURE_PERIOD    			(uint32_t)1   //ms
#define EVENT_DONE        			0
#define EVENT_CLR         			-1

void Clear_Events(void);
void Run_Events(void);

// ======== Common events ========
enum {
	EVNT_IDLE_MODE,
	EVNT_RESET_FILLING,
	CNT_COMMON_EVENTS  //count of registered common events
};

void Set_Common_Event(uint8_t event_id, uint32_t msec);
bool Test_Common_Event_Done(uint8_t event_id);
bool Test_Common_Event_Clr(uint8_t event_id);
void Clear_Common_Event(uint8_t event_id);
bool Test_Common_Event(uint8_t event_id);

// ======== Pixel events ========
enum {
	EVNT_RESET_PIXEL_COLOR,
	EVNT_CALIBRATE_ZERO,
	EVNT_CALIBRATE_SENS,
	CNT_PIXEL_EVENTS  //count of registered pixel events
};

void Set_Pixel_Event(uint8_t p, uint8_t event_id, uint32_t msec);
bool Test_Pixel_Event_Done(uint8_t p, uint8_t event_id);
bool Test_Pixel_Event_Clr(uint8_t p, uint8_t event_id);
void Clear_Pixel_Event(uint8_t p, uint8_t event_id);
bool Test_Pixel_Event(uint8_t p, uint8_t event_id);

#endif /* __EVENTS_H */
