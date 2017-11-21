#include "ultra_hal.h"
#include "cpu.h"
#include "board_led.h"

static volatile int echo_flag_rise; 	// Holds whether or not a RISING edge has been detected
											// @wWarning: Access shared between EXTI0 ISR and user code!!
static volatile int echo_flag_fall;
											
static void ultra_gpio_init(void)
{
	static GPIO_InitTypeDef  GPIO_InitStruct;
	/*
		PA0 Init
		Detect both edges, and route to the EXTI module so we can raise an interrupt accordingly
	 */

	GPIO_InitStruct.Pin = GPIO_PIN_0; //pin 0 
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; //set as rising falling
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; //pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOA_CLK_ENABLE(); // Enable clock to GPIOA so PA0 works

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // Done
}

/*
	Performs EXTI initialization of PA0 <-> EXTI0
	Also initializes the button_pressed_flag to 0
 */

void ultra_init(void)
{
	echo_flag_fall = 0;
	echo_flag_rise = 0;
	ultra_gpio_init();	// set up the PA0 GPIO pin so it can connect to the edge detection subsystem

	HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 4); // Set the priority according to the application needs
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); // Enable (un-mask) the EXTI0 interrupt in the NVIC
	
	
}

 int ultra_change() 
{
	int ret;
	if (echo_flag_rise)
	{
		ret = 1; 	
		echo_flag_rise = 0; //drop the flag
		//board_led_toggle(LED2);	
			
	}
	else if(echo_flag_fall) 
	{
		ret = 0; 	
		echo_flag_fall = 0; //drop the flag 
		//board_led_toggle(LED1);
	}
	
	return ret; 
}
 
 
/*
	EXTI IRQ Handlers. These call the generic EXTI IRQ handler from the Cube
	HAL, which in turn call the user-defined generic EXTI Callback function 
	defined further below:
 */

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); 	// This function will first clear the IRQ on EXTI0
											// It will then call the HAL_GPIO_EXTI_Callback below
}

/*
	User code section of the EXTI IRQ, called from HAL_GPIO_EXTI_IRQHandler
	after interrupt flags are cleared:
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0) // This means we're sure we've gotten an EXTI event on pin 0
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) // If the pin is high now, it must've been a
																// RISING edge we captured
																// I.e. the button was PRESSED
		{
			echo_flag_rise = 1; // Set the flag since we've detected a echo was high  event
			board_led_toggle(LED2);
		}
		else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET){
			
			echo_flag_fall = 1;									// If the pin is low now, it must've been a
			board_led_toggle(LED1);													// falling edge we captured
																// I.e. the button was released
		}
			
	}
}