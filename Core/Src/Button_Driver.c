/*
 * Button_Driver.c
 *
 *  Created on: Feb 11, 2025
 *      Author: mrflu
 */

#include "Button_Driver.h"

void ButtonInit(){
	GPIO_InitTypeDef PC;
	PC.Pin = BTN_PIN;
	PC.Mode = GPIO_MODE_IT_RISING;
	PC.Pull = GPIO_NOPULL;
	PC.Speed = GPIO_SPEED_FREQ_MEDIUM;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &PC);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

bool IsPressed(){
	return HAL_GPIO_ReadPin(GPIOA, BTN_PIN);
}


