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
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "Key.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
  AirPump_Low = 0,
  AirPump_Med,
  AirPump_High,
}AirPumpLevel;
typedef enum{
  UI_Main = 0,
  UI_Level,
  UI_RTC,
  UI_Tiemr_Motor,
  UI_Tiemr_AirPump
}UIIndex_;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RUN_MODE 1  // 1 debug 0 normal
#define TIMER_AIR_PUMP_CHANNEL_MAX 3
#define UI_INDEX_MAX (UI_Tiemr_AirPump + TIMER_AIR_PUMP_CHANNEL_MAX)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RTC_TimeTypeDef nTime;

uint32_t Timer2CounterTick = 0;

uint32_t TimerMotorTick_s_End = 0;
uint32_t TimerMotorTick_s = 0;
uint32_t TimerMotorTick_s_Run = 0;

uint32_t TimerAirPumpTick_s_End[TIMER_AIR_PUMP_CHANNEL_MAX] = {0};
uint32_t TimerAirPumpTick_s[TIMER_AIR_PUMP_CHANNEL_MAX] = {0};
uint32_t TimerAirPumpTick_s_Run[TIMER_AIR_PUMP_CHANNEL_MAX] = {0};

char PrintfString[2][128];
uint32_t Timer2Tick = 0;
uint32_t Timer2Tick_F = 0;

uint32_t Timer2TickAuto[TIMER_AIR_PUMP_CHANNEL_MAX] = {0};
uint32_t Timer2TickAuto_F[TIMER_AIR_PUMP_CHANNEL_MAX] = {0};

uint32_t Timer3Tick = 0;
uint16_t UILevelCloseTick = 0;
AirPumpLevel apl = AirPump_Low;
uint8_t UiIndex = UI_Main;
uint8_t MenuLevel = 0;
uint8_t TimerMotor[3] = { 0 };
uint8_t TimerAirPump[TIMER_AIR_PUMP_CHANNEL_MAX][4] = { 0 };
uint8_t TimerMotorIndex = 0;
uint8_t TimerAirPumpIndex = 0;
uint8_t TimerAirPumpAutoRun[TIMER_AIR_PUMP_CHANNEL_MAX] = { 0 };
uint8_t TimerAirPumpAutoRunTask[TIMER_AIR_PUMP_CHANNEL_MAX] = { 0 };
uint8_t TimerAirPumpManuallyFlag = 0;
uint8_t TimerAirPumpChannel = 0;
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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_RTC_GetTime(&hrtc, &nTime, RTC_FORMAT_BIN);
  PrintfGunDong("Get RTC!");
  HAL_Delay(500);  
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

  if(apl == AirPump_Low) Timer2Tick_F = 500 * 14;
  if(apl == AirPump_Med) Timer2Tick_F = 500 * 56;
  if(apl == AirPump_High) Timer2Tick_F = 500 * 112;  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    OLED_Clear();
    if(Timer2CounterTick >= 1000){
      TimerMotorTick_s++;
      for(uint8_t i = 0;i < TIMER_AIR_PUMP_CHANNEL_MAX;i++) TimerAirPumpTick_s[i]++;
      Timer2CounterTick = 0;
    }
    HAL_RTC_GetTime(&hrtc, &nTime, RTC_FORMAT_BIN);
    if(Timer3Tick >= 150){
      Timer3Tick = 0;
      if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_SET){
        if(UiIndex == UI_Tiemr_Motor && MenuLevel == 1){
          TimerMotorIndex++;
          TimerMotorIndex %= 3;  
        }
        if(UiIndex >= UI_INDEX_MAX - TIMER_AIR_PUMP_CHANNEL_MAX && MenuLevel == 1){
          TimerAirPumpIndex++;
          TimerAirPumpIndex %= 4;
        }        
      }
      if(HAL_GPIO_ReadPin(K2_GPIO_Port, K2_Pin) == GPIO_PIN_SET){
        if(MenuLevel == 0) UiIndex++;
        if(MenuLevel == 1 && UiIndex == UI_Level){
          apl++;
          apl %= 3;
        }
        if(MenuLevel == 1 && UiIndex == UI_Tiemr_Motor){
          if(TimerMotorIndex == 0){
            TimerMotor[0]++;
            TimerMotor[0] %= 24;
          }
          if(TimerMotorIndex == 1 || TimerMotorIndex == 2){
            TimerMotor[TimerMotorIndex]++;
            TimerMotor[TimerMotorIndex] %= 60;
          }
          TimerMotorTick_s_End = TimerMotorTick_s + TimerMotor[0] * 3600 + TimerMotor[1] * 60 + TimerMotor[2];
        }
        if(MenuLevel == 1 && UiIndex >= UI_INDEX_MAX - TIMER_AIR_PUMP_CHANNEL_MAX){
          if(TimerAirPumpIndex == 0){
            TimerAirPump[TimerAirPumpChannel][0]++;
            TimerAirPump[TimerAirPumpChannel][0] %= 24;
          }
          if(TimerAirPumpIndex == 1 || TimerAirPumpIndex == 2){
            TimerAirPump[TimerAirPumpChannel][TimerAirPumpIndex]++;
            TimerAirPump[TimerAirPumpChannel][TimerAirPumpIndex] %= 60;
          }
          if(TimerAirPumpIndex == 3){
            TimerAirPump[TimerAirPumpChannel][TimerAirPumpIndex]++;
            TimerAirPump[TimerAirPumpChannel][TimerAirPumpIndex] %= 3;
          }
          TimerAirPumpTick_s_End[TimerAirPumpChannel] = TimerAirPumpTick_s[TimerAirPumpChannel] + TimerAirPump[TimerAirPumpChannel][0] * 3600 + TimerAirPump[TimerAirPumpChannel][1] * 60 + TimerAirPump[TimerAirPumpChannel][2];
          if(TimerAirPump[TimerAirPumpChannel][0] != 0 || TimerAirPump[TimerAirPumpChannel][1] != 0 || TimerAirPump[TimerAirPumpChannel][2] != 0) TimerAirPumpAutoRun[TimerAirPumpChannel] = 1;
          else TimerAirPumpAutoRun[TimerAirPumpChannel] = 0;
        }
        
        UiIndex %= UI_INDEX_MAX;
        if(UiIndex == UI_Main) TimerAirPumpChannel = 0;
        if(UiIndex > UI_INDEX_MAX - TIMER_AIR_PUMP_CHANNEL_MAX && MenuLevel == 0){
          TimerAirPumpChannel++;
          TimerAirPumpChannel %= 3;
        }
        // UiIndex = UI_Level;
        // UILevelCloseTick = 0;
        // apl++;
        // if(apl == AirPump_Open) apl = AirPump_Low;
      }
      if(HAL_GPIO_ReadPin(K3_GPIO_Port, K3_Pin) == GPIO_PIN_SET){
        if(UiIndex == UI_Main){
          HAL_GPIO_TogglePin(QiBeng_GPIO_Port, QiBeng_Pin);
          TimerAirPumpManuallyFlag = 1;
        }
        if(UiIndex != UI_Main){
          MenuLevel++;
          MenuLevel %= 2;
        }

      } 
    }
       
      
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(UiIndex == UI_Main){
      OLED_ShowString(0, 0, "AP:", OLED_6X8);
      OLED_ShowString(18, 0, HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) ? "Close|" : "Open |", OLED_6X8);
      OLED_ShowString(54, 0, "O_2(mL)", OLED_6X8);
      // OLED_ShowNum(54, 8, (Timer2Tick / 500) * 9, 7, OLED_6X8);
      OLED_ShowNum(54, 8, Timer2TickAuto[0], 7, OLED_6X8);

      OLED_ShowString(0, 8, "WQ:", OLED_6X8);
      OLED_ShowString(18, 8, "00.00|", OLED_6X8); 
    }
    else if(UiIndex == UI_Level){
      OLED_ShowString(6, 0, "Air Pump Level", OLED_6X8);
      if(MenuLevel == 1) OLED_ShowString(0, 8, ">", OLED_6X8);
      OLED_ShowString(27, 8, GetAirPumpLevelStr(), OLED_6X8);     

      if(apl == AirPump_Low) Timer2Tick_F = 500 * 14;
      if(apl == AirPump_Med) Timer2Tick_F = 500 * 56;
      if(apl == AirPump_High) Timer2Tick_F = 500 * 112;    
    }
    else if(UiIndex == UI_RTC){
      OLED_ShowString(39, 0, "RTC", OLED_6X8);
      OLED_ShowString(24, 8, "00:00:00", OLED_6X8);
      OLED_ShowNum(24, 8, nTime.Hours, 2, OLED_6X8);
      OLED_ShowNum(42, 8, nTime.Minutes, 2, OLED_6X8);
      OLED_ShowNum(60, 8, nTime.Seconds, 2, OLED_6X8);        
    }
    else if(UiIndex == UI_Tiemr_Motor){
      OLED_ShowString(18, 0, "Timer Wash", OLED_6X8);
      if(MenuLevel == 1) OLED_ShowString(0, 8, ">", OLED_6X8);
      OLED_ShowString(24, 8, "00:00:00", OLED_6X8);
      OLED_ShowNum(24, 8, TimerMotor[0], 2, OLED_6X8);
      OLED_ShowNum(42, 8, TimerMotor[1], 2, OLED_6X8);
      OLED_ShowNum(60, 8, TimerMotor[2], 2, OLED_6X8);    

      if(TimerMotorIndex == 0 && TimerMotorTick_s % 2) OLED_ShowString(24, 8, "  ", OLED_6X8);
      if(TimerMotorIndex == 1 && TimerMotorTick_s % 2) OLED_ShowString(42, 8, "  ", OLED_6X8);
      if(TimerMotorIndex == 2 && TimerMotorTick_s % 2) OLED_ShowString(60, 8, "  ", OLED_6X8);
    }
    else if(UiIndex >= UI_Tiemr_AirPump && UiIndex <= UI_INDEX_MAX){
      OLED_ShowString(0, 0, "Timer AirPump   ", OLED_6X8);
      OLED_ShowString(0, 8, "CH-99 | 00:00:00", OLED_6X8);
      if(MenuLevel == 1) OLED_ShowString(36, 8, ">", OLED_6X8);
      OLED_ShowNum(18, 8, TimerAirPumpChannel, 2, OLED_6X8);
      OLED_ShowNum(48, 8, TimerAirPump[TimerAirPumpChannel][0], 2, OLED_6X8);
      OLED_ShowNum(66, 8, TimerAirPump[TimerAirPumpChannel][1], 2, OLED_6X8);
      OLED_ShowNum(84, 8, TimerAirPump[TimerAirPumpChannel][2], 2, OLED_6X8);  
      if(TimerAirPump[TimerAirPumpChannel][3] == 0) OLED_ShowString(84, 0, "L", OLED_6X8);
      if(TimerAirPump[TimerAirPumpChannel][3] == 1) OLED_ShowString(84, 0, "M", OLED_6X8);
      if(TimerAirPump[TimerAirPumpChannel][3] == 2) OLED_ShowString(84, 0, "H", OLED_6X8);

      if(TimerAirPumpIndex == 0 && TimerMotorTick_s % 2) OLED_ShowString(48, 8, "  ", OLED_6X8);
      if(TimerAirPumpIndex == 1 && TimerMotorTick_s % 2) OLED_ShowString(66, 8, "  ", OLED_6X8);
      if(TimerAirPumpIndex == 2 && TimerMotorTick_s % 2) OLED_ShowString(84, 8, "  ", OLED_6X8);
      if(TimerAirPumpIndex == 3 && TimerMotorTick_s % 2) OLED_ShowString(84, 0, "  ", OLED_6X8);
    }
    else OLED_ShowString(0, 0, ">No UI Data", OLED_8X16);

    //500ms -> 9mL
    if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) == GPIO_PIN_RESET && Timer2Tick >= Timer2Tick_F && TimerAirPumpManuallyFlag){
      Timer2Tick = 0;
      
      HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_SET);
    }
    if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin)) Timer2Tick = 0;

    if(TimerMotorTick_s == TimerMotorTick_s_End && TimerMotorTick_s_End != 0){
      if(UiIndex == UI_Main) {
        HAL_GPIO_WritePin(Motor_GPIO_Port, Motor_Pin, GPIO_PIN_RESET);
        TimerMotorTick_s_Run = TimerMotorTick_s + 5;
      }
    }
    if(TimerMotorTick_s == TimerMotorTick_s_Run){
      HAL_GPIO_WritePin(Motor_GPIO_Port, Motor_Pin, GPIO_PIN_SET);
      TimerMotorTick_s_End = TimerMotorTick_s + TimerMotor[0] * 3600 + TimerMotor[1] * 60 + TimerMotor[2];
    }

    for(uint8_t i = 0;i < TIMER_AIR_PUMP_CHANNEL_MAX;i++){
      if(nTime.Hours == TimerAirPump[i][0] && nTime.Minutes == TimerAirPump[i][1] && nTime.Seconds == TimerAirPump[i][2] && TimerAirPumpManuallyFlag == 0){
        if(UiIndex <= UI_Tiemr_Motor) {
          if(TimerAirPump[i][3] == AirPump_Low) Timer2TickAuto_F[i] = 500 * 14;
          if(TimerAirPump[i][3] == AirPump_Med) Timer2TickAuto_F[i] = 500 * 56;
          if(TimerAirPump[i][3] == AirPump_High) Timer2TickAuto_F[i] = 500 * 112;            
          // HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_RESET);
          TimerAirPumpAutoRunTask[i] = 1;
        }
      }
      if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin) == GPIO_PIN_RESET && Timer2TickAuto[i] >= Timer2TickAuto_F[i] && TimerAirPumpManuallyFlag == 0){
        Timer2TickAuto[i] = 0;
        
        // HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_SET);
        TimerAirPumpAutoRunTask[i] = 0;
      }    
      if(HAL_GPIO_ReadPin(QiBeng_GPIO_Port, QiBeng_Pin)) Timer2TickAuto[i] = 0;
    }
    for(uint8_t i = 0;i < TIMER_AIR_PUMP_CHANNEL_MAX;i++){
      if(TimerAirPumpAutoRunTask[i]){
        HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_RESET);
        break;
      }
    }
    for(uint8_t i = 0;i < TIMER_AIR_PUMP_CHANNEL_MAX;i++){
      if(TimerAirPumpAutoRunTask[i] == 0){
        if(i == 5) HAL_GPIO_WritePin(QiBeng_GPIO_Port, QiBeng_Pin, GPIO_PIN_SET);
      }
      else break;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
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
  else if(apl == AirPump_Med) return "Medium ";
  else return "  High ";
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) { // �ж��Ƿ�Ϊ TIM2 ���ж�
    Timer2Tick++;
    UILevelCloseTick++;
    Timer2CounterTick++;
    for(uint8_t i = 0;i < TIMER_AIR_PUMP_CHANNEL_MAX;i++) Timer2TickAuto[i]++;
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
