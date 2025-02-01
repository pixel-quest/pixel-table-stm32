#ifndef SRC_SENSORS_LOOP_H_
#define SRC_SENSORS_LOOP_H_

typedef struct {
	uint16_t Value;
	uint16_t Dacc; // RC filter
	uint16_t Dout; // RC filter
	uint16_t Offset;
	uint16_t ClickCnt;
	uint8_t DefectCnt;
	bool Click;
} Sensor_t;

extern Sensor_t Sensors[NUM_PIXELS];

void Sensors_Config();
void Reset_Sensor_Value(uint8_t p);
void Sensors_Event_loop();

#endif /* SRC_SENSORS_LOOP_H_ */
