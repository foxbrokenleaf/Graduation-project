/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>
#include "oled.h"
#include "TIM.h"
#include "Buzzer.h"
#include "Motor.h"
#include "Relay.h"
#include "Sensor.h"
#include "DHT11.h"
#include "Serial.h"
#include "AudioPlay.h"
// #include "WiFi.h"
// #include "PC_Frame.h"
#include "Delay.h"

#ifdef USE_STM32100B_EVAL
 #include "stm32100b_eval_lcd.h"
#elif defined USE_STM3210B_EVAL
 #include "stm3210b_eval_lcd.h"
#elif defined USE_STM3210E_EVAL
//  #include "stm3210e_eval_lcd.h" 
#elif defined USE_STM3210C_EVAL
 #include "stm3210c_eval_lcd.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval_lcd.h"
#endif

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef USE_STM32100B_EVAL
  #define MESSAGE1   "STM32 MD Value Line " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "  STM32100B-EVAL    " 
#elif defined (USE_STM3210B_EVAL)
  #define MESSAGE1   "STM32 Medium Density" 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210B-EVAL    " 
#elif defined (STM32F10X_XL) && defined (USE_STM3210E_EVAL)
  #define MESSAGE1   "  STM32 XL Density  " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210E-EVAL    "
#elif defined (USE_STM3210E_EVAL)
  #define MESSAGE1   " STM32 High Density " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210E-EVAL    " 
#elif defined (USE_STM3210C_EVAL)
  #define MESSAGE1   " STM32 Connectivity " 
  #define MESSAGE2   " Line Device running" 
  #define MESSAGE3   " on STM3210C-EVAL   "
#elif defined (USE_STM32100E_EVAL)
  #define MESSAGE1   "STM32 HD Value Line " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "  STM32100E-EVAL    "   
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 USART_InitTypeDef USART_InitStructure;
 DHT11_Data dht11_dat;
 sensor_ main_sensor;

 uint8_t Motor_Test = 0;
 uint8_t TickSec = 0;
 uint8_t DisplayTaskRunFlag = 0;
 uint8_t SerialTaskRunFlag = 0;
 uint8_t BuzzerTaskRunFlag = 0;
 uint8_t SensorTaskRunFlag = 0;
 uint8_t WiFiSendTaskRunFlag = 0;
 uint8_t RelayTaskRunFlag = 0;
 uint8_t AudioPlayTaskRunFlag = 0;
 uint8_t RunModeTaskRunFlag = 0;
 uint8_t WarnCheckTaskRunFlag = 0;

 uint8_t SensorSendIndex = 0;

 uint8_t BuzzerFlag = 0;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
void DisplayTask(void);
void TaskExec(void);
void SerialTask(void);
void MotorTask(void);
void BuzzerTask(void);
void SensorTask(void);
void RelayTask(void);
void AudioplayTask(void);
void RunModeTask(void);
void WarnCheckTask(void);
void WiFiRecevTask(void);
void WiFiSendTask(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	
	RCC_ClocksTypeDef tmp;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

  /* Initialize LEDs, Key Button, LCD and COM port(USART) available on
     STM3210X-EVAL board ******************************************************/

	OLED_Init();
  MyTIM_Init();
  Motor_Init();
  DHT11_Init();
  Buzzer_Init();
  Relay_Init();
  AudioPlay_Init();
  UART4_Init(115200);
  UART4_NVIC_Configuration();
  UART4_EnableRxInterrupt();
  
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Retarget the C library printf function to the USARTx, can be USART1 or USART2
     depending on the EVAL board you are using ********************************/
  printf("\n\r %s", MESSAGE1);
  printf(" %s", MESSAGE2);
  printf(" %s\n\r", MESSAGE3);
	
	RCC_GetClocksFreq(&tmp);
	printf("SYSCLK_Frequency = %d\n\r ", tmp.SYSCLK_Frequency);
	OLED_ShowString(0, 0, "SYSCLK=", OLED_6X8);
	OLED_ShowNum(42, 0, tmp.SYSCLK_Frequency, 9, OLED_6X8);
	OLED_ShowString(0, 8, "PCLK1=", OLED_6X8);
	OLED_ShowNum(36, 8, tmp.PCLK1_Frequency, 9, OLED_6X8);  
	OLED_ShowString(0, 16, "PCLK2=", OLED_6X8);
	OLED_ShowNum(36, 16, tmp.PCLK2_Frequency, 9, OLED_6X8);  
	OLED_ShowString(0, 24, "HCLK=", OLED_6X8);
	OLED_ShowNum(30, 24, tmp.HCLK_Frequency, 9, OLED_6X8);  
	OLED_ShowString(0, 32, "ADCCLK=", OLED_6X8);
	OLED_ShowNum(42, 32, tmp.ADCCLK_Frequency, 9, OLED_6X8);        
	OLED_Update();
	

  /* Turn on leds available on STM3210X-EVAL **********************************/


  /* Add your application code here
     */

  /* Infinite loop */

  UART4_SendString("AT\r\n");
  Delay_ms(100);
  UART4_SendString("AT+GMR\r\n");
  Delay_ms(100);
  UART4_SendString("AT+CWMODE?\r\n");
  Delay_ms(100);
  UART4_SendString("AT+CWMODE=1\r\n");
  Delay_ms(100);  
  //AT+CWLAP
  UART4_SendString("AT+CWLAP\r\n");
  Delay_ms(5000);
  //AT+CWJAP="SSID","password"
  UART4_SendString("AT+CWJAP=\"3BC4E533\",\"12345678\"\r\n");
  Delay_ms(10000);
  //AT+CIPSTA?
  UART4_SendString("AT+CIPSTA?\r\n");
  Delay_ms(5000);  
  //AT+PING="202.38.64.5"
  UART4_SendString("AT+PING=\"192.168.137.1\"\r\n");
  Delay_ms(5000); 
  //AT+CIPSTART="UDP","192.168.101.110",1000,1002,2
  UART4_SendString("AT+CIPSTART=\"UDP\",\"192.168.137.1\",8080,8080,2\r\n");
  Delay_ms(1000);   

  WiFi_UDP_SendFrame(DRIVER_TEMPTRUE, 0x01, 0xff);

  SerialFeedback = 0;

  while (1)
  {

		if(time >= 2000){
      time = 0;
      TickSec = !TickSec;
      SerialTaskRunFlag++;
      DisplayTaskRunFlag = 1;
      BuzzerTaskRunFlag = 1;
      SensorTaskRunFlag = 1;
      RelayTaskRunFlag = 1;
      AudioPlayTaskRunFlag = 1;
      RunModeTaskRunFlag = 1;
      WarnCheckTaskRunFlag = 1;
      WiFiSendTaskRunFlag++;
    }
    if(SerialTaskRunFlag > 5){
      SerialTaskRunFlag = 0;
      SerialTask();
      SensorSendIndex++;
      SensorSendIndex %= 5;
    }
    if(DisplayTaskRunFlag){
      DisplayTaskRunFlag = 0;
      DisplayTask();
    }
    if(BuzzerTaskRunFlag){
      BuzzerTaskRunFlag = 0;
      BuzzerTask();
    }
    if(SensorTaskRunFlag){
      SensorTaskRunFlag = 0;
      SensorTask();
    }
    if(WiFiSendTaskRunFlag > 5){
      WiFiSendTaskRunFlag = 0;
      WiFiSendTask();
    }
    if(RelayTaskRunFlag){
      RelayTaskRunFlag = 0;
      RelayTask();
    }
    if(AudioPlayTaskRunFlag){
      AudioPlayTaskRunFlag = 0;
      AudioplayTask();
    }
    if(RunModeTaskRunFlag){
      RunModeTaskRunFlag = 0;
      RunModeTask();
    }
    if(WarnCheckTaskRunFlag){
      WarnCheckTaskRunFlag = 0;
      WarnCheckTask();
    }


    if(updata_flag) MotorTask();

    WiFiRecevTask();
    
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

void DisplayTask(void){

  OLED_Printf(0, 0, OLED_6X8, "T = %02d H = %02d", SensorValue.DriverTemptrue, SensorValue.DriverHumidity);
  OLED_Printf(0, 8, OLED_6X8, "S = %05d F = %04d", SensorValue.DriverSmoke, SensorValue.DriverFire);
  OLED_Printf(0, 16, OLED_6X8, "IR = %01d", SensorValue.DriverInfraredBeam);

  OLED_Printf(0, 24, OLED_6X8, "T! = %02d H! = %02d", SensorWarn.DriverTemptrue, SensorWarn.DriverHumidity);
  OLED_Printf(0, 32, OLED_6X8, "S! = %05d F! = %04d", SensorWarn.DriverSmoke, SensorWarn.DriverFire);
  OLED_Printf(0, 40, OLED_6X8, "IR! = %01d", SensorWarn.DriverInfraredBeam);
  
  OLED_Printf(0, 48, OLED_6X8, "B = %01d M = %01d FFT = %01d", CtrlDriver.Driver_Buzzer, CtrlDriver.Driver_Motor, CtrlDriver.Driver_Relay_3);
  OLED_Printf(0, 56, OLED_6X8, "W = %01d C = %01d D = %01d", CtrlDriver.Driver_Relay_4, CtrlDriver.Driver_Relay_1, CtrlDriver.Driver_Relay_2);

  OLED_Update();
  OLED_Clear();
}

void BuzzerTask(void){
  if(CtrlDriver.Driver_Buzzer){
    BuzzerFlag = !BuzzerFlag;
    if(!BuzzerFlag) Buzzer_Rang();
    else Buzzer_Mute();
  }else{
    Buzzer_Mute();
  }
}

void SensorTask(void){
  DHT11_ReadData(&dht11_dat);

  SensorValue.DriverHumidity = dht11_dat.humi_int;
  SensorValue.DriverTemptrue = dht11_dat.temp_int;

  SensorValue.DriverSmoke = ADC_ReadMQ2Sensor();
  SensorValue.DriverFire = ADC_ReadFireSensor();
  SensorValue.DriverInfraredBeam = ReadInfraredRangingSensor();
}

void MotorTask(void){
  if(CtrlDriver.Driver_Motor) Motor_Test = ANG_180;
  else Motor_Test = ANG_0;

  updata_flag = 0;
  switch (Motor_Test)
  {
  case 0:
    Motor_Turn(ANG_0);
    break;

  case 1:
    Motor_Turn(ANG_45);
    break;

  case 2:
    Motor_Turn(ANG_90);
    break;
  
  case 3:
    Motor_Turn(ANG_145);
    break;
  
  case 4:
    Motor_Turn(ANG_180);
    break;
      
  default:
    Motor_Test = 0;
    break;
  }
}

void RelayTask(void){
  if(CtrlDriver.Driver_Relay_1) Relay_Open(RELAY_CLOD);
  else Relay_Close(RELAY_CLOD);

  if(CtrlDriver.Driver_Relay_2) Relay_Open(RELAY_DRY);
  else Relay_Close(RELAY_DRY);
  
  if(CtrlDriver.Driver_Relay_3) Relay_Open(RELAY_FIRE);
  else Relay_Close(RELAY_FIRE);
  
  if(CtrlDriver.Driver_Relay_4) Relay_Open(RELAY_WIND);
  else Relay_Close(RELAY_WIND);  
}

void AudioplayTask(void){
  if(CtrlDriver.Driver_AudioPlay) AudioPlay_Play00001();
  // AudioPlay_Play00002();
}

void RunModeTask(void){
  // if(Run_Mode){
  //   CtrlDriver.Driver_AudioPlay = 1;
  //   CtrlDriver.Driver_Buzzer = 1;
  //   CtrlDriver.Driver_Led = 1;
  //   CtrlDriver.Driver_Motor = 1;
  //   CtrlDriver.Driver_Relay_1 = 1;
  //   CtrlDriver.Driver_Relay_2 = 1;
  //   CtrlDriver.Driver_Relay_3 = 1;
  //   CtrlDriver.Driver_Relay_4 = 1;
  // }
}

void SerialTask(void){
  // printf("SerialTask\r\n");
  // Send_Driver_Info(DRIVER_FIRE);
  // 
  if(Run_Mode == 1){
    printf("RUN MODE = DEBUG\r\n");
  }
  else{
    if(SensorSendIndex == 0) WiFi_UDP_SendFrame(DRIVER_FIRE, 0x01, SensorValue.DriverFire);
    if(SensorSendIndex == 1) WiFi_UDP_SendFrame(DRIVER_HUMIDITY, 0x01, SensorValue.DriverHumidity);
    if(SensorSendIndex == 2) WiFi_UDP_SendFrame(DRIVER_INFRARED_RANGING, 0x01, SensorValue.DriverInfraredBeam);
    if(SensorSendIndex == 3) WiFi_UDP_SendFrame(DRIVER_SMOKE, 0x01, SensorValue.DriverSmoke);
    if(SensorSendIndex == 4) WiFi_UDP_SendFrame(DRIVER_TEMPTRUE, 0x01, SensorValue.DriverTemptrue);
  }
  
}

void WarnCheckTask(void){
  if(Run_Mode == 0){
    if(SensorValue.DriverFire >= SensorWarn.DriverFire) CtrlDriver.Driver_Relay_3 = 1;
    else CtrlDriver.Driver_Relay_3 = 0;
    
    if(SensorValue.DriverSmoke >= SensorWarn.DriverSmoke){
      CtrlDriver.Driver_Motor = 1;
      CtrlDriver.Driver_Relay_4 = 1;
    }else{
      CtrlDriver.Driver_Motor = 0;
      CtrlDriver.Driver_Relay_4 = 0;    
    }
    if((SensorValue.DriverFire >= SensorWarn.DriverFire) || (SensorValue.DriverSmoke >= SensorWarn.DriverSmoke)) CtrlDriver.Driver_Buzzer = 1;
    else CtrlDriver.Driver_Buzzer = 0;
    if(SensorValue.DriverTemptrue >= SensorWarn.DriverTemptrue) CtrlDriver.Driver_Relay_1 = 1;
    else CtrlDriver.Driver_Relay_1 = 0;
    if(SensorValue.DriverHumidity >= SensorWarn.DriverHumidity) CtrlDriver.Driver_Relay_2 = 1;
    else CtrlDriver.Driver_Relay_2 = 0;
    if(SensorValue.DriverInfraredBeam == 1) CtrlDriver.Driver_AudioPlay = 1;
    else CtrlDriver.Driver_AudioPlay = 0;
  }
}

void WiFiSendTask(void){
  
}

void WiFiRecevTask(void){
  WiFi_UDP_GetData();
  DecodeFrameData();
}

void TaskExec(void){

  if(TickSec){

    TickSec = 0;

    
    // Motor_Test++;                                   
  }


  
  
}

