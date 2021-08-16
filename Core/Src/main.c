/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DS18B20.h"
#include "sensor.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
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
double freqNotCalibrated = 0.0;
uint32_t freq = 0;
uint8_t endOfFreqMeasureFlag = 0;

char str[20] =
  {
  0,
  };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void waitForClickButton(void);
void goToSTOPMode(void);
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
  MX_GPIO_Init();
  MX_I2C1_Init();
  hi2c1.Init.OwnAddress1 = 0x64;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
    Error_Handler();
    }
  initSTOPMode();
  waitForClickButton();
  goToSTOPMode();
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
//  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim3);

  uint8_t readyFlag = 1, startFlag = 0;
  float temperatureDS18B20 = 0.0;
  unsigned char temp[sizeof(float)];
  uint8_t message = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
    {
    /*
     *        I2C command processing
     */
    while(readyFlag == 1)
      {
      do
        {
        if (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t*)&message, sizeof(uint8_t)) != HAL_OK)
          {
          Error_Handler();
          }
        while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
          {
          }
        } while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      readyFlag = 0;
      }

    if (message == START_TRANSMISSION)
      {
      message = 0;
      for (uint8_t i = 0; i < 5; i++)
        {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(300);
        }
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      startFlag = 1;
      readyFlag = 1;
      }

    else if (message == MOISTURE_MEASURE && startFlag == 1)
      {
      message = 0;
      endOfFreqMeasureFlag = 0;
      HAL_TIM_Base_Start_IT(&htim1);
      do
        {

        } while(endOfFreqMeasureFlag == 0 || freqNotCalibrated == 0);

      HAL_TIM_Base_Stop_IT(&htim1);
      freq = round(freqNotCalibrated);
      if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t*) &freq, sizeof(freq), HAL_MAX_DELAY) != HAL_OK)
        {
        Error_Handler();
        }
      else readyFlag = 1;

      endOfFreqMeasureFlag = 0;
      }

    else if (message == TEMPERATURE_MEASURE && startFlag == 1)
      {
      //ds18b20_init(DS18B20_RESOLUTION_12_BIT, Ds18b20_1wire_GPIO_Port, Ds18b20_1wire_Pin);
      temperatureDS18B20 = receiveDataDs18b20();
      memcpy(temp, &temperatureDS18B20, sizeof(temperatureDS18B20));
      if (HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t*) temp, sizeof(temp), HAL_MAX_DELAY) != HAL_OK)
        {
        Error_Handler();
        }
      }
    else if (message == STOP_TRANSMISSION)
      {
      message = 0;
      startFlag = 0;
      readyFlag = 1;
      initSTOPMode();
      goToSTOPMode();
      }
    else readyFlag = 1;

//  if (flag == 1)
//    {
//    snprintf(str, 96, "FREQUENCY: %.3f MHz | %.3f KHz | %lu Hz\n--------------------\n", (float) freq / 1000000.0, (float) freq / 1000.0, freq);
//    HAL_UART_Transmit(&huart1, (uint8_t*) str, strlen(str), 1000);
//    HAL_UART_Transmit(&huart1, (uint8_t*) "\n\r", 2, 1000);
//    flag = 0;
//    }
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
  RCC_OscInitTypeDef RCC_OscInitStruct =
    {
    0
    };
  RCC_ClkInitTypeDef RCC_ClkInitStruct =
    {
    0
    };
  RCC_PeriphCLKInitTypeDef PeriphClkInit =
    {
    0
    };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
    Error_Handler();
    }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
    Error_Handler();
    }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_2);
  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
    Error_Handler();
    }
  }

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
  {
  if (htim == &htim1)
    {
    uint16_t TIM2_count = __HAL_TIM_GET_COUNTER(&htim2);               // Значение счетчика таймера TIM2
                                                                       // (Количество последних подсчитанных
                                                                       // импульсов к моменту прерывания)
    uint16_t TIM2_overflows = __HAL_TIM_GET_COUNTER(&htim3);               // значение в счётчике таймера №3
    uint16_t TIM2_resolution = __HAL_TIM_GET_AUTORELOAD(&htim2);               // значение переполнения таймера №2 (65535)
    freqNotCalibrated = 1.003764115432873 * (TIM2_count + (TIM2_overflows * TIM2_resolution) + TIM2_overflows);               // вычисляем
    endOfFreqMeasureFlag = 1;

    //////////////// обнуляем счётчики и рестартуем таймер №1 /////////////////
    __HAL_TIM_SET_COUNTER(&htim2, 0x0000);
    __HAL_TIM_SET_COUNTER(&htim3, 0x0000);
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
  //__disable_irq();
  HAL_UART_Transmit(&huart1, (uint8_t*) "Error handler", strlen("Error handler"), 1000);
  HAL_UART_Transmit(&huart1, (uint8_t*) "\n\r", 2, 1000);
  while(1)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
