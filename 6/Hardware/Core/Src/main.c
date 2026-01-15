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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "OLED.h"
#include "PC_Frame.h"
#include "string.h"
#include "MQ.h"
#include "DHT11.h"
#include "WiFi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_CONV 0.0008
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t Serial_RxData[256];
uint8_t System_Run_Flag = 0;
// uint8_t Warn_Value = 0;

uint8_t RxBuff[1];      //ï¿½ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½ï¿½ï¿½ï¿½
uint8_t DataBuff[256]; //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½ï¿½ï¿½ï¿???????????????????
uint8_t RxLine=0;           //ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý³ï¿½ï¿½ï¿½

uint8_t RxBuff_3[1];      //ï¿½ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½ï¿½ï¿½ï¿½
uint8_t DataBuff_3[256]; //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½ï¿½ï¿½ï¿½ï¿???????????????????
uint8_t RxLine_3=0;           //ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý³ï¿½ï¿½ï¿½

uint16_t CO_Value = 0;     //Ò»ï¿½ï¿½ï¿½ï¿½Ì¼
uint16_t Fire_Value = 0;   //ï¿½ï¿½È¼ï¿½ï¿½ï¿½ï¿½
uint16_t Air_Level = 0;    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
uint16_t Temptrue_Value = 0;     //ï¿½Â¶ï¿½

uint16_t CO_Warn_Value = 50;
uint16_t Fire_Warn_Value = 30;   //ï¿½ï¿½È¼ï¿½ï¿½ï¿½ï¿½
uint16_t Air_Warn_Level = 310;    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
uint16_t Temptrue_Warn_Value = 0;     //ï¿½Â¶ï¿½

uint16_t TIM_Counter = 0;
uint8_t CounterFlag = 0;
uint32_t SystemTick = 0;

uint8_t DisplayRunFlag = 1;
uint8_t GetSensorFlag = 1;
uint8_t SendDataFlag = 1;

float f_Temptrue_Value = 0.0;
float f_humi_Value = 0.0;

char *ipaddress;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART4_Init(void);
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  printf("Initialize all configured peripherals!\r\n");
  OLED_Init();
  printf("OLED has init!\r\n");
  // DHT11_Init();
  // printf("DHT11 has init!\r\n");
  Calibrate_MQ9();
  printf("MQ9 has Calibrate!\r\n");
  Calibrate_MQ135();
  printf("MQ135 has Calibrate!\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)RxBuff, 1); //ï¿½ò¿ª´ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½ï¿½
  printf("USART1 rx has init!\r\n");
  HAL_UART_Transmit(&huart3, "Hand\r\n", 6, HAL_MAX_DELAY);
  HAL_Delay(10);
  HAL_UART_Transmit(&huart3, "Read\r\n", 6, HAL_MAX_DELAY);
  HAL_Delay(10);
  HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuff_3, 1); //ï¿½ò¿ª´ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½ï¿½
  printf("USART3 rx has init!\r\n");
  
  HAL_ADC_Start_IT(&hadc1);
  printf("ADC has init!\r\n");
  ESP01S_Init(&huart4);
  printf("ESP-01S has init!\r\n");
  // ÉèÖÃÊý¾Ý½ÓÊÕ»Øµ÷
  ESP01S_SetDataCallback(ESP01S_DataReceived);
  // HAL_TIM_Base_Start_IT(&htim2);
  // printf("TIM has init!\r\n");
  // HAL_TIM_Base_Start(&htim1);

    if (ESP01S_Test() == ESP01S_OK)
    {
        printf("ESP-01S Ready!\r\n");
    }

    if (ESP01S_ConnectAP("3BC4E533", "12345678") == ESP01S_OK)
    {
        printf("ESP-01S Connect Wi-Fi!\r\n");
    }    
    if (ESP01S_GetLocalIP(ipaddress) == ESP01S_OK)
    {
        printf("ESP-01S ipaddress=%s\r\n", ipaddress);
    }        

    switch (ESP01S_StartUDP("192.168.137.1", 8080, 8080, 2, 0))
    {
      case ESP01S_OK:
          printf("ESP-01S Start UDP!\r\n");
          
        break;
      case ESP01S_ERROR:
          printf("ESP-01S Start UDP ERROR!\r\n");
        break;
      case ESP01S_TIMEOUT:
          printf("ESP-01S Start UDP TIMEOUT!\r\n");
        break;
      case ESP01S_BUSY:
          printf("ESP-01S Start UDP BUSY!\r\n");
        break;                        
      
      default:
        break;
    }

 
    

  while (1)
  {
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    SystemTick %= 10000000;
    if(CO_Value > CO_Warn_Value || Fire_Value > Fire_Warn_Value || Air_Level > Air_Warn_Level) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, (GPIO_PinState)RESET);
    else HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, (GPIO_PinState)SET);

    

    if(GetSensorFlag){
      CO_Value = MQ_9();
      Fire_Value = MQ_9();
      Air_Level = MQ_135();
      // GetSensorFlag = 0;
    }

    Receive_PC_Value(DataBuff, 256);
    // OLED_ShowHexNum(16, 0, System_Run_Flag++, 2, OLED_8X16);
    if(DisplayRunFlag){
      OLED_ShowString(0, 0, "CO=", OLED_8X16);
      OLED_ShowNum(32, 0, CO_Value, 4, OLED_8X16);
      OLED_ShowString(64, 0, "FR=", OLED_8X16);
      OLED_ShowNum(96, 0, Fire_Value, 4, OLED_8X16);
      OLED_ShowString(0, 16, "AL=", OLED_8X16);
      OLED_ShowNum(32, 16, Air_Level, 4, OLED_8X16);
      OLED_ShowString(64, 16, "TE=", OLED_8X16);
      OLED_ShowNum(96, 16, Temptrue_Value, 4, OLED_8X16);      

      OLED_ShowString(0, 32, "WC=", OLED_8X16);
      OLED_ShowNum(32, 32, CO_Warn_Value, 4, OLED_8X16);
      OLED_ShowString(64, 32, "WF=", OLED_8X16);
      OLED_ShowNum(96, 32, Fire_Warn_Value, 4, OLED_8X16);
      OLED_ShowString(0, 48, "WA=", OLED_8X16);
      OLED_ShowNum(32, 48, Air_Warn_Level, 4, OLED_8X16);
      OLED_ShowString(64, 48, "WT=", OLED_8X16);
      OLED_ShowNum(96, 48, Temptrue_Warn_Value, 4, OLED_8X16);           
      OLED_Update();
      OLED_Clear();
      // DisplayRunFlag = 0;
    }
    if(SendDataFlag){
      Send_Driver_Info(DRIVER_MQ_9);
      HAL_Delay(200);
      Send_Driver_Info(DRIVER_Fire);
      HAL_Delay(200);
      Send_Driver_Info(DRIVER_MQ_135);
      HAL_Delay(200);
      Send_Driver_Info(DRIVER_DHT11);
      HAL_Delay(200);
      // SendDataFlag = 0;
    }

    // ´¦Àí½ÓÊÕÊý¾Ý
    ESP01S_ProcessBuffer();
    
    // ¼ì²éÊÇ·ñÓÐÊý¾Ý¿ÉÓÃ£¨ÂÖÑ¯·½Ê½£©
    if (ESP01S_HasData())
    {
        uint8_t buffer[256];
        uint16_t len = ESP01S_ReadData(buffer, sizeof(buffer));
        
        printf("[Polling] Received %d bytes: ", len);
        for (uint16_t i = 0; i < len && i < 32; i++)
        {
          DataBuff[i] = buffer[i];

          printf("%02X ", buffer[i]);
        }
        printf("\r\n");
    }
    
    // // ¶¨Ê±·¢ËÍÊý¾Ý£¨Ã¿5Ãë£©
    // static uint32_t last_send_time = 0;
    // if (HAL_GetTick() - last_send_time > 5000)
    // {
    //     char timestamp[32];
    //     snprintf(timestamp, sizeof(timestamp), "Time: %lu", HAL_GetTick());
        
    //     ESP01S_Status status = ESP01S_SendDataWithResult(0, timestamp, strlen(timestamp), 1000);
    //     if (status == ESP01S_OK)
    //     {
    //         printf("Sent: %s\r\n", timestamp);
    //     }
        
    //     last_send_time = HAL_GetTick();
    // }    
    // DHT11_Get_Values(&f_Temptrue_Value, &f_humi_Value);
    // DHT11_Print_Debug_Info();
    
    // printf("SystemTick = %d\r\n", SystemTick);
    // printf("f_Temptrue_Value = %f\r\n", f_Temptrue_Value);
    // printf("f_humi_Value = %f\r\n", f_humi_Value);
    // HAL_UART_Transmit(&huart3, "Auto\r\n", 6, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart3, "Read\r\n", 6, HAL_MAX_DELAY);
    HAL_Delay(10);  
    // if(DataBuff_3[0] != 'R') {
    //   RxLine_3 = 0;
    // }      
    if(RxLine_3 >= 16){
      for(uint8_t i = 0;i < 5;i++) printf("%c",DataBuff_3[i]);
      if(DataBuff_3[0] >= '0' && DataBuff_3[0] <= '9') Temptrue_Value = (DataBuff_3[0] - '0') * 10;
      if(DataBuff_3[1] >= '0' && DataBuff_3[1] <= '9') Temptrue_Value += (DataBuff_3[1] - '0');
      if(DataBuff_3[2] == '.')
      if(DataBuff_3[3] >= '0' && DataBuff_3[3] <= '9')
      if(DataBuff_3[4] == 'C');
      // printf("Temptrue_Value = %d\r\n", Temptrue_Value);
    }     
    
    // HAL_Delay(1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OLED_SCL_Pin|OLED_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_SCL_Pin OLED_SDA_Pin */
  GPIO_InitStruct.Pin = OLED_SCL_Pin|OLED_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
  if(UartHandle->Instance == USART1){
    RxLine++;                      //Ã¿ï¿½ï¿½ï¿½Õµï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý£ï¿½ï¿½ï¿½ï¿½ï¿½Øµï¿½ï¿½ï¿½ï¿½Ý³ï¿½ï¿½È¼ï¿???????????????????1
    DataBuff[RxLine-1]=RxBuff[0];  //ï¿½ï¿½Ã¿ï¿½Î½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý±ï¿½ï¿½æµ½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    RxBuff[0]=0;
    HAL_UART_Receive_IT(&huart1, (uint8_t *)RxBuff, 1); //Ã¿ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý£ï¿½ï¿½Í´ï¿½Ò»ï¿½Î´ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½Õ£ï¿½ï¿½ï¿½ï¿½ï¿½Ö»ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý¾ï¿½Í£Ö¹ï¿½ï¿½ï¿½ï¿???????????????????
  }
  if(UartHandle->Instance == USART3){
    RxLine_3++;                      //Ã¿ï¿½ï¿½ï¿½Õµï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý£ï¿½ï¿½ï¿½ï¿½ï¿½Øµï¿½ï¿½ï¿½ï¿½Ý³ï¿½ï¿½È¼ï¿???????????????????1
    DataBuff_3[RxLine_3-1]=RxBuff_3[0];  //ï¿½ï¿½Ã¿ï¿½Î½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý±ï¿½ï¿½æµ½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    RxLine_3 %= 17;
    RxBuff_3[0]=0;
    HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuff_3, 1); //Ã¿ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý£ï¿½ï¿½Í´ï¿½Ò»ï¿½Î´ï¿½ï¿½ï¿½ï¿½Ð¶Ï½ï¿½ï¿½Õ£ï¿½ï¿½ï¿½ï¿½ï¿½Ö»ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½Ý¾ï¿½Í£Ö¹ï¿½ï¿½ï¿½ï¿???????????????????
  }
  HAL_UART_RxCpltCallback_WiFi(&huart4);

}

void Clear_ReceiveBuff(){
  for(uint8_t i = 0;i < 255;i++) DataBuff[i] = 0x00;
  RxLine = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2){
    SystemTick++;
    if(SystemTick == TIM_Counter) HAL_TIM_Base_Stop_IT(&htim2);
  }
}

uint8_t Read_Temperature(void){
    return Temptrue_Value;
}

// »Øµ÷º¯ÊýÊµÏÖÊ¾Àý
void ESP01S_DataReceived(uint8_t link_id, uint8_t *data, uint16_t length)
{
    // link_id: Á¬½ÓID£¨¶àÁ¬½ÓÄ£Ê½ÏÂÓÐÐ§£©
    // data: ½ÓÊÕµ½µÄÊý¾ÝÖ¸Õë
    // length: Êý¾Ý³¤¶È
    
    // printf("Received from link %d, length: %d\n", link_id, length);
    
    // ´¦ÀíÊý¾Ý...
    if (length > 0)
    {
        // printf("Data: ");
        for (uint16_t i = 0; i < length; i++)
        {
            // ÒÔ16½øÖÆºÍASCIIÁ½ÖÖ¸ñÊ½ÏÔÊ¾
            if (data[i] >= 32 && data[i] <= 126) // ¿É´òÓ¡×Ö·û
            {
                // printf("%c", data[i]);
            }
            else
            {
                // printf("[0x%02X]", data[i]);
            }
        }
        // printf("\n");
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
