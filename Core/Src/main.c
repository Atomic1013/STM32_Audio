/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "memorymap.h"
#include "sai.h"
#include "sdmmc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
#include "stdio.h"
#include "Audio/wm8978.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t Date[4096];
FRESULT res;
FATFS fs;
DIR dir;
FILINFO fno;
FIL file;
UINT br;
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096

uint16_t	PlayBuff_0[PLAY_BUFF_SIZE];
uint16_t	PlayBuff_1[PLAY_BUFF_SIZE];
//uint16_t* PlayBuff_0 = (uint16_t*)0x24000000;
//uint16_t* PlayBuff_1 = (uint16_t*)0x24000000+4096;

uint8_t		BuffHead[PLAY_HEADER];
uint8_t SAI_Sate = 0;
int SAI_Time = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  DebugInit(&huart1);
  printf("\n*********************************WM8978 test*****************************************\n");

  /* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
  if (WM8978_Init() != HAL_OK)
  {
    printf("检测不到WM8978芯片!!!\n");
    while (1);	/* 停机 */
  }
  printf("初始化WM8978成功\n");

  /* 延迟一段时间，等待I2S中断结束 */
  HAL_Delay(10);

  /* 复位WM8978到复位状态 */
  WM8978_Reset();

  /* 配置WM8978芯片，输入为DAC，输出为耳机 */
  WM8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
//  WM8978_CfgAudioPath(DAC_ON, SPK_ON);
//  WM8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON );

  /* 调节音量，左右相同音量 */
  WM8978_SetOUT1Volume(20);
//  WM8978_SetOUT2Volume(60);

  /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
  WM8978_CfgAudioIF(SAI_I2S_STANDARD, 16);


  res = f_mount(&fs, "0:", 1);
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}

  res = f_opendir(&dir, "0:");
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}

  res = f_readdir(&dir, &fno);
  while(res == FR_OK && fno.fname[0])
  {
	  printf("%s\n",fno.fname);
	  res = f_readdir(&dir, &fno);
  }

  res = f_open(&file, "music.wav", FA_READ);
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}

  res = f_read(&file, BuffHead, PLAY_HEADER, &br);
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}
//  printf("读取了:0x%X个字节\n",br);
  for (int i = 0; i < br; ++i)
  {
	  printf("0x%02X ",BuffHead[i]);
  }

  uint32_t Date_Size = (uint32_t)BuffHead[0x2B]<<24 | (uint32_t)BuffHead[0x2A]<<16 | (uint32_t)BuffHead[0x29]<<8 | BuffHead[0x28];
  printf("\n数据量:0x%lX\n",Date_Size);
  printf("时长 %d:%02d\n",(uint8_t)(Date_Size/(2*44100*16/8) / 60),(uint8_t)(Date_Size/(2*44100*16/8) % 60));

  res = f_read(&file, (uint8_t *)PlayBuff_0, PLAY_BUFF_SIZE * 2, &br);
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}
  HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)PlayBuff_0, PLAY_BUFF_SIZE * 2);

  while (1)
  {
	  while (SAI_Sate == 1);
	  res = f_read(&file, (uint8_t *)PlayBuff_1, PLAY_BUFF_SIZE * 2, &br);
	  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}
	  if(br < PLAY_BUFF_SIZE * 2) {printf("结束\n");f_lseek(&file, 0);}

	  while (SAI_Sate == 0);
	  res = f_read(&file, (uint8_t *)PlayBuff_0, PLAY_BUFF_SIZE * 2, &br);
	  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}
	  if(br < PLAY_BUFF_SIZE * 2) {printf("结束\n");f_lseek(&file, 0);}

  }

  res = f_closedir(&dir);
  if(res) {printf("%s(%d),错误码:%d\n",__FILE__, __LINE__,res);while(1);}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  HAL_SAI_Transmit(&hsai_BlockA1, (uint8_t*)Date, 4096, 500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 13;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
//	printf("%d\n",SAI_Time++);
	if(SAI_Sate == 0)
	{
		HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)PlayBuff_1, PLAY_BUFF_SIZE);
		SAI_Sate = 1;
	}
	else if(SAI_Sate == 1)
	{
		HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)PlayBuff_0, PLAY_BUFF_SIZE);
		SAI_Sate = 0;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
