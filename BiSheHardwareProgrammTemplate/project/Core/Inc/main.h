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
#define KEY_L_Pin GPIO_PIN_12
#define KEY_L_GPIO_Port GPIOB
#define KEY_C_Pin GPIO_PIN_13
#define KEY_C_GPIO_Port GPIOB
#define KEY_B_Pin GPIO_PIN_14
#define KEY_B_GPIO_Port GPIOB
#define KEY_R_Pin GPIO_PIN_15
#define KEY_R_GPIO_Port GPIOB
#define KEY_T_Pin GPIO_PIN_8
#define KEY_T_GPIO_Port GPIOD
#define DS18B20_DO_Pin GPIO_PIN_9
#define DS18B20_DO_GPIO_Port GPIOD
#define PRS_DT_Pin GPIO_PIN_10
#define PRS_DT_GPIO_Port GPIOD
#define PRS_SCK_Pin GPIO_PIN_11
#define PRS_SCK_GPIO_Port GPIOD
#define M_PWM_Pin GPIO_PIN_12
#define M_PWM_GPIO_Port GPIOD
#define IR_DO_Pin GPIO_PIN_13
#define IR_DO_GPIO_Port GPIOD
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define RELAY1_Pin GPIO_PIN_8
#define RELAY1_GPIO_Port GPIOB
#define RELAY2_Pin GPIO_PIN_9
#define RELAY2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
