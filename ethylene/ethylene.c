/*
	This is the main user software application	
	(c) Ying Zhu
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


#define OP_READ 0x03




#define ADEN 0x00000001
//#define VREFINT_CAL_ADDR    0x1FFFF7BA

uint8_t SPI_read(SPI_HandleTypeDef hspi, uint32_t addr){

        
      return 0;
     


}




int main()
{

  //OPCODE_WREN   = 0b0110,     /* Set Enable Latch */
  //OPCODE_WRDI   = 0b0100,     /* Reset Write Enable Latch */
  //OPCODE_RDSR   = 0b0101,     /* Read Status Register */
  //OPCODE_WRSR   = 0b0001,     /* Write Status Register */
 
  //OPCODE_WRITE  = 0b0010,     /* Write Memory */
  //OPCODE_RDID   = 0b10011111  /* Read Device ID */
  /*
    Initialize the PLL, clock tree to all peripherals, flash and Systick 1 ms time reference:
   */
  cpu_init();

   uart_debug_init(); //initialize PA9 and PA10,           9 is Rx ,10 is Tx
 
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

//ADC_HandleTypeDef hADC1 = ADC_setup(); //sets up , enables, and starts ADC1 channel 1, which is PA0 

/////////////////////////////////////////SET UP SPI STUFF///////////////////////////////////////////////////////////////////////


	__SPI1_CLK_ENABLE(); //enable SPI1
    __HAL_RCC_SPI1_CLK_ENABLE();
    SPI_HandleTypeDef hspi;
    hspi.Instance = SPI1;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; //prescale 8, frequency / 8
    hspi.Init.Direction = SPI_DIRECTION_2LINES; 
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi.Init.NSS = SPI_NSS_SOFT; //use software slave manager
    hspi.Init.TIMode = SPI_TIMODE_DISABLED;
    hspi.Init.Mode = SPI_MODE_MASTER; 
    while (HAL_SPI_Init(&hspi) != HAL_OK)
    {
         
    }



 //initiallize SPI1, SPI1 mapping is AF5 alternate funcion 5, maps PA4 as CS, PA5 as SCK, PA6 as MISO, and PA7 as MOSI

    static GPIO_InitTypeDef  GPIO_InitStruct_SPI;
	  GPIO_InitStruct_SPI.Pin       = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct_SPI.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct_SPI.Pull      = GPIO_PULLUP;
    GPIO_InitStruct_SPI.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct_SPI.Alternate = GPIO_AF5_SPI1;
	__HAL_RCC_GPIOA_CLK_ENABLE();

	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_SPI);
////////////////////////////////////////////////

    
    static GPIO_InitTypeDef  GPIO_InitStruct_SPI_cs;
    GPIO_InitStruct_SPI_cs.Pin  |= GPIO_PIN_4; // PA4 as CS 
    GPIO_InitStruct_SPI_cs.Mode = GPIO_MODE_OUTPUT_PP;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_SPI_cs);

while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t value;


/////////////////////////////READ WORKS/////////////////
        
        uint8_t cmd[] = {0x00,0x03,0x00,0x00};
      
      
        uint8_t data_get[5] = {0};

        for(int i = 0; i < sizeof(data_get) ; i++){
          data_get[i] = 27;
}

       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); //make CS 0
        
        HAL_SPI_TransmitReceive(&hspi, (&cmd), &data_get , sizeof(cmd) , HAL_MAX_DELAY); //transmit opcode to read and read address
        while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //make it 1
       
      for(int i = 0; i < sizeof(data_get) ; i++){

        printf("%d index %d \r\n", data_get[i],i);
      }
      printf("Initial read finished, it is currently %d \r\n", data_get[2]);
      cpu_sw_delay(100);
       ////////////////////////////////////////////WRITE///////////////////
        printf("TIME TO WRITE \r\n");
uint8_t cmdW1[] = {0x00,0x06,0x00,0x00}; //ENABLE WRITE LATCH
         
      uint8_t dummy[10] = {0};

       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); //make CS 0
        
        HAL_SPI_TransmitReceive(&hspi, (&cmdW1), &dummy , sizeof(cmdW1) , HAL_MAX_DELAY); //transmit opcode to read and read address
        while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //make it 1
         cpu_sw_delay(100);

       ///////////////////        WRITE THE DATA
      uint8_t cmdW2[] = {0x00,0x02,0x25,0x00};   
        
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); //make CS 0
        
        HAL_SPI_TransmitReceive(&hspi, (&cmdW2), &dummy , sizeof(cmdW2) , HAL_MAX_DELAY); //transmit opcode to read and read address
        while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //make it 1

 cpu_sw_delay(100);
        ////////////CLOSE WRITE LATCH

        uint8_t cmdW3[] = {0x00,0x04,0x00,0x00}; //ENABLE WRITE LATCH

       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); //make CS 0
        
        HAL_SPI_TransmitReceive(&hspi, (&cmdW3), &dummy , sizeof(cmdW3) , HAL_MAX_DELAY); //transmit opcode to read and read address
        while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //make it 1
        printf("write finished, lets read \r\n");
 cpu_sw_delay(100);
       


  ///////////////////////////////////////////////////TEST //////////////////////////////////////////

        uint8_t cmd2[] = {0x00,0x03,0x00,0x00};
      
      
        uint8_t data_get2[5] = {0};

        for(int i = 0; i < sizeof(data_get2) ; i++){
          data_get2[i] = 27;
}

       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); //make CS 0
      
        HAL_SPI_TransmitReceive(&hspi, (&cmd2), &data_get2 , sizeof(cmd2) , HAL_MAX_DELAY); //transmit opcode to read and read address
        while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //make it 1
       
      for(int i = 0; i < sizeof(data_get2) ; i++){

        printf("%d index %d \r\n", data_get2[i],i);
      }

printf("final read compelte, is it right? IT SHOULD SAYYYYYYYY %d FOR INDEX 2 \r\n", data_get2[2]);




     

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
 	 // if( HAL_ADC_PollForConversion(&hADC1,10000)  == HAL_OK )
//{
//	   value = HAL_ADC_GetValue(&hADC1);
//}
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
      //voltage *= 100;
      //voltage = roundf(voltage);
      //voltage /= 100;
 
     //printf("%f millivolts \r\n",voltage);
      ++i; // Increment i for the next test iteration...
    }
  }

  return 0;
}
