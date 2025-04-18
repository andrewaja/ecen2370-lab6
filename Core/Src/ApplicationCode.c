/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: drew
 */

#include "ApplicationCode.h"

static STMPE811_TouchData StaticTouchData;

static Gameboard gameboard;
static uint8_t currentColumn = 1;
static ChipColor winner;

extern void initialise_monitor_handles(void); 

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
		HAL_Delay(500);
		uint8_t numPlayers = selectMode(&StaticTouchData);
		DisplayGame();
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
		returnToMenu(&StaticTouchData);
	}
}

void DisplayTitle(void){
	LCD_Clear(0,LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(55,LCD_PIXEL_HEIGHT/2-10,'C');
	LCD_DisplayChar(70,LCD_PIXEL_HEIGHT/2-10,'o');
	LCD_DisplayChar(85,LCD_PIXEL_HEIGHT/2-10,'n');
	LCD_DisplayChar(100,LCD_PIXEL_HEIGHT/2-10,'n');
	LCD_DisplayChar(115,LCD_PIXEL_HEIGHT/2-10,'e');
	LCD_DisplayChar(130,LCD_PIXEL_HEIGHT/2-10,'c');
	LCD_DisplayChar(140,LCD_PIXEL_HEIGHT/2-10,'t');
	LCD_DisplayChar(165,LCD_PIXEL_HEIGHT/2-10,'4');
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
	if (winner == YELLOW){
		LCD_DisplayChar(46,30,'W');
		LCD_DisplayChar(56,30,'i');
		LCD_DisplayChar(66,30,'n');
		LCD_DisplayChar(81,30,'n');
		LCD_DisplayChar(96,30,'e');
		LCD_DisplayChar(111,30,'r');
		LCD_DisplayChar(116,30,':');

		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_DisplayChar(136,30,'Y');
		LCD_DisplayChar(148,30,'e');
		LCD_DisplayChar(158,30,'l');
		LCD_DisplayChar(163,30,'l');
		LCD_DisplayChar(170,30,'o');
		LCD_DisplayChar(185,30,'w');

	} else if (winner == RED){
		LCD_DisplayChar(50,30,'W');
		LCD_DisplayChar(65,30,'i');
		LCD_DisplayChar(75,30,'n');
		LCD_DisplayChar(90,30,'n');
		LCD_DisplayChar(105,30,'e');
		LCD_DisplayChar(120,30,'r');
		LCD_DisplayChar(125,30,':');

		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DisplayChar(145,30,'R');
		LCD_DisplayChar(160,30,'e');
		LCD_DisplayChar(175,30,'d');
	}

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font12x12);
	for(y = 60; y<100; y++){
		for(x=15; x < LCD_PIXEL_WIDTH-15; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_WHITE);
		}
	}
	LCD_DisplayChar(88,75,'M');
	LCD_DisplayChar(98,75,'a');
	LCD_DisplayChar(105,75,'i');
	LCD_DisplayChar(110,75,'n');
	LCD_DisplayChar(127,75,'M');
	LCD_DisplayChar(137,75,'e');
	LCD_DisplayChar(145,75,'n');
	LCD_DisplayChar(152,75,'u');
}

void PlaySinglePlayer(){
	// TODO
	HAL_Delay(2000);
}

bool CheckForWinner(uint8_t x, uint8_t y, ChipColor color){
	uint8_t connect = 1;
	int8_t dir[4][2] = {{-1, 1},  // UP_LEFT
						{ 1, 0},  // UP
						{ 1, 1},  // UP_RIGHT
						{ 0, 1}}; // RIGHT

	for (int i=0; i<4; i++){
		int j = 1;
		while (1){ // check positive dir
			int adjx = x + dir[i][0] * j; int adjy = y + dir[i][1] * j;
			if (adjx < 0 || adjx >= NUM_COLUMNS || adjy < 0 || adjy >= NUM_ROWS) break;
			if (gameboard.column[adjx].contents[adjy] != color) break;
			connect++; j++;
		}

		j = 1;
		while (1){ // check negative dir
			int adjx = x - dir[i][0] * j; int adjy = y - dir[i][1] * j;
			if (adjx < 0 || adjx >= NUM_COLUMNS || adjy < 0 || adjy >= NUM_ROWS) break;
			if (gameboard.column[adjx].contents[adjy] != color) break;
			connect++; j++;
		}
	}

	if (connect >= 4) return true;
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

	uint8_t previousColumn = currentColumn;
	LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_YELLOW);

	while (1){
		uint16_t playerColor = LCD_COLOR(gameboard.whosTurn);
		if (switchColumn(&StaticTouchData, &currentColumn)){
			LCD_Draw_Circle_Fill(CHIP_HSPACE*previousColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_BLUE);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);
			previousColumn = currentColumn;
		}
		uint32_t eventsToRun = getScheduledEvents();
		if (eventsToRun & DROP_CHIP){
			removeSchedulerEvent(DROP_CHIP);
			dropChip(&gameboard, currentColumn, playerColor);

			uint8_t x = currentColumn-1;
			uint8_t y = gameboard.column[currentColumn-1].currentHeight-1;
			gameboard.column[x].contents[y] = gameboard.whosTurn;
			if (CheckForWinner(x, y, gameboard.whosTurn)) break;

			gameboard.whosTurn = SWITCH_COLOR(gameboard.whosTurn);
			playerColor = LCD_COLOR(gameboard.whosTurn);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);
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

