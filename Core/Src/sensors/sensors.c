#include "main.h"
#include "sensors.h"
#include "../protocol/protocol.h"
#include "../events/events.h"
#include "../config/config.h"
#include "../can/can.h"

// Defines
const IOPin_t S1 = {GPIOC, LL_GPIO_PIN_13};
const IOPin_t S2 = {GPIOB, LL_GPIO_PIN_9};
const IOPin_t S3 = {GPIOB, LL_GPIO_PIN_4};
const IOPin_t S4 = {GPIOB, LL_GPIO_PIN_3};
const IOPin_t S5 = {GPIOA, LL_GPIO_PIN_15};

const IOPin_t S6 = {GPIOC, LL_GPIO_PIN_15};
const IOPin_t S7 = {GPIOC, LL_GPIO_PIN_14};
const IOPin_t S8 = {GPIOB, LL_GPIO_PIN_5};
const IOPin_t S9 = {GPIOB, LL_GPIO_PIN_14};
const IOPin_t S10 = {GPIOB, LL_GPIO_PIN_15};

const IOPin_t S11 = {GPIOA, LL_GPIO_PIN_0};
const IOPin_t S12 = {GPIOA, LL_GPIO_PIN_3};
const IOPin_t S13 = {GPIOA, LL_GPIO_PIN_6};
const IOPin_t S14 = {GPIOB, LL_GPIO_PIN_12};
const IOPin_t S15 = {GPIOB, LL_GPIO_PIN_13};

const IOPin_t S16 = {GPIOA, LL_GPIO_PIN_1};
const IOPin_t S17 = {GPIOA, LL_GPIO_PIN_4};
const IOPin_t S18 = {GPIOA, LL_GPIO_PIN_7};
const IOPin_t S19 = {GPIOB, LL_GPIO_PIN_2};
const IOPin_t S20 = {GPIOB, LL_GPIO_PIN_11};

const IOPin_t S21 = {GPIOA, LL_GPIO_PIN_2};
const IOPin_t S22 = {GPIOA, LL_GPIO_PIN_5};
const IOPin_t S23 = {GPIOB, LL_GPIO_PIN_0};
const IOPin_t S24 = {GPIOB, LL_GPIO_PIN_1};
const IOPin_t S25 = {GPIOB, LL_GPIO_PIN_10};

//LL_GPIO_SetPinMode ( MDIO_GPIO_Port, MDIO_Pin, LL_GPIO_MODE_OUTPUT );
//LL_GPIO_SetPinMode ( MDIO_GPIO_Port, MDIO_Pin, LL_GPIO_MODE_INPUT );

void Sensors_Config() {

}

void Sensors_Event_loop() {

}
