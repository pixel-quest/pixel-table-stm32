/**
 *******************************************
 * @file    ARGB.h
 * @author  Dmitriy Semenov / Crazy_Geeks
 * @link    https://crazygeeks.ru
 * @version 1.33
 * @date	17-May-2022
 * @brief   Header file for ARGB Driver (Addressable RGB)
 *******************************************
 *
 * @note Repo: https://github.com/Crazy-Geeks/STM32-ARGB-DMA
 * @note RU article: https://crazygeeks.ru/stm32-argb-lib
 */

#ifndef ARGB_H_
#define ARGB_H_

#include "main.h"    ///< Main project file
#include <stdlib.h>  ///< Standard library
#include <stdint.h>  ///< Std types
#include <stdbool.h> ///< _Bool to bool
#include <string.h>  ///< Lib for memcpy, strlen, etc
#include <stdio.h>   ///< Lib for sprintf, printf, etc

typedef uint8_t u8_t; 	///< 8-bit unsigned
typedef int8_t i8_t;	///< 8-bit signed
typedef uint16_t u16_t; ///< 16-bit unsigned
typedef int16_t i16_t;	///< 16-bit signed
typedef uint32_t u32_t; ///< 32-bit unsigned
typedef int32_t i32_t;	///< 32-bit signed
typedef float fl_t;	///< float type

/**
 * @addtogroup ARGB_Driver
 * @brief Addressable RGB LED Driver
 * @{
 * @addtogroup User_settings
 * @brief LED & Timer's settings
 * @{
 */

#define WS2812B       ///< Family: {WS2811S, WS2811F, WS2812, SK6812, WS2812B}
// WS2811S — RGB, 400kHz;
// WS2811F — RGB, 800kHz;
// WS2812  — GRB, 800kHz;
// WS2812B — RGB, 800kHz;
// SK6812  — RGBW, 800kHz

#define USE_GAMMA_CORRECTION 1 ///< Gamma-correction should fix red&green, try for yourself

#define TIM_NUM	   		4  ///< Timer number
#define TIM_CH	   		TIM_CHANNEL_3  ///< Timer's PWM channel
#define DMA_HANDLE 		hdma_tim4_ch3  ///< DMA Channel
#define DMA_SIZE_BYTE     ///< DMA Memory Data Width: {.._BYTE, .._HWORD, .._WORD}
// DMA channel can be found in main.c / tim.c

/// @}

/**
 * @addtogroup Global_entities
 * @brief All driver's methods
 * @{
 * @enum ARGB_STATE
 * @brief Driver's status enum
 */
typedef enum ARGB_STATE {
    ARGB_BUSY = 0,  ///< DMA Transfer in progress
    ARGB_READY = 1, ///< DMA Ready to transfer
    ARGB_OK = 2,    ///< Function execution success
    ARGB_PARAM_ERR = 3, ///< Error in input parameters
} ARGB_STATE;

void ARGB_Init(void);   // Initialization
void ARGB_Clear(void);  // Clear strip

void ARGB_SetBrightness(u8_t br); // Set global brightness

void ARGB_SetRGB(u16_t i, u8_t r, u8_t g, u8_t b);  // Set single LED by RGB
void ARGB_SetHSV(u16_t i, u8_t hue, u8_t sat, u8_t val); // Set single LED by HSV
void ARGB_SetWhite(u16_t i, u8_t w); // Set white component in LED (RGBW)

void ARGB_FillRGB(u8_t r, u8_t g, u8_t b); // Fill all strip with RGB color
void ARGB_FillHSV(u8_t hue, u8_t sat, u8_t val); // Fill all strip with HSV color
void ARGB_FillWhite(u8_t w); // Fill all strip's white component (RGBW)

ARGB_STATE ARGB_Ready(void); // Get DMA Ready state
ARGB_STATE ARGB_Show(void); // Push data to the strip

/// @} @}
#endif /* ARGB_H_ */
