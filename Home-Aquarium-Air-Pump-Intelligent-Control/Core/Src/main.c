/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "Key.h"
#include <stdarg.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
  AirPump_Close = 0,
  AirPump_Low,
  AirPump_Med,
  AirPump_High,
  AirPump_Open
}AirPumpLevel;
typedef enum{
  UI_Main = 0,
  UI_Level,
  UI_Tiemr
}UIIndex_;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RUN_MODE 1  // 1 debug 0 normal
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char PrintfString[2][128];
uint32_t Timer2Tick = 0;
uint32_t Timer2Tick_F = 0;
uint32_t Timer3Tick = 0;
uint16_t UILevelCloseTick = 0;
AirPumpLevel apl = AirPump_Low;
uint8_t UiIndex = UI_Main;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void PrintfGunDong(char *format, ...);
char* GetAirPumpLevelStr(void);
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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  PrintfGunDong("Display ok!");
  HAL_Delay(500);
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
  PrintfGunDong("Buzzer close!");
  HAL_Delay(500);
  HAL_GPIO_WritePin(Motor_GPIO_Port, Motor_Pin, GPIO_PIN_SET);
  PrintfGunDong("Motor close!");
  HAL_Delay(500);
  HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_SET);
  PrintfGunDong("QiBeng close!");
  HAL_Delay(500);
  HAL_TIM_Base_Start_IT(&htim2);
  PrintfGunDong("Star TIM2!");
  HAL_Delay(500);
  HAL_TIM_Base_Start_IT(&htim3);
  PrintfGunDong("Star TIM3!");
  HAL_Delay(500);

  uint8_t KeyClickCounter = 0;
  if(apl == AirPump_Low) Timer2Tick_F = 500 * 14;
  if(apl == AirPump_Med) Timer2Tick_F = 500 * 56;
  if(apl == AirPump_High) Timer2Tick_F = 500 * 112;  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    OLED_Clear();
    if(Timer3Tick >= 150){
      Timer3Tick = 0;
      if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_SET){
        HAL_GPIO_TogglePin(QiBeng_GPIO_Port, QiBeng_Pin);
        // apl =  HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) ? AirPump_Close : AirPump_Open;
        // HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
        
      }
      if(HAL_GPIO_ReadPin(K2_GPIO_Port, K2_Pin) == GPIO_PIN_SET){
        UiIndex = UI_Level;
        UILevelCloseTick = 0;
        apl++;
        if(apl == AirPump_Open) apl = AirPump_Low;
      }
      if(HAL_GPIO_ReadPin(K3_GPIO_Port, K3_Pin) == GPIO_PIN_SET){
        
        
      } 
    }
       
      
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //500ms -> 9mL
    if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) == GPIO_PIN_RESET && Timer2Tick >= Timer2Tick_F){
      Timer2Tick = 0;
      HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_SET);
    }
    if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin)) Timer2Tick = 0;
    if(UiIndex != UI_Level) UILevelCloseTick = 0;
    if(UiIndex == UI_Main){
      OLED_ShowString(0, 0, "AP:", OLED_6X8);
      OLED_ShowString(18, 0, HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) ? "Close|" : "Open |", OLED_6X8);
      OLED_ShowString(54, 0, "O_2(mL)", OLED_6X8);
      OLED_ShowNum(54, 8, (Timer2Tick / 500) * 9, 7, OLED_6X8);

      OLED_ShowString(0, 8, "WQ:", OLED_6X8);
      OLED_ShowString(18, 8, "00.00|", OLED_6X8); 
    }
    else if(UiIndex == UI_Level){
      OLED_ShowString(6, 0, "Air Pump Level", OLED_6X8);
      OLED_ShowString(24, 8, GetAirPumpLevelStr(), OLED_6X8);
      if(UILevelCloseTick >= 3000){
        UiIndex = UI_Main;
        if(apl == AirPump_Low) Timer2Tick_F = 500 * 14;
        if(apl == AirPump_Med) Timer2Tick_F = 500 * 56;
        if(apl == AirPump_High) Timer2Tick_F = 500 * 112;
      }
    }

   

    OLED_Update();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void PrintfGunDong(char *format, ...){
    
    va_list arg;
    va_start(arg, format);
    strcpy(PrintfString[0], (const char*)PrintfString[1]);
    vsprintf(PrintfString[1], format, arg);
    va_end(arg);
    OLED_Clear();
    OLED_ShowString(0, 0, PrintfString[0], OLED_6X8);
    OLED_ShowString(0, 8, PrintfString[1], OLED_6X8);
    OLED_Update();
}

char* GetAirPumpLevelStr(void){
  if(apl == AirPump_Low) return "  Low  ";
  if(apl == AirPump_Med) return "Medium ";
  if(apl == AirPump_High) return "  High ";
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) { // �ж��Ƿ�Ϊ TIM2 ���ж�
    Timer2Tick++;
    UILevelCloseTick++;
  }
  if (htim->Instance == TIM3) { // �ж��Ƿ�Ϊ TIM3 ���ж�
    Timer3Tick++;

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
#ifdef USE_FULL_ASSERT
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
