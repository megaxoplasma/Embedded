#include "cpu.h"
#include "board_led.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "foo.h"
#include "stm32f3xx_hal_tim.h"
#include "ultra_sonic.h"
#include "uart.h"
#include "foo.h"
#include "pwm_hal.h"
#include <stdio.h>

#define UIE 0x00000001 // Update interrupt enable
#define CEN 0x00000001 // Counter enable
#define UIF 0x00000001 // Update interrupt flag
#define RCC_APB1ENR_TIM7EN 0x00000020


void ahead(float d1, float d2);
void back(float d1, float d2);
void turn_around_clk(float d1, float d2);
void turn_around_cclk(float d1, float d2);
void stop_run();
int change_fuck(int fuck);

float speedl=0.35f;
float speedr=0.45f;
float speedaheadl=0.35f;
float speedaheadr=0.45f;


int counter_trigger; 
int counter_scale;
int counter; //in units of microseconds, (counter) amouont of microseconds has passed 
int T1; //takes in the time in units of us for rising edge event, for sensor 2
int T2; //takes in the time in units of us when falling edge occured for sensor 2

//sensor 1 stuff
int T11;
int T12;

//sensor 0 stuff
int T01;
int T02;

int wall_front=0;


float distance;
float distance2;
float distance3;

TIM_HandleTypeDef timer2_handle;

static volatile int echo_flag_rise; 	
static volatile int echo_flag_fall;

//sensor 1
static volatile int echo_flag_rise1; 	
static volatile int echo_flag_fall1;

//sensor 0
static volatile int echo_flag_rise0; 	
static volatile int echo_flag_fall0;
/////////////////////////////////////////////////////////////////////////TIMER
voidInitTimer7(void){
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Enable clock for TIM7   
TIM7->CR1 &= ~CEN; // Disable TIM7 interrupt 
TIM7->PSC = 71; // not 72MHZ, only 1MHZ now, tick each 1us
TIM7->ARR = 1;

NVIC_EnableIRQ(TIM7_IRQn);

TIM7->DIER |= UIE; // Enable TIM7 interrupt
TIM7->CR1 |= CEN; // TIM7 enable
}

void TIM7_IRQHandler (void) {
TIM7->SR &= ~UIF; // Clear UIF
counter++; //increment each microsecond
counter_trigger++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////           TIMER								
static void ultra_gpio_init(void)
{
	static GPIO_InitTypeDef  GPIO_InitStruct_F;
	/*
		PF9 Init
		Detect both edges, and route to the EXTI module so we can raise an interrupt accordingly
	 */

	GPIO_InitStruct_F.Pin = GPIO_PIN_9 | GPIO_PIN_6 ; //pin 9 and 6
	GPIO_InitStruct_F.Mode = GPIO_MODE_IT_RISING_FALLING; //set as rising falling
	GPIO_InitStruct_F.Pull = GPIO_PULLDOWN; //pull down
	GPIO_InitStruct_F.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOF_CLK_ENABLE(); // Enable clock to GPIOF so PF9 works

	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_F); // Done
	
	
	
	
	static GPIO_InitTypeDef  GPIO_InitStruct_B;

	GPIO_InitStruct_B.Pin = GPIO_PIN_0 ; //pin 0
	GPIO_InitStruct_B.Mode = GPIO_MODE_OUTPUT_PP; //out put since its a trigger
	GPIO_InitStruct_B.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct_B.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOB_CLK_ENABLE(); // enable clock for GPIOB


	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_B); // set up done
	
/* SENSOR 2 DONE*///////////////////////////////////////////////////////////////////////////////////

static GPIO_InitTypeDef  GPIO_InitStruct_D;

	GPIO_InitStruct_D.Pin = GPIO_PIN_0; // D 0 is changed
	GPIO_InitStruct_D.Mode = GPIO_MODE_OUTPUT_PP; //trigger
	GPIO_InitStruct_D.Pull = GPIO_PULLDOWN; //pull down
	GPIO_InitStruct_D.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOD_CLK_ENABLE(); 

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct_D); // Done
	
/* SENSOR 1 DONE ////////////////////////////////////////////////////////////////////*/	
	static GPIO_InitTypeDef  GPIO_InitStruct_E1; // echo 0
	
	static GPIO_InitTypeDef  GPIO_InitStruct_E2; // trigger 0
	

//echo 0


	GPIO_InitStruct_E1.Pin = GPIO_PIN_11;  // pin 11 of echo 1
	GPIO_InitStruct_E1.Mode = GPIO_MODE_IT_RISING_FALLING; //set as rising falling
	GPIO_InitStruct_E1.Pull = GPIO_PULLDOWN; //pull down
	GPIO_InitStruct_E1.Speed = GPIO_SPEED_FREQ_LOW;

	__HAL_RCC_GPIOE_CLK_ENABLE(); 

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_E1); // Done
	
	

	GPIO_InitStruct_E2.Pin = GPIO_PIN_10 ; //pin 10
	GPIO_InitStruct_E2.Mode = GPIO_MODE_OUTPUT_PP; //out put since its a trigger
	GPIO_InitStruct_E2.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct_E2.Speed = GPIO_SPEED_FREQ_LOW;



	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct_E2); // set up done
	
}

void ultra_init(void)
{
	echo_flag_fall = 0;
	echo_flag_rise = 0;
	
	echo_flag_fall1 = 0;
	echo_flag_rise1 = 0;
	
	echo_flag_fall0 = 0;
	echo_flag_rise0 = 0;
	ultra_gpio_init();	// set up the PF9 GPIO pin so it can connect to the edge detection subsystem

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 3); // Set the priority according to the application needs
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); // Enable (un-mask) the EXTI9  to 5 interrupt in the NVIC
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 3); // Set the priority according to the application needs
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // Enable (un-mask) the EXTI15  to 10 interrupt in the NVIC
	
	
}

 
void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
											// This function will first clear the IRQ on EXTI9
											// It will then call the HAL_GPIO_EXTI_Callback below
											

}

void EXTI15_10_IRQHandler(void)
{
	
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11); 	// This function will first clear the IRQ on EXTI9
											// It will then call the HAL_GPIO_EXTI_Callback below
											

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //parameter is GPIO that threw the interrupt
{    
	if(GPIO_Pin == GPIO_PIN_9) // This means we're sure we've gotten an EXTI event on pin 9
	{	
		if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9) == GPIO_PIN_SET) // If the pin is high now, it must've been a
																// RISING edge we captured															// I.e. the button was PRESSED
		{
			echo_flag_rise = 1; // Set the flag since we've detected a echo was high  event
			T1 = counter;
		}
		else if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9) == GPIO_PIN_RESET){
			echo_flag_fall = 1;									// If the pin is low now, it must've been a
			T2 = counter;										// falling edge we captured
													// I.e. the button was released
		}
		else{}
	}
	//this is for sensor 1
	
	 else if(GPIO_Pin == GPIO_PIN_6) // This means we're sure we've gotten an EXTI event on pin 6
	{
			
		if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET) // If the pin is high now, it must've been a
																// RISING edge we captured															// I.e. the button was PRESSED
		{
			echo_flag_rise1 = 1; // Set the flag since we've detected a echo was high  event
			T11 = counter;
		}
		else if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_RESET){
			echo_flag_fall1 = 1;									// If the pin is low now, it must've been a
			T12 = counter;										// falling edge we captured
															// I.e. the button was released
		}
		else{}
	}
	//this is for sensor 0
	else if(GPIO_Pin == GPIO_PIN_11) // This means we're sure we've gotten an EXTI event on pin 11
	{
			
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET) // If the pin is high now, it must've been a
																// RISING edge we captured															// I.e. the button was PRESSED
		{
			echo_flag_rise0 = 1; // Set the flag since we've detected a echo was high  event
			T01 = counter;
		}
		else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_RESET){
			echo_flag_fall0 = 1;									// If the pin is low now, it must've been a
			T02 = counter;										// falling edge we captured
															// I.e. the button was released
		}
		else{}
	}
	
	
}



static void Error_Handler(void)
{
	while(1);
}

void TIM2_IRQHandler(TIM_HandleTypeDef *htim)
{

  HAL_TIM_IRQHandler(&timer2_handle);

}

int main()
{	
  T2 = 1;
  T1 = 2;
  
  T12 = 1;
  T11 = 2;
  
  T02 = 1;
  T01 = 2;
int fuck = -1;  
  
  cpu_init();
  board_led_init();
  init_pwm();

  voidInitTimer7();
  ultra_init();
   uart_debug_init();
   
  setvbuf(stdin,NULL,_IONBF,0);   // Sets stdin in unbuffered mode (normal for usart com)
  setvbuf(stdout,NULL,_IONBF,0);  // Sets stdin in unbuffered mode (normal for usart com)
  
  
  printf("YYYYYYYYAAAAAAAYYYYYYY!\r\n");
 
  int i = 0;
 
  float angle = 0.0f;
  float duty1 = 0.0f;
  float duty2 = 0.0f;
  int t=0;     // decide the motions
		
			
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
    else //infinite loop
    {
		
		
		
		
		if (counter_trigger >= 10000){ //trigger  each  half second
			
			
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); //up trigger
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET); //up trig
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET); //up trig
			
			cpu_sw_delay_US(10U);// 10 micro delay
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //down trigger
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET); //down trigger
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET); //down trigger
					
			counter_trigger = 0; //reset counter
		}
		
		
	

		if(T2 > T1){   //left
			
				
				distance = (float)(T2-T1) / (float)58.0f;
				//printf("%f \r\n", (float)distance);	
			}
			else{}
		
		
		
		
		if(T12 > T11){     //front
			
				
				distance2 = (float)(T12-T11) / (float)58.0f;
				//printf("%f \r\n", (float)distance2);
				
					
			}
		else{}

	    if(T02 > T01){    // right one
			
				
				distance3 = (float)(T02-T01) / (float)58.0f;
				//printf("%f \r\n", (float)distance3);
				
					
			}
			else{}

		/*duty1=0.5f;     //left wheel
		duty2=0.5f;    // right wheel
		ahead(duty1,duty2);*/

		/*if(distance3>=15.0f && distance>=15.0f){   // if there is block in the front, shut down the motor
			duty1=0.0f;
			duty2=0.0f;
			ahead(duty1,duty2);
		}*/	//cpu_sw_delay(100U);

		if (distance2 <= 12.0f && distance2 > 4){
							    
			    //cpu_sw_delay(100U);
			 	duty1=0.5f;     //d1=0.5f, d2=0.8f for turn_around_clk
		 		duty2=0.5f;
				turn_around_cclk(duty1,duty2);
				//turnleft();
				cpu_sw_delay(15U);
				stop_run();
				cpu_sw_delay(100U);
				
				printf("turninggg\r\n");
			}
			
			// else if(distance>=15.0f){   // the wall is at right side
			// 		if (distance3<=10.0f){
		 // 				duty1=0.42f;       // duty1=0.43f; duty2=0.58f --- good
			// 			duty2=0.58f;
			// 			ahead(duty1,duty2);
			// 			cpu_sw_delay_US(100U);// 10 micro delay
			// 		 } //  turn left
					 
		 // 			else if (distance3>10.0f && distance3<12.0f){
		 // 				duty1=0.52f;      //duty1=0.52f; duty2=0.5f
			// 			duty2=0.5f;
			// 			ahead(duty1,duty2);
			// 			cpu_sw_delay_US(100U);// 10 micro delay
		 // 				}
		 // 			else{}
		
			// 		}// turn right
		if(distance2>12.0f){
				//printf("no wall in front\r\n");
				/*duty1 = 0.50f;			
			     duty2 = 0.50f;
				ahead(duty1,duty2);
				cpu_sw_delay_US(500U);*/
			    //stop_run();
				//cpu_sw_delay_US(100U);
				  // the wall is at right side
				if(distance>10.0f){
					if (distance3 <=7.0f  ) {
		 				duty1=0.55f;       // duty1=0.43f; duty2=0.58f --- good
						duty2=0.65f;
						ahead(duty1,duty2);
						//printf("turn left\r\n");
					 } //  turn left
					 
		 			/*else if (distance3>10.0f && distance3<12.0f){
		 				duty1=0.52f;      //duty1=0.52f; duty2=0.5f
						duty2=0.5f;
						ahead(duty1,duty2);*/
						//cpu_sw_delay_US(100U);// 10 micro delay
		 				//}
		 			else if(distance3>7.0f && distance3 < 15){
		 				duty1=0.65f;      //duty1=0.52f; duty2=0.5f
						duty2=0.57f;
						ahead(duty1,duty2);
						//printf("turn right\r\n");
					
		 			}
					else if( distance3 > 15){
						duty1=0.5f;     //d1=0.5f, d2=0.8f for turn_around_clk
						duty2=0.5f;
						turn_around_cclk(duty1,duty2);
				//turnleft();
						cpu_sw_delay(15U);
						stop_run();
						cpu_sw_delay(100U);
						
						ahead(duty1,duty2);
				//turnleft();
						cpu_sw_delay(15U);
						stop_run();
						cpu_sw_delay(100U);
						
				
						
					}
		 		
				
		 		}
		 	}	

		 	if(distance2 <= 4  ){

		 		back(0.55,0.55);
		 		cpu_sw_delay(15U);
				stop_run();
				cpu_sw_delay(20U);
				
		 	}
		 			//else{printf("turn right\r\n");}
		 	//}
		 
		/*else if(distance2>15.0f && distance3>15.0f){
				if (distance<=7.0f ) {
		 				duty1=0.6f;       // duty1=0.43f; duty2=0.58f --- good
						duty2=0.55f;
						ahead(duty1,duty2);
						printf("turn right\r\n");
						//cpu_sw_delay_US(100U);// 10 micro delay
		 				//stop_run();
		 				//cpu_sw_delay_US(100U);
					 } //  turn left
					 
		 		
		 			else if(distance>7.0f && duty1 == 0.6f && duty2 == 0.55f ){
		 				duty1=0.35f;      //duty1=0.52f; duty2=0.5f
						duty2=0.85f;
						ahead(duty1,duty2);
						printf("turn left\r\n");
						//cpu_sw_delay_US(100U);
		 				//stop_run();
		 				//cpu_sw_delay_US(100U);
		 			}
		 			else{printf("turn left\r\n");}
		}*/
				/*else if(distance3>=15.0f){   //the wall is at left side
					if (distance>10.0f && distance<12.0f){
						duty1=0.43f;
						duty2=0.64f;
						ahead(duty1,duty2);	
					}
					else if (distance<=10.0f){   // turn right actually duty1>duty2, but the right wheel is not smart
						duty1=0.5f;    
						duty2=0.54f;
						ahead(duty1,duty2);	
					}
				}*/
			//}
	//wall_front=0;
	//switch(wall_front){
	//case 0:  // no wall in front
		/*if(distance>=15.0f){   // the wall is at right side
			if (distance3<=10.0f){
		 		duty1=0.42f;       // duty1=0.43f; duty2=0.58f --- good
				duty2=0.58f;
				ahead(duty1,duty2);
		 } //  turn left
		 	else if (distance3>10.0f && distance3<12.0f){
		 		duty1=0.52f;      //duty1=0.52f; duty2=0.5f
				duty2=0.5f;
				ahead(duty1,duty2);
		 }	
		}// turn right
		
		else if(distance3>=15.0f){   //the wall is at left side
			if (distance>10.0f && distance<12.0f){
				duty1=0.43f;
				duty2=0.64f;
				ahead(duty1,duty2);	
			}
			else if (distance<=10.0f){   // turn right actually duty1>duty2, but the right wheel is not smart
				duty1=0.5f;    
				duty2=0.54f;
				ahead(duty1,duty2);	
			}
		}*/
			
	//case 1:  // a wall in front
		// duty1=0.48f;
		// duty2=0.45f;
		// turn_around_clk(duty1,duty2);
//}


		

		/*if (distance2<=15.0f){
			duty1=0.0f;
			duty2=0.0f;
			ahead(duty1,duty2);
			cpu_sw_delay(1000);
			if(distance3>15.0f && distance<12.0f){
			duty1=0.43f;
			duty2=0.0f;
			ahead(duty1,duty2);
			}
			else{}
		}*/

		



		



		/*if (distance<6.0f)
		{
			duty1=0.52f;   //d2>d1 left
			duty2=0.5f;
			ahead(duty1,duty2);
		}
		else{
			duty1=0.5f;
			duty2=0.5f;
			ahead(duty1,duty2);
		}*/
		/*if(distance<5.0f){
			duty1=0.0f;
			duty2=0.0f;
			ahead(duty1,duty2);	
		}	
		else{
			duty1=0.7f;
			duty2=0.7f;
			ahead(duty1,duty2);
		}*/
		



			
			
	
		//cpu_sw_delay_US(500U);
		++i; // Increment i for the next test iteration...

    }

  }
  return 0;
}



void ahead(float d1, float d2){   
  set_pwm(led_pwm4, 0.0f); 
  set_pwm(led_pwm1, 0.0f);
  set_pwm(led_pwm2, d1);
  set_pwm(led_pwm3, d2);   // should rotate clockwise 

}

void back(float d1, float d2){
  set_pwm(led_pwm2, 0.0f); 
  set_pwm(led_pwm3, 0.0f);
  set_pwm(led_pwm1, d2);
  set_pwm(led_pwm4, d1);  // rotate counterclockwise
}

void turn_around_clk(float d1, float d2){
	set_pwm(led_pwm3,0.0f);
	set_pwm(led_pwm1,0.0f);
	set_pwm(led_pwm2,d1);   //left	
	set_pwm(led_pwm4,d2);

}

void turn_around_cclk(float d1, float d2){
	set_pwm(led_pwm2,0.0f); //left
	set_pwm(led_pwm4,0.0f);
	set_pwm(led_pwm1,d1); //left
	set_pwm(led_pwm3,d2);
}

void stop_run(){


	ahead(0.0f,0.0f);
}


void turnright(){
	ahead(speedaheadl,0.0f);
	cpu_sw_delay(70U);
	printf("%f \r\n","'turnright'");
}

void turnleft(){
	ahead(0.0f,speedaheadr);
	cpu_sw_delay(60U);
	printf("%f \r\n","'turnleft'");
}

int change_fuck(int fuck){

	return (fuck++);
}

























