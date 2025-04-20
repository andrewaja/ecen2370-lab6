/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: drew
 */

#include "Scheduler.h"
#include "LCD_Driver.h"
#include "Timer_Driver.h"
#include "Button_Driver.h"

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdbool.h>

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define SWITCH_COLOR(color) ((color == YELLOW) ? RED : YELLOW)
#define LCD_COLOR(color) 	((color == YELLOW) ? LCD_COLOR_YELLOW : LCD_COLOR_RED)

void ApplicationInit(void);

void DisplayTitle(void);
void DisplayMenu(void);
void DisplayGame(void);
void DisplayEnd(uint32_t time);

uint8_t CheckForWinner(uint8_t x, uint8_t y, ChipColor color);
uint8_t GetNextMove();

uint32_t PlayGame(uint8_t numPlayers);
void StartGame(void);

#endif /* INC_APPLICATIONCODE_H_ */
