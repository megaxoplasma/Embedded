#include "board_led.h"

void board_led_init(void)
{
	static GPIO_InitTypeDef  GPIO_InitStruct;
	static GPIO_InitTypeDef  GPIO_InitStruct_2;
	
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	GPIO_InitStruct_2.Pin = GPIO_PIN_9;
	GPIO_InitStruct_2.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct_2.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct_2.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_2);
}

void board_led_on(led led_id)
{
	switch(led_id)
	{
		case LED1:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
			break;
		case LED2:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
			break;
	}
}

void board_led_off(led led_id)
{
	switch(led_id)
	{
		case LED1:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
			break;
		case LED2:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
	}
}

void board_led_toggle(led led_id)
{
	switch(led_id)
	{
		case LED1:
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
			break;
		case LED2:
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
			break;
	}
}