#include "main.h"
#include "events.h"

volatile Event_t Events[CNT_EVENTS];

void Clear_Events(void) {
  for (uint8_t event_id=0; event_id<CNT_EVENTS;event_id++) {
    Clear_Event(event_id);
  }
}

void Run_Events(void) {
  for (uint8_t event_id=0; event_id<CNT_EVENTS; event_id++) {
    if (Events[event_id].Count>EVENT_DONE) Events[event_id].Count--;
  }
}

void Set_Event(uint8_t event_id, uint32_t msec) {
  Set_Event_With_Params(event_id, msec, NULL, 0);
}

void Set_Event_With_Params(uint8_t event_id, uint32_t msec, uint8_t* pArray, uint8_t size) {
	if (msec == 0) msec++;
	// Похоже 0 нельзя, т.к прерывания DMA могут накладываться и сразу получается -1
	// я искусственно апнул все 0 на 1, чтобы больше не наступать на эти грабли,
	// т.к не понимаю, как накладываются другие типы прерываний

	Events[event_id].Count = msec / MEASURE_PERIOD;
	for (uint8_t i = 0; i < EVENT_PARAM_SIZE; i++) {
	  if (pArray != NULL && i < size) {
		  Events[event_id].P[i] = pArray[i];
	  } else {
		  Events[event_id].P[i] = 0;
	  }
	}
}

bool Set_Event_With_Params_If_Clr(uint8_t event_id, uint32_t msec, uint8_t* pArray, uint8_t size) {
	if (!Test_Event_Clr(event_id)) return false;
	Set_Event_With_Params(event_id, msec, pArray, size);
	return true;
}

Event_t Get_Event(uint8_t event_id) {
	return Events[event_id];
}

bool Test_Event(uint8_t event_id) {
	return Events[event_id].Count == EVENT_DONE;
}

bool Test_Event_Done(uint8_t event_id) {
  if (Events[event_id].Count == EVENT_DONE) {
	  Clear_Event(event_id);
	  return true;
  }

  return false;
}

bool Test_Event_Clr(uint8_t event_id) {
  return Events[event_id].Count <= EVENT_CLR;
}

void Clear_Event(uint8_t event_id) {
  Events[event_id].Count = EVENT_CLR;
  // ! Don't clear Params because it's used after Test_Event_Done
}
