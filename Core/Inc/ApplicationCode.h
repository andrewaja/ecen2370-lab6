/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "Button_Driver.h"
#include "Scheduler.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdbool.h>

#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SWITCH_COLOR(color) 	((color == YELLOW) ? LCD_COLOR_YELLOW : LCD_COLOR_RED)

typedef enum {
	UP_LEFT,
	UP,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT
}Direction;

void ApplicationInit(void);
void PlayGame(void);

bool CheckForWinnerHelper(uint8_t x, uint8_t y, ChipColor color, uint8_t numSame, uint8_t dir);
bool CheckForWinner(uint8_t x, uint8_t y, ChipColor color);

void PlaySinglePlayer(void);
void PlayTwoPlayer(void);

void DisplayTitle(void);
void DisplayMenu(void);
void DisplayGame(void);
void DisplayEnd(void);

#endif /* INC_APPLICATIONCODE_H_ */
