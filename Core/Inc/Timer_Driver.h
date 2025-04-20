/*
 * Timer_Driver.h
 *
 *  Created on: Apr 19, 2025
 *      Author: mrflu
 */

#ifndef TIMER_DRIVER_H_
#define TIMER_DRIVER_H_

#include "stm32f4xx_hal.h"

#define AUTORELOAD 0x03D08FFF
#define PRESCALER  0x0
#define PERIOD 	   4   // seconds

void Tim2Init();
void Tim2Start();
void Tim2Stop();
void Tim2Reset();

uint32_t GetTim2Count();
uint32_t GetTim2AutoReload();
void SetTim2Count(uint32_t count);
void SetTim2AutoReload(uint32_t val);

void Tim2ClearSR();

uint32_t GetTimeInSeconds(uint32_t numTimerOverflow);

#endif /* TIMER_DRIVER_H_ */
