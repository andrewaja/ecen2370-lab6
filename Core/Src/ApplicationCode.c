/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: drew
 */

#include "ApplicationCode.h"

static STMPE811_TouchData StaticTouchData;

static uint32_t numTimerOverflow = 0;

static Gameboard gameboard;
static uint8_t currentColumn = 1;
static ChipColor winner;

extern void initialise_monitor_handles(void); 

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality

	ButtonInit();
	Tim2Init();

	LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);
	InitializeLCDTouch();
	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;
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

void DisplayEnd(uint32_t time){
	uint16_t x, y;
	for(y = 0; y<50; y++){
		for(x=0; x < LCD_PIXEL_WIDTH; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_BLUE);
		}
	}

	// display time
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	uint32_t minutes = time / 60.0;
	uint32_t seconds = time % 60;

	char min[9]; char sec[9];
	sprintf(min, "%02lu", minutes);
	sprintf(sec, "%02lu", seconds);

	LCD_DisplayChar(93,15,min[0]);
	LCD_DisplayChar(105,15,min[1]);
	LCD_DisplayChar(115,15,':');
	LCD_DisplayChar(125,15,sec[0]);
	LCD_DisplayChar(137,15,sec[1]);

	// display winner
	if (winner == YELLOW){
		LCD_DisplayChar(46,40,'W');
		LCD_DisplayChar(56,40,'i');
		LCD_DisplayChar(66,40,'n');
		LCD_DisplayChar(81,40,'n');
		LCD_DisplayChar(96,40,'e');
		LCD_DisplayChar(110,40,'r');
		LCD_DisplayChar(115,40,':');

		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_DisplayChar(136,40,'Y');
		LCD_DisplayChar(148,40,'e');
		LCD_DisplayChar(158,40,'l');
		LCD_DisplayChar(163,40,'l');
		LCD_DisplayChar(170,40,'o');
		LCD_DisplayChar(185,40,'w');

	} else if (winner == RED){
		LCD_DisplayChar(50,40,'W');
		LCD_DisplayChar(65,40,'i');
		LCD_DisplayChar(75,40,'n');
		LCD_DisplayChar(90,40,'n');
		LCD_DisplayChar(105,40,'e');
		LCD_DisplayChar(120,40,'r');
		LCD_DisplayChar(125,40,':');

		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_DisplayChar(145,40,'R');
		LCD_DisplayChar(160,40,'e');
		LCD_DisplayChar(175,40,'d');

	} else {
		LCD_DisplayChar(110,40,'T');
		LCD_DisplayChar(120,40,'i');
		LCD_DisplayChar(130,40,'e');
	}

	// main menu button
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font12x12);
	for(y = 70; y<110; y++){
		for(x=15; x < LCD_PIXEL_WIDTH-15; x++){
			LCD_Draw_Pixel(x, y, LCD_COLOR_WHITE);
		}
	}
	LCD_DisplayChar(90,85,'M');
	LCD_DisplayChar(100,85,'a');
	LCD_DisplayChar(107,85,'i');
	LCD_DisplayChar(112,85,'n');
	LCD_DisplayChar(125,85,'M');
	LCD_DisplayChar(135,85,'e');
	LCD_DisplayChar(143,85,'n');
	LCD_DisplayChar(150,85,'u');
}

uint8_t CheckForWinner(uint8_t x, uint8_t y, ChipColor color){
	// direction lookup table
	int8_t dir[4][2] = {{-1, 1},  // UP_LEFT
						{ 1, 0},  // UP
						{ 1, 1},  // UP_RIGHT
						{ 0, 1}}; // RIGHT

	// check for win
	for (uint8_t i=0; i<4; i++){
		uint8_t connect = 1; // number of consecutive chips

		uint8_t j = 1;
		while (1){ // check positive dir
			uint8_t adjx = x + dir[i][0] * j; uint8_t adjy = y + dir[i][1] * j; // get new indices
			if (adjx < 0 || adjx >= NUM_COLUMNS || adjy < 0 || adjy >= NUM_ROWS) break;
			if (gameboard.column[adjx].contents[adjy] != color) break;
			connect++; j++;
		}
		j = 1;
		while (1){ // check negative dir
			uint8_t adjx = x - dir[i][0] * j; uint8_t adjy = y - dir[i][1] * j; // get new indices
			if (adjx < 0 || adjx >= NUM_COLUMNS || adjy < 0 || adjy >= NUM_ROWS) break;
			if (gameboard.column[adjx].contents[adjy] != color) break;
			connect++; j++;
		}
		if (connect >= 4) return 1;
	}

	// check for tie
	bool tie = true;
	for (int i=0; i<NUM_COLUMNS; i++){
		if (gameboard.column[i].currentHeight < FULL){
			tie = false;
			break;
		}
	}
	if (tie) return 2;

	return 0;
}

uint8_t GetNextMove(){
    for (uint8_t col = 0; col < NUM_COLUMNS; col++) {
    	if (gameboard.column[col].currentHeight == FULL) continue;

    	// place chip in gameboard (no visual)
		uint8_t row = gameboard.column[col].currentHeight;
		gameboard.column[col].currentHeight++;

		// check for AI win
		gameboard.column[col].contents[row] = AI_COLOR;
		bool wins = CheckForWinner(col, row, AI_COLOR);

		// check for block player win
		gameboard.column[col].contents[row] = SWITCH_COLOR(AI_COLOR);
		bool blocks = CheckForWinner(col, row, SWITCH_COLOR(AI_COLOR));

		// remove chip from gameboard
		gameboard.column[col].currentHeight--;
		gameboard.column[col].contents[row] = EMPTY;

		if (wins)
			return (col+1);
		if (blocks)
			return (col+1);
    }

    // prioritize middle columns
    for (int i=0; i<4; i++){
    	uint8_t rightCol = 4+i, leftCol = 4-i;
    	if (gameboard.column[rightCol-1].currentHeight < FULL)
    		return rightCol;
    	if (gameboard.column[rightCol-1].currentHeight < FULL)
    		return leftCol;
    }
    return 1; // should never reach here but gets rid of warning
}

uint32_t PlayGame(uint8_t numPlayers){
	// initialize
	for (int i=0; i<NUM_COLUMNS; i++){
		gameboard.column[i].currentHeight = 0;
		for (int j=0; j<NUM_ROWS; j++){
			gameboard.column[i].contents[j] = EMPTY;
		}
	}
	gameboard.whosTurn = YELLOW;
	uint8_t previousColumn = currentColumn;
	LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_YELLOW);

	// reset timer
	Tim2Reset();
	Tim2Start();
	numTimerOverflow = 0;

	while (1){
		uint16_t playerColor = LCD_COLOR(gameboard.whosTurn);

		// poll for column selection
		previousColumn = currentColumn;
		if (numPlayers == 2 || gameboard.whosTurn == YELLOW){
			if (switchColumn(&StaticTouchData, &currentColumn)){
				LCD_Draw_Circle_Fill(CHIP_HSPACE*previousColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_BLUE);
				LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);
			}
		} else { // AI's turn
			currentColumn = GetNextMove();

			HAL_Delay(400);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*previousColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR_BLUE);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, LCD_COLOR(AI_COLOR));
			HAL_Delay(400);

			addSchedulerEvent(DROP_CHIP);
		}

		// check for button press
		uint32_t eventsToRun = getScheduledEvents();
		if (eventsToRun & DROP_CHIP){
			removeSchedulerEvent(DROP_CHIP);
			dropChip(&gameboard, currentColumn, playerColor);

			// check for 4 in a row
			uint8_t x = currentColumn-1;
			uint8_t y = gameboard.column[currentColumn-1].currentHeight-1;
			gameboard.column[x].contents[y] = gameboard.whosTurn;
			if (CheckForWinner(x, y, gameboard.whosTurn) == 1){
				winner = gameboard.whosTurn;
				break;
			} else if (CheckForWinner(x, y, gameboard.whosTurn) == 2){
				winner = EMPTY;
				break;
			}

			// switch player
			gameboard.whosTurn = SWITCH_COLOR(gameboard.whosTurn);
			playerColor = LCD_COLOR(gameboard.whosTurn);
			LCD_Draw_Circle_Fill(CHIP_HSPACE*currentColumn, CHIP_VSPACE, CHIP_RADIUS, playerColor);
		}
		HAL_Delay(200);
	}
	Tim2Stop();
	return GetTimeInSeconds(numTimerOverflow);
}

void StartGame(void)
{
	DisplayTitle();
	HAL_Delay(2000);
	while(1){
		DisplayMenu();
		HAL_Delay(200);
		uint8_t numPlayers = selectMode(&StaticTouchData);
		DisplayGame();
		if (numPlayers == 1 || numPlayers == 2){
			uint32_t time = PlayGame(numPlayers);
			DisplayEnd(time);
			returnToMenu(&StaticTouchData);
		} else LCD_Error_Handler();
	}
}

void EXTI0_IRQHandler(){
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);

	if (gameboard.column[currentColumn-1].currentHeight < NUM_ROWS)
		addSchedulerEvent(DROP_CHIP);

	__HAL_GPIO_EXTI_CLEAR_FLAG(BTN_PIN);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void TIM2_IRQHandler(){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);

	numTimerOverflow++;

	Tim2ClearSR();
	HAL_NVIC_ClearPendingIRQ(TIM2_IRQn);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

