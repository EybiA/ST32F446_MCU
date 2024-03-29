
//------------------------ Includes --------------------------------------------*/

#include "SPI_driver.h"
#include <stdio.h>

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;

/* ----------------------------------FUNCTIONS--------------------------------*/

/**SPI2 GPIO Configuration
PC1     ------> SPI2_MOSI (Nucleo FR446 board, CN8 pin #5)
PC2     ------> SPI2_MISO (Nucleo FR446 board, CN7 pin #35)
PB10     ------> SPI2_SCK (Nucleo FR446 board, CN9 pin #7)
PB12     ------> SPI2_NSS (Nucleo FR446 board, CN10 pin #14)
*/

extern void MX_SPI2_Init(void)
{

	  hspi2.Instance = SPI2;
	  hspi2.Init.Mode = SPI_MODE_SLAVE;
	  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	  hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
	  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi2.Init.CRCPolynomial = 10;
	  if (HAL_SPI_Init(&hspi2) != HAL_OK)
	  {
	    Error_Handler();
	  }

}

// ****************************************************************************

extern uint8_t SPI_read(void)
{

	  uint8_t *buf=0;
	  uint8_t response[]={0xab,0xcd,0xca,0xfe};

	  while (!(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_READY));

	  HAL_SPI_Receive(&hspi2, &buf, 1,HAL_MAX_DELAY);

	  if (buf==0x5) {

		  HAL_SPI_Transmit(&hspi2, &response, 4,HAL_MAX_DELAY);

	  };

	  return &response;
}


