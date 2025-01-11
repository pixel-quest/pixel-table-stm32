#ifndef __EVENTS_H
#define __EVENTS_H

enum {
	EVNT_IDLE_MODE,
	EVNT_RESET_PIXEL_COLOR,
	EVNT_TIME_TO_SET_PIXELS,

	CNT_EVENTS  //count of registered events
};

#define MEASURE_PERIOD    			(uint32_t)1   //ms
#define EVENT_DONE        			0
#define EVENT_CLR         			-1
#define EVENT_PARAM_SIZE 			10 // Command + PixelNum + 8 data bytes

typedef struct {
	int32_t Count;
	uint8_t P[EVENT_PARAM_SIZE]; // Params
} Event_t;

void Clear_Events(void);
void Run_Events(void);
void Set_Event(uint8_t event_id, uint32_t msec);
void Set_Event_With_Params(uint8_t event_id, uint32_t msec, uint8_t* pArray, uint8_t size);
bool Set_Event_With_Params_If_Clr(uint8_t event_id, uint32_t msec, uint8_t* pArray, uint8_t size);
Event_t Get_Event(uint8_t event_id);
bool Test_Event_Done(uint8_t event_id);
bool Test_Event_Clr(uint8_t event_id);
void Clear_Event(uint8_t event_id);
bool Test_Event(uint8_t event_id);

#endif /* __EVENTS_H */
