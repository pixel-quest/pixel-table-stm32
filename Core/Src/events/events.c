#include "main.h"
#include "events.h"

// ======== Common events ========
volatile int32_t Common_Events[CNT_COMMON_EVENTS];

void Set_Common_Event(uint8_t event_id, uint32_t msec) {
	if (msec == 0) msec++;
	// Похоже 0 нельзя, т.к прерывания DMA могут накладываться и сразу получается -1
	// я искусственно апнул все 0 на 1, чтобы больше не наступать на эти грабли,
	// т.к не понимаю, как накладываются другие типы прерываний

	Common_Events[event_id] = msec / MEASURE_PERIOD;
}

bool Test_Common_Event(uint8_t event_id) {
	return Common_Events[event_id] == EVENT_DONE;
}

bool Test_Common_Event_Done(uint8_t event_id) {
  if (Common_Events[event_id] == EVENT_DONE) {
	  Clear_Common_Event(event_id);
	  return true;
  }

  return false;
}

bool Test_Common_Event_Clr(uint8_t event_id) {
  return Common_Events[event_id] <= EVENT_CLR;
}

void Clear_Common_Event(uint8_t event_id) {
  Common_Events[event_id] = EVENT_CLR;
}

// ======== Pixel events ========
volatile int32_t Pixel_Events[CNT_PIXEL_EVENTS][NUM_PIXELS];

void Set_Pixel_Event(uint8_t p, uint8_t event_id, uint32_t msec) {
	if (msec == 0) msec++;
	// Похоже 0 нельзя, т.к прерывания DMA могут накладываться и сразу получается -1
	// я искусственно апнул все 0 на 1, чтобы больше не наступать на эти грабли,
	// т.к не понимаю, как накладываются другие типы прерываний

	Pixel_Events[event_id][p] = msec / MEASURE_PERIOD;
}

bool Test_Pixel_Event(uint8_t p, uint8_t event_id) {
	return Pixel_Events[event_id][p] == EVENT_DONE;
}

bool Test_Pixel_Event_Done(uint8_t p, uint8_t event_id) {
	if (Pixel_Events[event_id][p] == EVENT_DONE) {
	  Clear_Pixel_Event(p, event_id);
	  return true;
	}

	return false;
}

bool Test_Pixel_Event_Clr(uint8_t p, uint8_t event_id) {
	return Pixel_Events[event_id][p] <= EVENT_CLR;
}

void Clear_Pixel_Event(uint8_t p, uint8_t event_id) {
	Pixel_Events[event_id][p] = EVENT_CLR;
}

// ======== Summary Events ========
void Clear_Events(void) {
	uint8_t event_id;
	for (event_id=0; event_id<CNT_COMMON_EVENTS;event_id++) {
		Clear_Common_Event(event_id);
	}

	uint8_t p;
	for (event_id=0; event_id<CNT_PIXEL_EVENTS;event_id++) {
		for (p=0; p<NUM_PIXELS; p++) {
			Clear_Pixel_Event(p, event_id);
		}
	}
}

void Run_Events(void) {
	uint8_t event_id;
	for (event_id=0; event_id<CNT_COMMON_EVENTS; event_id++) {
		if (Common_Events[event_id]>EVENT_DONE) Common_Events[event_id]--;
	}

	uint8_t p;
	for (event_id=0; event_id<CNT_PIXEL_EVENTS; event_id++) {
		for (p=0; p<NUM_PIXELS; p++) {
			if (Pixel_Events[event_id][p]>EVENT_DONE) Pixel_Events[event_id][p]--;
		}
	}
}
