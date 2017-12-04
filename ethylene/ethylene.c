/*
	This is the main user software application	
 */

#include "cpu.h"
#include "board_led.h"
#include "foo.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "foo.h"
#include "stm32f3xx_hal_tim.h"
#include "uart.h"
#include "foo.h"
#include "adc.h"
#include <stdio.h>
#include <math.h>

#define ADEN 0x00000001
//#define VREFINT_CAL_ADDR    0x1FFFF7BA

int main()
{
  /*
    Initialize the PLL, clock tree to all peripherals, flash and Systick 1 ms time reference:
   */
  cpu_init();

   uart_debug_init(); //initialize PA9 and PA10, 9 is R ,10 is T
 
  /*
    Initialize the GPIO (General-Purpose I/O) subsystem pins that are connected to the LEDs on the board:
   */
  //initiallize PA0 for ADC

    static GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin |= GPIO_PIN_0 ;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//set up led
	static GPIO_InitTypeDef  GPIO_InitStruct_E;

	GPIO_InitStruct_E.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct_E.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct_E.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct_E.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOE_CLK_ENABLE();

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_E);

 

  int i = 0;

  setvbuf(stdin,NULL,_IONBF,0);   // Sets stdin in unbuffered mode (normal for usart com)
  setvbuf(stdout,NULL,_IONBF,0);  // Sets stdin in unbuffered mode (normal for usart com)
  
  printf("YAY\r\n");

//ADC_Common_TypeDef* ADC12_com;
//ADC12_com->CCR |= ADC12_CCR_VREFEN; //Enable V reference internal

ADC_HandleTypeDef hADC1 = ADC_setup();

uint32_t value;

  while(1)
  {
    /*
      Carry out a simple unit test of foo() declared in foo.h:
     */
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

    	

    	//start conversion
 	  if( HAL_ADC_PollForConversion(&hADC1,10000)  == HAL_OK )
{
	   value = HAL_ADC_GetValue(&hADC1);
}
	//float vdd = 3.3 * v_cal / value;

	float voltage;
	voltage = 1500.0 * (float)value  / 4095.0  ;
	//float Vdd = 3300.0 * (ref) / value;



   //  uint32_t temperatureC = (voltage - 0.5) * 100 ;
      cpu_sw_delay(50U);  // Invoke a simple software busy-wait routine to delay approximately 50 milliseconds
      //save conversion

  
HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
   
    
      cpu_sw_delay(100U);
      //print conversion
      voltage *= 100;
      voltage = roundf(voltage);
      voltage /= 100;
 
     printf("%f millivolts \r\n",voltage);
      ++i; // Increment i for the next test iteration...
    }
  }

  return 0;
}
