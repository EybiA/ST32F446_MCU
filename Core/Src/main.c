
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "main.h"
#include "I2C_driver.h"
#include "SPI_driver.h"
#include "ADC_driver.h"
#include "DMA_driver.h"

/* Private function prototypes -----------------------------------------------*/

static void MX_I2S1_Init(void);
static void MX_USART2_UART_Init(void);
void        SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void GPIO_blink(void);
static void read_register (unsigned int addr);
static void write_register(unsigned int addr,unsigned int val);
void help_menu(void);

/* Private variables ---------------------------------------------------------*/

I2S_HandleTypeDef hi2s1;
UART_HandleTypeDef huart2;

/* UART input/output configuration---------------------------------------------*/

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif

PUTCHAR_PROTOTYPE
{

	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1,0xFFFF);

  return ch;
}


GETCHAR_PROTOTYPE
{
  uint8_t ch = 0;

  __HAL_UART_CLEAR_OREFLAG(&huart2);

  HAL_UART_Receive (&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

  return ch;

}

/* ----------------------------------MAIN ROUTINE------------------------*/

int main(void)
{

  char input[20]="\0"; // cleaning the CLI input string
  char cmd[5]="\0";    // and commands
  unsigned int addr;
  unsigned int val;
  int16_t temp;
  float V;
  uint8_t res;
  int i=0;

  setvbuf(stdin, NULL, _IONBF, 0);

  /* Initialize all configured peripherals */

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2S1_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();

  write_register (0x4002040c,0x5100); // required for setting I2C #1 pins with internal pull ups

  printf("\r\n<<<<<<<Hello from ST32F4466RTE MCU UART terminal>>>>>\r\n");
  printf("\r\nFor menu please type help\r\n");
  printf("\r\n");

 // main CLI loop

  while (1)
  {

	  	 printf("\rST32F446RTE>>");

	  	 scanf("%[^\r]", input);

		 sscanf(input,"%s %x %x",cmd,&addr,&val);


	  	 if (strstr(cmd,"rd")) {         // reading from a register
	  		read_register(addr);
	  		printf("\r\n");

	  	 }

	  	 else if (strstr(cmd,"dump")) {    // writing to a register

	  		 for (i=0; i<val; i++) {
	  			read_register(addr);
	  			addr=addr+4;
	  		 }
	  		printf("\r\n");

	  	 }

	  	 else if (strstr(cmd,"wr")) {    // registers' dump
	  		write_register(addr,val);
	  		printf("\r\n");

	  	 }

	  	 else if (strstr(cmd,"quit")) {    // writing to a register

	  		printf("\r\n<<<<<<<Goodbye from ST32F4466RTE MCU UART terminal>>>>>\r\n");
	  		printf("\r\n");
	  		break;
	  	 }

	  	 else if (strstr(cmd,"temp")) {     // reading temperature sensor

	  		temp = I2C_read_temp_sensor();
	  		printf("\r\nTemperature is: %d\r\n",temp);

	  	 }

	  	 else if (strstr(cmd,"spi")) {     // activating SPI interface

	  		res=SPI_read();
	  		printf("\r\nST32F446 MCU SPI slave interface approached by host and answered with 0xABCDCAFE\r\n");

	  	 }

	  	 else if (strstr(cmd,"adc")) {     // sampling voltage level

	  		V=read_ADC();
	  		printf("\r\nVoltage measured at PA5 physical pin is: %.2f[V]\r\n",(float)V);

	  	 }

	  	 else if (strstr(cmd,"dac")) {     // output DAC signal

	  		DAC_generate(addr);
	  		printf("\r\nOutput the signal measured on PA5 GPIO pin....\r\n");
//          DAC_output();

	  	 }

	  	 else if (strstr(cmd,"gpio")) {     // toggling LED GPIO

	  		printf("\r\nBlink LED associated with PA5 GPIO pin....\r\n");
	  		GPIO_blink();

	  	 }


	  	 else if (strstr(cmd,"help")) {     // help menu of commands

	  		help_menu();
	  		DAC_output_Stop();

	  	 }

	  	 else if (strstr(cmd,"regs")) {     // help menu of commands

	  		regs_menu();

	  	 }

	  	 else {

	  		 printf("\runknown command, please type again...\r\n");
	  		 DAC_output_Stop();


	  	 	  }	  	 setvbuf(stdin, NULL, _IONBF, 0);

  	  }  // end of while loop

} // end of main code
/* =========================================================================================
 * */
/* ----------------------------------AUX functions------------------------*/

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Macro to configure the PLL multiplication factor
  */
  __HAL_RCC_PLL_PLLM_CONFIG(16);

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

// ****************************************************************************

static void MX_I2S1_Init(void)
{

  hi2s1.Instance = SPI1;
  hi2s1.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s1.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s1.Init.CPOL = I2S_CPOL_LOW;
  hi2s1.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s1.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    Error_Handler();
  }

}

// ****************************************************************************

/**USART2 GPIO Configuration
PA2     ------> USART2_TX
PA3     ------> USART2_RX
*/

static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}


// ****************************************************************************

void read_register(unsigned int addr)
{
	 unsigned int  data;
	 // reading a register command

	 data = READ_REG(*((unsigned int *)addr));
	 printf("\r\nRegister 0x%x, value : 0x%x \r",addr,data);

}

// ****************************************************************************

void write_register(unsigned int addr, unsigned int val)
{

	*((unsigned int *)addr)=((unsigned int *)val);


}

// ****************************************************************************

void help_menu(void)

{

	  printf("\r\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<CLI supported commands >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	  printf("\r=========================================================================================================\r\n");
	  printf("\rhelp                  : list of supported commands\r\n");
	  printf("rd <xxxx>             : read a register address <xxxx>\r\n");
	  printf("dump <xxxx> <yyyy>    : register dump from address <xxxx> # of addresses <yyyy> (32 bit each)\r\n");
	  printf("wr <xxxx> <yyyy>      : write to a register <xxxx> value <yyyy>\r\n");
	  printf("temp                  : read temperature sensor value\r\n");
	  printf("spi                   : allow host to send an SPI command to MCU SPI slave interface\r\n");
	  printf("adc                   : print analog reading from MCU ADC interface (pin A5 input)\r\n");
	  printf("dac <lvl in mV in hex>: output on MCU PA4 pin the DAC output analog voltage corresponding to the voltage level in mV in hex format\r\n");
	  printf("gpio                  : toggle GPIO PA#5 (i.e., blinking LED on the board\r\n");
	  printf("regs                  : ST32F446 registers memory mapping addresses\r\n");
	  printf("quit                  : Exit Command Line terminal \r\n");
	  printf("\r=========================================================================================================\r\n");


}
// ****************************************************************************

void regs_menu(void)

{

	  printf("\r\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<ST32F446 Registers map >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	  printf("\r=========================================================================================================\r\n");
	  printf("\rFLASH_BASE          : 0x08000000\r\n");
	  printf("CCMDATARAM_BASE       : 0x10000000\r\n");
	  printf("SRAM1_BASE            : 0x20000000\r\n");
	  printf("SRAM2_BASE            : 0x2001C000\r\n");
	  printf("SRAM3_BASE            : 0x20020000\r\n");
	  printf("PERIPH_BASE           : 0x40000000\r\n");
	  printf("Timers                : 0x40000000-0x400023FF\r\n");
	  printf("RTC                   : 0x40002800-0x40002BFF\r\n");
	  printf("SPI#2/I2S#2           : 0x40003800-0x40003BFF\r\n");
	  printf("SPI#3/I2S#3           : 0x40003C00-0x40003FFF\r\n");
	  printf("USART#2-UART5         : 0x40004400-0x400053FF\r\n");
	  printf("I2C#1-#3              : 0x40005400-0x40005FFF\r\n");
	  printf("DAC                   : 0x40007400-0x400077FF\r\n");
	  printf("ADC#1-#3              : 0x40012000-0x400123FF\r\n");
	  printf("SPI#1,#4              : 0x40013000-0x400137FF\r\n");
	  printf("SYSCFG                : 0x40013800-0x40013BFF\r\n");
	  printf("SAI #1,#2             : 0x40015800-0x40015FFF\r\n");
	  printf("GPIOs                 : 0x40020000-0x40021FFF\r\n");
	  printf("RCC                   : 0x40023800-0x40023BFF\r\n");
	  printf("DMA#1, #2             : 0x40026000-0x400267FF\r\n");
	  printf("DCMI                  : 0x50050000-0x500503FF\r\n");
	  printf("\r=========================================================================================================\r\n");


}

// ****************************************************************************

static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

}

// ****************************************************************************


void GPIO_blink()
{

	 write_register (0x40020000,0xA80087A0);  // configuring PA5 pin to GPIO
	 write_register (0x40020014,0x20);        // setting the bit to HIGH
	 HAL_Delay(1000);
	 write_register (0x40020014,0x0);         // setting the bit to LOW

	 write_register (0x40020000,0xA8008FA0);  // configuring PA5 GPIO back to
                                              // analog mode
}

// ****************************************************************************


void Error_Handler(void)
{

  __disable_irq();
  	 printf ("An error occured! Will break now...");

}

// ****************************************************************************

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */

// ****************************************************************************

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
