
/*  This file contains all the ADC associated functions running on the ST32F446RTE MCU
 *  application as developed by me
 */
//----------------------------------- Includes -------------------------------*/

#include "stm32f4xx_hal.h"
#include "main.h"
#include "ADC_driver.h"
#include "DMA_driver.h"

ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac1;
TIM_HandleTypeDef htim3;

uint16_t adc_dma_buf[10];
int32_t sensorValue=0;
volatile int adc_conv_complete_flag;
float voltage;

/* ----------------------------------FUNCTIONS--------------------------------*/
// ADC channel 5 configuration (GPIO PA5)

extern void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion   = 1;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;   // triggered by Timer3
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

  MX_DMA_Init(&hadc1);

  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}
// ****************************************************************************

/**
  * @brief DAC Initialization Function
    DAC channel OUT1 configuration (GPIO PA4)
  */
extern void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig = {0};

  hdac1.Instance = DAC;

  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

}

// ****************************************************************************

extern float read_ADC(void)
{

	sensorValue=0;

	adc_conv_complete_flag = 0;

	HAL_TIM_Base_Start(&htim3);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_dma_buf , 10); // start ADC => DMA samples transfer


	while (adc_conv_complete_flag==0)   // waiting until DMA if filled with ADC data
	{

	}

	for(size_t ind = 0; ind < 10; ++ind) {
		sensorValue += (uint32_t)adc_dma_buf[ind];

	}
	sensorValue /=10;
	voltage = (0.001*sensorValue * (ADC_REFERENCE_VOLTAGE_MV / ADC_MAX_OUTPUT_VALUE));
	return voltage-0.08;  // removing DC offset contribution

}

// ****************************************************************************

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1)
{
	adc_conv_complete_flag = 1;
	printf("\n\rSamples are moved by DMA to memory!\r");
	HAL_ADC_Stop(&hadc1);
	HAL_TIM_Base_Stop(&htim3);


}

// ****************************************************************************

extern void DAC_output(void)
{
	sensorValue=0;

	if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	for(size_t ind = 0; ind < 10; ++ind) {          //reading the ADC samples
		sensorValue += (uint32_t)adc_dma_buf[ind];  //stored on SRAM by DMA
	   }                                            // to output them to PA5 pin

	sensorValue /=10;

    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sensorValue);

}
// ****************************************************************************

extern void DAC_output_Stop(void)
{
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
}
// ****************************************************************************

extern void MX_TIM3_Init(void)
{

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;   // using 16MHz => 62.5nsec timer clock counts to ~4msec
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// ****************************************************************************




