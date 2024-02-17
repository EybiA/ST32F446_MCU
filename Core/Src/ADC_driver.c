
/*  This file contains all the ADC associated functions running on the ST32F446RTE MCU
 *  application as developed by me
 */
//----------------------------------- Includes -------------------------------*/

#include "stm32f4xx_hal.h"
#include "ADC_driver.h"
#include "DMA_driver.h"

ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac1;

__IO uint16_t adc_dma_result = 0;
int32_t sensorValue;
volatile int adc_conv_complete_flag;
float voltage;

/* ----------------------------------FUNCTIONS--------------------------------*/
// ADC channel 5 configuration (GPIO PA5)

extern void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion   = 0;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = DISABLE;


  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  MX_DMA_Init(&hadc1);
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
	adc_conv_complete_flag = 0;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_dma_result , 1); // link DMA to ADC #1 buffer

	while (adc_conv_complete_flag==0)   // waiting until DMA if filled with ADC data
	{

	}

	sensorValue = (uint32_t)adc_dma_result;
	voltage = (0.001*sensorValue * (ADC_REFERENCE_VOLTAGE_MV / ADC_MAX_OUTPUT_VALUE));
	HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop_DMA(&hadc1);
	return voltage-0.08;  // removing DC offset contribution
}

// ****************************************************************************

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1)
{
	adc_conv_complete_flag = 1;

}

// ****************************************************************************

extern void DAC_output(void)
{

	if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	HAL_ADC_Start(&hadc1);

	if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
	{
	    sensorValue = (int32_t)HAL_ADC_GetValue(&hadc1);
	    HAL_ADC_Stop(&hadc1);
	}

    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sensorValue);
    HAL_Delay(100);
}
// ****************************************************************************

extern void DAC_output_Stop(void)
{
	HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
}
// ****************************************************************************






