
/*  This file contains all the I2C associated functions running on the ST32F446RTE MCU
 *  application as developed by me
 */

/*
------------------------ Includes --------------------------------------------*/

#include "I2C_driver.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

uint8_t buf[12];
int16_t val;
static const uint8_t SENS_ADDR = 0x48 << 1; // Use 8-bit address
static const uint8_t CFG_REG = 0x1;
static const uint8_t LOW_TH_REG = 0x2;
static const uint8_t HIGH_TH_REG = 0x3;

/* ----------------------------------FUNCTIONS--------------------------------*/

extern void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

// ****************************************************************************
extern int16_t I2C_read_temp_sensor()
{

	  *buf=0;

	  if (HAL_I2C_Master_Transmit(&hi2c1, SENS_ADDR, buf, 1, HAL_MAX_DELAY) != HAL_OK)
	  {
		  printf("\r\nNo sensor connected!\r\n");
		  Error_Handler();
	  }

	  if (HAL_I2C_Master_Receive(&hi2c1, SENS_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK)
	  {
		  printf("\r\nNo data from sensor received!\r\n");
		  Error_Handler();
	  }

	  val = ((int16_t)buf[0] << 4) | (buf[1] >> 4);

	  if ( val > 0x7FF ) {
        val |= 0xF000;
      }

	  val=val*0.0625;

	  return val;
}

// ****************************************************************************




