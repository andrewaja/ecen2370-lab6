/*
 * Timer_Driver.c
 *
 *  Created on: Apr 19, 2025
 *      Author: mrflu
 */

#include "Timer_Driver.h"

static TIM_HandleTypeDef TC;

void Tim2Init(){
	TC.Instance = TIM2;
	TC.Init.Prescaler         = PRESCALER; // 0x0
	TC.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TC.Init.Period            = AUTORELOAD; // 0x03D08FFF
	TC.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	TC.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	__HAL_RCC_TIM2_CLK_ENABLE();
	HAL_TIM_Base_Init(&TC);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	__HAL_TIM_ENABLE_IT(&TC, TIM_IT_UPDATE);
}

void Tim2Start(){
	__HAL_TIM_ENABLE(&TC);
}

void Tim2Stop(){
	__HAL_TIM_DISABLE(&TC);
}

void Tim2Reset(){
	__HAL_TIM_SET_COUNTER(&TC, RESET);
}

uint32_t GetTim2Count(){
	return __HAL_TIM_GET_COUNTER(&TC);
}

uint32_t GetTim2AutoReload(){
	return __HAL_TIM_GET_AUTORELOAD(&TC);
}

void SetTim2Count(uint32_t count){
	__HAL_TIM_SET_COUNTER(&TC, count);
}

void SetTim2AutoReload(uint32_t val){
	__HAL_TIM_SET_AUTORELOAD(&TC, val);
}

void Tim2ClearSR(){
	__HAL_TIM_CLEAR_IT(&TC, TIM_SR_UIF);
}

uint32_t GetTimeInSeconds(uint32_t numTimerOverflow) {
    uint32_t timer_clock = HAL_RCC_GetPCLK1Freq()*2;
    uint32_t time = (GetTim2Count()+1) / timer_clock;
    time += ((numTimerOverflow*(AUTORELOAD+1)) / timer_clock);
    return time;
}
