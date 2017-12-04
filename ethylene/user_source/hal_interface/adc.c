#include "adc.h"


ADC_HandleTypeDef ADC_setup(void)
{
	
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


ADC1->CR |= ADC_CR_ADEN; // Enable ADC1
while(!ADC1->ISR & ADC_ISR_ADRD); // wait for ADRDY 
HAL_ADC_Start(&hADC1);

return hADC1;

}
