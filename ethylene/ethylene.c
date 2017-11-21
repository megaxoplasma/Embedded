/*
	This is the main user software application
	In this case, we have below the code necessary to toggle a set of LEDs on a	STM32 F3 Discovery PCB at 20 Hz.

	(c) Abhimanyu Ghosh, 2017
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
#include <stdio.h>

#define ADEN 0x00000001
#define VREFINT_CAL_ADDR    0x1FFFF7BA

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



static ADC_ChannelConfTypeDef ADC1_chan;   //PA0 is adc 1 channle input 1
ADC1_chan.Channel = ADC_CHANNEL_1;
ADC1_chan.Rank = ADC_REGULAR_RANK_1;
ADC1_chan.SamplingTime = ADC_SAMPLETIME_2CYCLE_5;
ADC1_chan.Offset = 0;





static ADC_InitTypeDef hADC1_init;
hADC1_init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV1;
hADC1_init.Resolution = ADC_RESOLUTION_12B;
hADC1_init.DataAlign = ADC_DATAALIGN_RIGHT;
hADC1_init.ContinuousConvMode = ENABLE;




static ADC_HandleTypeDef hADC1;
hADC1.Instance = ADC1;
hADC1.Init = hADC1_init;

__HAL_RCC_ADC1_CLK_ENABLE();
HAL_ADC_Init(&hADC1);
HAL_ADC_ConfigChannel(&hADC1, &ADC1_chan);


 ADC1->CR &= ~ADC_CR_ADVREGEN;
 ADC1->CR |= ADC_CR_ADVREGEN_0; // 01: ADC Voltage regulator enabled

 cpu_sw_delay(100U);

 ADC1->CR &= ~ADC_CR_ADCALDIF;// calibration in Single-ended inputs Mode.
 ADC1->CR |= ADC_CR_ADCAL; // Start ADC calibration
 // Read at 1 means that a calibration in progress.
 while(ADC1->CR & ADC_CR_ADCAL); //wait until calibration done 



uint32_t cal = ADC1->CALFACT;
uint32_t v_cal;                        // VREFINT calibration value
v_cal= *((uint16_t*)VREFINT_CAL_ADDR); // read VREFINT_CAL_ADDR memory location




ADC1->CR |= ADC_CR_ADEN; // Enable ADC1
while(!ADC1->ISR & ADC_ISR_ADRD); // wait for ADRDY 


 

uint32_t value;
HAL_ADC_Start(&hADC1);
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
