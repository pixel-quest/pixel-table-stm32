#ifndef __DEFINE_H
#define __DEFINE_H

#include "stdbool.h"
#include "string.h"

// ======= DevBoard
#define RedOn 		  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define RedOff 		  	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define RedInv 		  	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)
#define RedStatus 		HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)

// ======= FLASH Config
#define FLASH_END_ADDR 				    0x0800FFFFU // STM32F103C8
#define FLASH_CONFIG_PAGES 			    2
#define FLASH_CONFIG_PAGES_OFFSET 		0
#define FLASH_CONFIG_START_ADDR 	  	(FLASH_END_ADDR+1) - (FLASH_CONFIG_PAGES+FLASH_CONFIG_PAGES_OFFSET)*FLASH_PAGE_SIZE
#define FLASH_CONFIG_END_ADDR 		  	FLASH_CONFIG_START_ADDR + FLASH_CONFIG_PAGES*FLASH_PAGE_SIZE
#define FLASH_CONFIG_SIZE_WORDS 	  	FLASH_PAGE_SIZE/4

#define DEFAULT_DEVICE_CAN_ADDRESS 		        	0
#define DEFAULT_HX711_RC_FILTER_K					15
#define DEFAULT_HX711_COEFF						    10
#define DEFAULT_HX711_MIN_CALIBRATION_THRESHOLD		10000
#define DEFAULT_CLICK_THRESHOLD_GRAM			    2000
#define DEFAULT_CLICK_HYSTERESIS_GRAM			    500
#define DEFAULT_CLICK_OFF_DUPLICATE_MESSAGES 		2

// Интервал между запросами кликов получается примерно 2 ms
#define DEFAULT_CLICK_DUPLICATE_PER 	        	255 // * 2 ms = 500 ms

// ======= RS485
#define En485Tx			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)
#define En485Rx			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)

// ======= Touch
#define TouchPowerOn	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET)
#define TouchPowerOff	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET)
#define TouchOutPin		HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)

// ======= Frame
#define DEFAULT_FRAME_CLICK_THRESHOLD		20
#define DEFAULT_FRAME_CLICK_HYSTERESIS		5
#define DEFAULT_FRAME_COEFF					10

#endif /* __DEFINE_H */
