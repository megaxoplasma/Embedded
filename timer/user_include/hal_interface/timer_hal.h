#ifndef TIMER_HAL_H_
#define TIMER_HAL_H_	1

#include "hal_common_includes.h"
#include "stm32f3xx_hal_tim.h"


#define TIMER_PERIOD_1KHZ	6545

typedef enum {
	timer2_flag
} flag;

//void init_timer(TIM_HandleTypeDef* timer2_handle);
//int timer_reset();

#endif