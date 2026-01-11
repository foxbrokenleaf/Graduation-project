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
extern UART_HandleTypeDef huart1;
extern uint8_t Warn_Value;
extern uint8_t RxBuff[1];      //进入中断接收数据的数组
extern uint8_t DataBuff[256]; //保存接收到的数据的数组
extern uint8_t RxLine;           //接收到的数据长度

extern uint8_t CO_Value;     //一氧化碳
extern uint8_t Fire_Value;   //可燃气体
extern uint8_t Air_Level;    //空气质量
extern uint8_t Temptrue_Value;     //温度

extern uint8_t CO_Warn_Value;
extern uint8_t Fire_Warn_Value;   //可燃气体
extern uint8_t Air_Warn_Level;    //空气质量
extern uint8_t Temptrue_Warn_Value;     //温度
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
#define MQ_9_Pin GPIO_PIN_3
#define MQ_9_GPIO_Port GPIOA
#define MQ_135_Pin GPIO_PIN_4
#define MQ_135_GPIO_Port GPIOA
#define DHT11_RXD_Pin GPIO_PIN_10
#define DHT11_RXD_GPIO_Port GPIOB
#define DHT11_TX_Pin GPIO_PIN_11
#define DHT11_TX_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void Clear_ReceiveBuff();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
