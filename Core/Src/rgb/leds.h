#ifndef SRC_RGB_LEDS_H_
#define SRC_RGB_LEDS_H_

#define EVENT_IDLE_MODE_TIMEOUT		15000 // ms
#define EVENT_IDLE_MODE_STEP		30 // ms
#define DEFAULT_BRIGHT				100

void Set_Pixel_Color_RGB8(uint8_t p, uint8_t color);
void Set_Pixel_Color_RGB12(uint8_t p, uint8_t R, uint8_t G, uint8_t B);
void Set_Pixel_Color_RGB24(uint8_t p, uint8_t R, uint8_t G, uint8_t B);

void Set_Pixel_Green_msec(uint8_t p, uint32_t msec);
void Set_Pixel_Red_msec(uint8_t p, uint32_t msec);
void Set_Pixel_Yellow_msec(uint8_t p, uint32_t msec);

void Fill_Color(uint8_t R, uint8_t G, uint8_t B);
void Fill_Green_msec(uint32_t msec);
void Fill_Red_msec(uint32_t msec);
void Fill_Yellow_msec(uint32_t msec);

void Reset_Idle_Mode();
void Init_Leds();
void Led_Event_loop();

#endif /* SRC_RGB_LEDS_H_ */
