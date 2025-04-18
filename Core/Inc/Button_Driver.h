/*
 * Button_Driver.h
 *
 *  Created on: Feb 11, 2025
 *      Author: mrflu
 */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define BTN_PORT GPIOA
#define BTN_PIN GPIO_PIN_0
#define BTN_PRESSED ONE
#define BTN_NOT_PRESSED ZERO

void ButtonInit();
bool IsPressed();

#endif /* BUTTON_DRIVER_H_ */
