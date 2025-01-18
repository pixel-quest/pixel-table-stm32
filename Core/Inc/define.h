#ifndef __DEFINE_H
#define __DEFINE_H

#include "stdbool.h"
#include "string.h"

#define NUM_PIXELS 		25

// ======= FLASH Config
#define FLASH_END_ADDR 				    0x0800FFFFU // STM32F103C8
#define FLASH_CONFIG_PAGES 			    2
#define FLASH_CONFIG_PAGES_OFFSET 		0
#define FLASH_CONFIG_START_ADDR 	  	(FLASH_END_ADDR+1) - (FLASH_CONFIG_PAGES+FLASH_CONFIG_PAGES_OFFSET)*FLASH_PAGE_SIZE
#define FLASH_CONFIG_END_ADDR 		  	FLASH_CONFIG_START_ADDR + FLASH_CONFIG_PAGES*FLASH_PAGE_SIZE
#define FLASH_CONFIG_SIZE_WORDS 	  	FLASH_PAGE_SIZE/4

#define DEFAULT_DEVICE_CAN_ADDRESS 		        	0
#define DEFAULT_CLICK_OFF_DUPLICATE_MESSAGES 		2
#define DEFAULT_CLICK_THRESHOLD					    10
#define DEFAULT_CLICK_HYSTERESIS			  		3
#define DEFAULT_SENSOR_COEFF						100 // %

// ======= IOPin_t
typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
} IOPin_t;

#endif /* __DEFINE_H */
