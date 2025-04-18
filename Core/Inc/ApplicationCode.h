/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: drew
 */

#include "LCD_Driver.h"
#include "Button_Driver.h"
#include "Scheduler.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdbool.h>

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define SWITCH_COLOR(color) ((color == YELLOW) ? RED : YELLOW)
#define LCD_COLOR(color) 	((color == YELLOW) ? LCD_COLOR_YELLOW : LCD_COLOR_RED)

void ApplicationInit(void);
void PlayGame(void);

bool CheckForWinner(uint8_t x, uint8_t y, ChipColor color);

void PlaySinglePlayer(void);
void PlayTwoPlayer(void);

void DisplayTitle(void);
void DisplayMenu(void);
void DisplayGame(void);
void DisplayEnd(void);

#endif /* INC_APPLICATIONCODE_H_ */
