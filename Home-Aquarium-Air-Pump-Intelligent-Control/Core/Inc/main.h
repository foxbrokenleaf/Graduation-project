/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define C32_768I_Pin GPIO_PIN_14
#define C32_768I_GPIO_Port GPIOC
#define C32_768O_Pin GPIO_PIN_15
#define C32_768O_GPIO_Port GPIOC
#define C8I_Pin GPIO_PIN_0
#define C8I_GPIO_Port GPIOD
#define C8O_Pin GPIO_PIN_1
#define C8O_GPIO_Port GPIOD
#define YaLi_Pin GPIO_PIN_6
#define YaLi_GPIO_Port GPIOA
#define ZuoDu_Pin GPIO_PIN_7
#define ZuoDu_GPIO_Port GPIOA
#define Motor_Pin GPIO_PIN_0
#define Motor_GPIO_Port GPIOB
#define QiBeng_Pin GPIO_PIN_1
#define QiBeng_GPIO_Port GPIOB
#define Buzzer_Pin GPIO_PIN_2
#define Buzzer_GPIO_Port GPIOB
#define OLED_CLK_Pin GPIO_PIN_10
#define OLED_CLK_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_11
#define OLED_SDA_GPIO_Port GPIOB
#define K3_Pin GPIO_PIN_12
#define K3_GPIO_Port GPIOB
#define K2_Pin GPIO_PIN_13
#define K2_GPIO_Port GPIOB
#define K1_Pin GPIO_PIN_14
#define K1_GPIO_Port GPIOB
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define BT_RXD_Pin GPIO_PIN_6
#define BT_RXD_GPIO_Port GPIOB
#define BT_TXD_Pin GPIO_PIN_7
#define BT_TXD_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
