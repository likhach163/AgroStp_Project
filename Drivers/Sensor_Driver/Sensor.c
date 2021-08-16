/*
 * Sensor.c
 *
 *  Created on: May 5, 2021
 *      Author: Лихачев Иван
 */
#include "main.h"
#include "Sensor.h"
#include "gpio.h"
#include "i2c.h"

void initI2CAddress(void)
  {
  if (HAL_GPIO_ReadPin(I2C_ADDR_MIDDLE_GPIO_Port, I2C_ADDR_MIDDLE_Pin) == 1)
    {
    hi2c1.Init.OwnAddress1 = I2C_ADDRESS_MIDDLE;
    }
  else if (HAL_GPIO_ReadPin(I2C_ADDR_MIDDLE_GPIO_Port, I2C_ADDR_LOWER_Pin) == 1)
    {
    hi2c1.Init.OwnAddress1 = I2C_ADDRESS_LOWER;
    }
  else hi2c1.Init.OwnAddress1 = I2C_ADDRESS_UPPER;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
    Error_Handler();
    }
  }

void initSTOPMode(void)
  {
  SET_BIT(EXTI->IMR1, EXTI_IMR1_IM23);                 //Разрешение генерирования прерывания от I2C1 для пробуждения МК
  SET_BIT(RCC->CFGR, RCC_CFGR_STOPWUCK);               //Установка бита STOPWUCK: 0 - после пробуждения тактирование
                                                       //микроконтроллера осуществляется от MSI 4МГц; 1 - от HSI 16МГц
  SET_BIT(PWR->CR1, PWR_CR1_VOS_1);                    //Установка бита VOS: 0 - питание процессора 1.2В, макс. тактовая частота до 80 МГц
                                                       //1 - питание процессора 1.0В, макс. тактовая частота до 26 МГц
  NVIC_EnableIRQ(EXTI9_5_IRQn);                        //Включение источника прерывания для выхода из режима Stop
  NVIC_SetPriority(EXTI9_5_IRQn, 0);                   //Установка приоритета прерывания
  }

void goToSTOPMode(void)
  {
  HAL_I2CEx_EnableWakeUp(&hi2c1);               //Включение режима пробуждения МК по прерыванию от I2C1
  __disable_irq();                              //Отключение прерываний
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);               //Вход в Stop режим
  HAL_I2CEx_DisableWakeUp(&hi2c1);               //Отключение режима пробуждения МК по прерыванию от I2C1
  __enable_irq();
  }

void waitForClickButton(void)               //Функция ожидания нажатия кнопки
  {
  while(HAL_GPIO_ReadPin(BUT_GPIO_Port, BUT_Pin) != GPIO_PIN_SET)
    {
    }
  while(HAL_GPIO_ReadPin(BUT_GPIO_Port, BUT_Pin) != GPIO_PIN_RESET)
    {
    }
  }

