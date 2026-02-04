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
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern uint8_t Warn_Value;
extern uint8_t RxBuff[1];      //�����жϽ������ݵ�����
extern uint8_t DataBuff[256]; //������յ������ݵ�����
extern uint8_t RxLine;           //���յ������ݳ���

extern uint16_t CO_Value;     //һ����̼
extern uint16_t Fire_Value;   //��ȼ����
extern uint16_t Air_Level;    //��������
extern uint16_t Temptrue_Value;     //�¶�

extern uint16_t CO_Warn_Value;
extern uint16_t Fire_Warn_Value;   //��ȼ����
extern uint16_t Air_Warn_Level;    //��������
extern uint16_t Temptrue_Warn_Value;     //�¶�

extern uint32_t SystemTick;
extern uint16_t TIM_Counter;
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
#define BUZZER_Pin GPIO_PIN_4
#define BUZZER_GPIO_Port GPIOE
#define MQ_9_Pin GPIO_PIN_3
#define MQ_9_GPIO_Port GPIOA
#define MQ_135_Pin GPIO_PIN_4
#define MQ_135_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void Clear_ReceiveBuff();
uint8_t Read_Temperature(void);
void ESP01S_DataReceived(uint8_t link_id, uint8_t *data, uint16_t length);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
