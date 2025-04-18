/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

static Gameboard gameboard;
static uint8_t currentColumn = 1;
static ChipColor winner;

extern void initialise_monitor_handles(void); 

static STMPE811_TouchData StaticTouchData;

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
	ButtonInit();
	LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

	InitializeLCDTouch();
	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;
}

void PlayGame(void)
{
	DisplayTitle();
	HAL_Delay(2000);
	while(1){
		DisplayMenu();
		uint8_t numPlayers = selectMode(&StaticTouchData);
		DisplayGame();
		HAL_Delay(500);
		switch (numPlayers){
			case 1:
				PlaySinglePlayer();
			break;
			case 2:
				PlayTwoPlayer();
			break;
			default:
				LCD_Clear(0, LCD_COLOR_RED);
				return;
		}
		DisplayEnd();
	}
}

void DisplayTitle(void){
	LCD_Clear(0,LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(55,LCD_PIXEL_HEIGHT/2,'C');
	LCD_DisplayChar(70,LCD_PIXEL_HEIGHT/2,'o');
	LCD_DisplayChar(85,LCD_PIXEL_HEIGHT/2,'n');
	LCD_DisplayChar(100,LCD_PIXEL_HEIGHT/2,'n');
	LCD_DisplayChar(115,LCD_PIXEL_HEIGHT/2,'e');
	LCD_DisplayChar(130,LCD_PIXEL_HEIGHT/2,'c');
	LCD_DisplayChar(140,LCD_PIXEL_HEIGHT/2,'t');
	LCD_DisplayChar(165,LCD_PIXEL_HEIGHT/2,'4');
}

void DisplayMenu(void){
	LCD_Clear(0,LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font12x12);

	uint16_t x, y;
	for(y=10; y<50; y++){
		for(x=15; x < LCD_PIXEL_WIDTH-15; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_WHITE);
		}
	}
	LCD_DisplayChar(78,25,'O');
	LCD_DisplayChar(87,25,'n');
	LCD_DisplayChar(94,25,'e');
	LCD_DisplayChar(103,25,'P');
	LCD_DisplayChar(110,25,'l');
	LCD_DisplayChar(113,25,'a');
	LCD_DisplayChar(120,25,'y');
	LCD_DisplayChar(127,25,'e');
	LCD_DisplayChar(134,25,'r');
	LCD_DisplayChar(141,25,'M');
	LCD_DisplayChar(150,25,'o');
	LCD_DisplayChar(157,25,'d');
	LCD_DisplayChar(164,25,'e');

	x = 0; y = 0;
	for(y=60; y<100; y++){
		for(x=15; x < LCD_PIXEL_WIDTH-15; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_WHITE);
		}
	}
	LCD_DisplayChar(78,75,'T');
	LCD_DisplayChar(85,75,'w');
	LCD_DisplayChar(94,75,'o');
	LCD_DisplayChar(103,75,'P');
	LCD_DisplayChar(110,75,'l');
	LCD_DisplayChar(113,75,'a');
	LCD_DisplayChar(120,75,'y');
	LCD_DisplayChar(127,75,'e');
	LCD_DisplayChar(134,75,'r');
	LCD_DisplayChar(141,75,'M');
	LCD_DisplayChar(150,75,'o');
	LCD_DisplayChar(157,75,'d');
	LCD_DisplayChar(164,75,'e');

	displayFilledBoard();
}

void DisplayGame(void){
	LCD_Clear(0,LCD_COLOR_BLUE);
	displayEmptyBoard();
}

void DisplayEnd(void){
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	uint16_t x, y;
	for(y = 0; y<50; y++){
		for(x=0; x < LCD_PIXEL_WIDTH; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_BLUE);
		}
	}
	LCD_DisplayChar(85,30,'W');
	LCD_DisplayChar(95,30,'i');
	LCD_DisplayChar(105,30,'n');
	LCD_DisplayChar(120,30,'n');
	LCD_DisplayChar(135,30,'e');
	LCD_DisplayChar(150,30,'r');
	LCD_DisplayChar(155,30,':');

	// TODO
	HAL_Delay(3000);
}

void PlaySinglePlayer(){
	// TODO
	HAL_Delay(2000);
}

/* Direction Macros
 * 	   1 2 3
 * 	   8 x 4
 *     7 6 5
 */

bool CheckForWinnerHelper(uint8_t x, uint8_t y, ChipColor color, uint8_t numSame, Direction dir){
	if (numSame == 3)
		return true; // base case

	switch (dir){
		case UP_LEFT:
			if (x > 0 && y < NUM_ROWS-1){
				ChipColor upLeft = gameboard.column[x-1].contents[y+1];
				if (upLeft == color)
					return CheckForWinnerHelper(x-1, y+1, color, numSame+1, UP_LEFT);
			}
			return false;
		break;
		case UP:
			if (y < NUM_ROWS-1){
				ChipColor up = gameboard.column[x].contents[y+1];
				if (up == color)
					return CheckForWinnerHelper(x, y+1, color, numSame+1, UP);
			}
			return false;
		break;
		case UP_RIGHT:
			if (x < NUM_COLUMNS-1 && y < NUM_ROWS-1){
				ChipColor upRight = gameboard.column[x+1].contents[y+1];
				if (upRight == color)
					return CheckForWinnerHelper(x+1, y+1, color, numSame+1, UP_RIGHT);
			}
			return false;
		break;
		case RIGHT:
			if (x < NUM_COLUMNS-1){
				ChipColor right = gameboard.column[x+1].contents[y];
				if (right == color)
					return CheckForWinnerHelper(x+1, y, color, numSame+1, RIGHT);
			}
			return false;
		break;
		case DOWN_RIGHT:
			if (x < NUM_COLUMNS-1 && y > 0){
				ChipColor downRight = gameboard.column[x+1].contents[y-1];
				if (downRight == color)
					return CheckForWinnerHelper(x+1, y-1, color, numSame+1, DOWN_RIGHT);
			}
			return false;
		break;
		case DOWN:
			if (y > 0){
				ChipColor down = gameboard.column[x].contents[y-1];
				if (down == color)
					return CheckForWinnerHelper(x, y-1, color, numSame+1, DOWN);
			}
			return false;
		break;
		case DOWN_LEFT:
			if (x > 0 && y > 0){
				ChipColor downLeft = gameboard.column[x-1].contents[y-1];
				if (downLeft == color)
					return CheckForWinnerHelper(x-1, y-1, color, numSame+1, DOWN_LEFT);
			}
			return false;
		break;
		case LEFT:
			if (x > 0){
				ChipColor left = gameboard.column[x-1].contents[y];
				if (left == color)
					return CheckForWinnerHelper(x-1, y, color, numSame+1, LEFT);
			}
			return false;
		break;
		default:
			LCD_Clear(0, LCD_COLOR_RED);
			HAL_Delay(3000);
			return false;
	}
}

bool CheckForWinner(uint8_t x, uint8_t y, ChipColor color){
	if (x > 0 && y < NUM_ROWS-1){
		if (CheckForWinnerHelper(x-1, y+1, color, 1, UP_LEFT)) return true;
	}
	if (y < NUM_ROWS-1){
		if (CheckForWinnerHelper(x, y+1, color, 1, UP)) return true;
	}
	if (x < NUM_COLUMNS-1 && y < NUM_ROWS-1){
		if (CheckForWinnerHelper(x+1, y+1, color, 1, UP_RIGHT)) return true;
	}
	if (x < NUM_COLUMNS-1){
		if (CheckForWinnerHelper(x+1, y, color, 1, RIGHT)) return true;
	}
	if (x < NUM_COLUMNS-1 && y > 0){
		if (CheckForWinnerHelper(x+1, y-1, color, 1, DOWN_RIGHT)) return true;
	}
	if (y > 0){
		if (CheckForWinnerHelper(x, y-1, color, 1, DOWN)) return true;
	}
	if (x > 0 && y > 0){
		if (CheckForWinnerHelper(x-1, y-1, color, 1, DOWN_LEFT)) return true;
	}
	if (x > 0){
		if (CheckForWinnerHelper(x-1, y, color, 1, LEFT)) return true;
	}
	return false;
}

void PlayTwoPlayer(){
	for (int i=0; i<NUM_COLUMNS; i++){
		gameboard.column[i].currentHeight = 0;
		for (int j=0; j<6; j++){
			gameboard.column[i].contents[j] = EMPTY;
		}
	}
	gameboard.whosTurn = YELLOW;

	bool gameOver = false;
	uint8_t previousColumn = currentColumn;

	LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_YELLOW);

	while (!gameOver){
		uint16_t playerColor = SWITCH_COLOR(gameboard.whosTurn);
		if (switchColumn(&StaticTouchData, &currentColumn)){
			LCD_Draw_Circle_Fill(CHIP_HSPACE*previousColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_BLUE);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);
			previousColumn = currentColumn;
		}
		uint32_t eventsToRun = getScheduledEvents();
		if (eventsToRun & DROP_CHIP){
			removeSchedulerEvent(DROP_CHIP);
			dropChip(&gameboard, currentColumn, playerColor);
			playerColor = SWITCH_COLOR(gameboard.whosTurn);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);

			uint8_t x = currentColumn-1;
			uint8_t y = gameboard.column[currentColumn-1].currentHeight-1;
			gameboard.column[x].contents[y] = gameboard.whosTurn;
			gameOver = CheckForWinner(x, y, gameboard.whosTurn);
		}
		HAL_Delay(100);
	}
	winner = gameboard.whosTurn;
}

void EXTI0_IRQHandler(){
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	if (gameboard.column[currentColumn-1].currentHeight < NUM_ROWS)
		addSchedulerEvent(DROP_CHIP);
	__HAL_GPIO_EXTI_CLEAR_FLAG(BTN_PIN);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

