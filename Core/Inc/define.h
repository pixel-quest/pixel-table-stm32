#ifndef __DEFINE_H
#define __DEFINE_H

#include "stdbool.h"
#include "string.h"

#define NUM_PIXELS 			20
#define NUM_WS2812_LEDS 	40

// ======= FLASH Config
#define FLASH_END_ADDR 				    0x0800FFFFU // STM32F103C8
#define FLASH_CONFIG_PAGES 			    2
#define FLASH_CONFIG_PAGES_OFFSET 		0
#define FLASH_CONFIG_START_ADDR 	  	(FLASH_END_ADDR+1) - (FLASH_CONFIG_PAGES+FLASH_CONFIG_PAGES_OFFSET)*FLASH_PAGE_SIZE
#define FLASH_CONFIG_END_ADDR 		  	FLASH_CONFIG_START_ADDR + FLASH_CONFIG_PAGES*FLASH_PAGE_SIZE
#define FLASH_CONFIG_SIZE_WORDS 	  	FLASH_PAGE_SIZE/4

// 1.2 msec * 25 sensors = 30 msec per period
#define DEFAULT_CLICK_DUPLICATE_PER					33 // periods ~ 1 sec
#define DEFAULT_CLICK_OFF_DUPLICATE_MESSAGES 		1
#define FAIL_STATUS_THRESHOLD						100 // periods ~ 3 sec autodefect

#define MIN_SENSOR_SENS								10
#define SENSOR_SCALE								1000
#define DEFAULT_CLICK_THRESHOLD					    100
#define DEFAULT_CLICK_HYSTERESIS			  		10
#define DEFAULT_SENSOR_COEFF						10
#define DEFAULT_RC_FILTER_K							10

// ======= IOPin_t
typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
} IOPin_t;

#endif /* __DEFINE_H */
