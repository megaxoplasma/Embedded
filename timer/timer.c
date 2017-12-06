/*
	This is the main user software application
(c) Ying Zhu
 */

#include "cpu.h"
#include "board_led.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "foo.h"
#include "stm32f3xx_hal_tim.h"

#define UIE 0x00000001 // Update interrupt enable
#define CEN 0x00000001 // Counter enable
#define UIF 0x00000001 // Update interrupt flag
#define RCC_APB1ENR_TIM7EN 0x00000020

int counter;

voidInitTimer7(void){
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Enable clock for TIM7   
TIM7->CR1 &= ~CEN; // Disable TIM7 interrupt 
TIM7->PSC = 71;
TIM7->ARR = 1000;

NVIC_EnableIRQ(TIM7_IRQn);

TIM7->DIER |= UIE; // Enable TIM7 interrupt
TIM7->CR1 |= CEN; // TIM7 enable
}

void TIM7_IRQHandler (void) {
TIM7->SR &= ~UIF; // Clear UIF
counter ++;

}


TIM_HandleTypeDef timer2_handle;

static void Error_Handler(void)
{
	while(1);
}

void TIM2_IRQHandler(TIM_HandleTypeDef *htim)
{

  HAL_TIM_IRQHandler(&timer2_handle);

}
/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//if(htim->Instance == TIM2 ){
	//HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_9);
	//}
}
*/
int main()
{	int timer2_flag;
  
  cpu_init();
  board_led_init();

  voidInitTimer7();
    
   
  //HAL_TIM_Base_Start_IT(&timer2_handle);
  int i = 0;

  while(1)
  {
    if(TEST_FOO(i, i+1) < 0)
    {
      /*
        If the above fails there is either a hardware, code or other undefined error.
        Now we're in an undefined state regarding processor core behavior...
       */
      while(1); // We probably have had a radiation hit or a major malfunction on the ALU of the processor...
    }
    else
    {
		if(counter == 1000 ) {
		HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_9);
		counter = 0;
		}
		++i; // Increment i for the next test iteration...
    }
  }
  return 0;
}

/*
void init_timer(void)
{
   TIM_ClockConfigTypeDef timer2_clk;
   TIM_MasterConfigTypeDef sMasterConfig;
	//-------------------------------------------------
	
	timer2_handle.Instance = TIM2;
	timer2_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	
	timer2_handle.Init.Prescaler = 48000;
	timer2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer2_handle.Init.Period = 499;
	timer2_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer2_handle.Init.AutoReloadPreload = 0;

  // RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
   	
	timer2_clk.ClockSource = TIM_CLOCKSOURCE_INTERNAL;  
	timer2_clk.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  
   TIM2->CR1 |= TIM_CR1_CEN; // enable timer 2  
   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  //HAL_TIM_Base_Init(&timer2_handle);
  //HAL_TIM_ConfigClockSource(&timer2_handle, &timer2_clk);
  //HAL_TIMEx_MasterConfigSynchronization(&timer2_handle, &sMasterConfig);
 if (HAL_TIM_Base_Init(&timer2_handle) != HAL_OK)
  {
    Error_Handler();
  }

  timer2_clk.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&timer2_handle, &timer2_clk) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&timer2_handle, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_NVIC_SetPriority(TIM2_IRQn, 2, 2); // Set the priority according to the application needs
	HAL_NVIC_EnableIRQ(TIM2_IRQn); // Enable (un-mask) the EXTI0 interrupt in the NVIC
	

}
*/
