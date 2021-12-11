/**
  ******************************************************************************
  * @file    stm32l475e_iot01.c
  * @author  MCD Application Team
  * @brief   STM32L475E-IOT01 board support package
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01.h"

/** @defgroup BSP BSP
  * @{
  */ 

/** @defgroup STM32L475E_IOT01 STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_Defines LOW LEVEL Private Def
  * @{
  */
/**
 * @brief STM32L475E IOT01 BSP Driver version number
   */
#define __STM32L475E_IOT01_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32L475E_IOT01_BSP_VERSION_SUB1   (0x01) /*!< [23:16] sub1 version */
#define __STM32L475E_IOT01_BSP_VERSION_SUB2   (0x06) /*!< [15:8]  sub2 version */
#define __STM32L475E_IOT01_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32L475E_IOT01_BSP_VERSION        ((__STM32L475E_IOT01_BSP_VERSION_MAIN << 24)\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_SUB1 << 16)\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_SUB2 << 8 )\
                                                 |(__STM32L475E_IOT01_BSP_VERSION_RC))
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_LOW_LEVEL_Private_Variables LOW LEVEL Variables 
  * @{
  */

<<<<<<< HEAD
=======
const uint32_t GPIO_PIN[LED2n] = {LED2_PIN};
const uint32_t GPIO_PIN[LED1n] = {LED1_PIN};
>>>>>>> bd94e3207a170b1fc6d2fcff38a6f7820c63f9be

const uint32_t GPIO_PIN[LED1] = {LED1_PIN};
               GPIO_PIN[LED2] = {LED2_PIN};

<<<<<<< HEAD
GPIO_TypeDef* GPIO_PORT[LED1] = {LED1_GPIO_PORT};
              GPIO_PORT[LED2] = {LED2_GPIO_PORT};
=======
GPIO_TypeDef* GPIO_PORT[LED2n] = {LED2_GPIO_PORT};
GPIO_TypeDef* GPIO_PORT[LED1n] = {LED1_GPIO_PORT};

>>>>>>> bd94e3207a170b1fc6d2fcff38a6f7820c63f9be

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};


/**
  * @brief  This method returns the STM32L475E IOT01 BSP Driver revision
  * @retval version  0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __STM32L475E_IOT01_BSP_VERSION;
}

/**
  * @brief  Initializes LED GPIO.
  * @param  Led  LED to be initialized. 
  *              This parameter can be one of the following values:
  *                @arg  LED2
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;
  
  LEDx_GPIO_CLK_ENABLE(Led);
  /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin   = GPIO_PIN[Led];
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(GPIO_PORT[Led], &gpio_init_structure);
}

/**
  * @brief  DeInitializes LED GPIO.
  * @param  Led  LED to be deinitialized. 
  *              This parameter can be one of the following values:
  *                @arg  LED2
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;
  
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = GPIO_PIN[Led];
  
  /* Turn off LED */
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  HAL_GPIO_DeInit(GPIO_PORT[Led], gpio_init_structure.Pin);
}

/**
  * @brief  Turns the selected LED On.
  * @param  Led  LED to be set on 
  *              This parameter can be one of the following values:
  *                @arg  LED2
  */
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

/**
  * @brief  Turns the selected LED Off. 
  * @param  Led  LED to be set off
  *              This parameter can be one of the following values:
  *                @arg  LED2
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led  LED to be toggled
  *              This parameter can be one of the following values:
  *                @arg  LED2
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Initializes push button GPIO and EXTI Line.
  * @param  Button  Button to be configured
  *                 This parameter can be one of the following values:
  *                   @arg  BUTTON_USER  User Push Button 
  */
void BSP_PB_Init(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;
  
  /* Enable the BUTTON clock */
  USER_BUTTON_GPIO_CLK_ENABLE();
    
  /* Configure Button pin as input */
  gpio_init_structure.Pin = BUTTON_PIN[Button];
  gpio_init_structure.Mode = GPIO_MODE_INPUT;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
}

/**
  * @brief  DeInitializes push button.
  * @param  Button  Button to be configured
  *                 This parameter can be one of the following values:
  *                   @arg  BUTTON_USER  User Push Button 
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}


/**
  * @brief  Returns the selected button state.
  * @param  Button  Button to be checked
  *                 This parameter can be one of the following values:
  *                   @arg  BUTTON_USER  User Push Button 
  * @retval The Button GPIO pin value (GPIO_PIN_RESET = button pressed)
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
