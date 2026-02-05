#include "stm32f10x.h"
#include "Serial.h"
#include <string.h>
#include <stdio.h>
#include "Delay.h"
#include "Sensor.h"

// #define DEBUG_PRINT printf("Frame_DataBuff[%d] = %X\r\n", Frame_Data_Index, Frame_DataBuff[Frame_Data_Index]);
#define DEBUG_PRINT

uint8_t RxDataBuffIndex = 0;
uint16_t RxDataBuff[256];

uint8_t FrameDecodeIndex = 0;
uint8_t Frame_DataLenght = 0;
uint8_t Frame_DataBuff[20] = {0};
uint8_t Frame_Data_Index = 0;

uint8_t SerialFeedback = 1;

/**
  * @brief  针对8MHz时钟的UART4波特率计算
  * @param  baudrate: 期望的波特率
  * @retval BRR寄存器值
  */
static uint16_t UART4_CalculateBRR_8MHz(uint32_t baudrate)
{
    float fPCLK1 = 8000000.0f;  // APB1时钟为8MHz
    float fDiv = fPCLK1 / (16.0f * baudrate);
    uint16_t div_int = (uint16_t)fDiv;
    uint16_t div_fra = (uint16_t)((fDiv - div_int) * 16.0f + 0.5f);
    
    return (div_int << 4) | div_fra;
}

/**
  * @brief  获取8MHz时钟下的标准波特率BRR值
  * @param  baudrate: 波特率
  * @retval BRR寄存器值
  */
static uint16_t UART4_GetStandardBRR_8MHz(uint32_t baudrate)
{
    switch(baudrate)
    {
        case 9600:   return 0x0341;    // 实际波特率: 9600.00
        case 19200:  return 0x01A1;    // 实际波特率: 19200.00
        case 38400:  return 0x00D0;    // 实际波特率: 38461.54 (有误差)
        case 57600:  return 0x008B;    // 实际波特率: 57692.31 (有误差)
        case 115200: return 0x0045;    // 实际波特率: 113636.36 (有-1.36%误差)
        case 230400: return 0x0022;    // 实际波特率: 250000.00 (有+8.51%误差)
        default:     return UART4_CalculateBRR_8MHz(baudrate);
    }
}

/**
  * @brief  初始化UART4（适配8MHz时钟）
  * @param  baudrate: 波特率
  * @retval None
  */
void UART4_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* 1. 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    
    /* 2. 配置GPIO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* 3. 配置串口参数 */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(UART4, &USART_InitStructure);
    
    /* 4. 调整波特率寄存器以适应8MHz时钟 */
    UART4->BRR = UART4_GetStandardBRR_8MHz(baudrate);
    
    /* 5. 使能串口 */
    USART_Cmd(UART4, ENABLE);
}

/**
  * @brief  初始化USART1用于调试输出
  * @param  baudrate: 波特率
  * @retval None
  */
void USART1_Init_Debug(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* 1. 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* 2. 配置GPIO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  // USART1_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 3. 配置串口参数 */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;  // 只发送
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(USART1, &USART_InitStructure);
    
    /* 4. 调整波特率寄存器以适应8MHz时钟 */
    // USART1在APB2总线上，时钟频率不同
    float fPCLK2 = 8000000.0f;  // APB2时钟为8MHz
    float fDiv = fPCLK2 / (16.0f * baudrate);
    uint16_t div_int = (uint16_t)fDiv;
    uint16_t div_fra = (uint16_t)((fDiv - div_int) * 16.0f + 0.5f);
    USART1->BRR = (div_int << 4) | div_fra;
    
    /* 5. 使能串口 */
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  配置UART4接收中断
  * @param  None
  * @retval None
  */
void UART4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 配置UART4中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能接收中断 */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  使能UART4接收中断
  * @param  None
  * @retval None
  */
void UART4_EnableRxInterrupt(void)
{
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  串口发送字符串
  * @param  usart: 串口
  * @param  str: 字符串
  * @retval None
  */
void USART_SendString(USART_TypeDef* USARTx, char* str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(USARTx, *str++);
    }
}

/**
  * @brief  UART4发送一个字节
  * @param  data: 要发送的数据
  * @retval None
  */
void UART4_SendByte(uint8_t data)
{
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    USART_SendData(UART4, data);
}

/**
  * @brief  UART4发送字符串
  * @param  str: 要发送的字符串
  * @retval None
  */
void UART4_SendString(char* str)
{
    while(*str)
    {
        UART4_SendByte(*str++);
    }
}

/**
  * @brief  UART4接收一个字节（轮询方式）
  * @param  None
  * @retval 接收到的数据
  */
uint8_t UART4_ReceiveByte(void)
{
    while(USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(UART4);
}

void WiFi_UDP_GetData(void){
    if(RxDataBuffIndex > 0){
        if(FrameDecodeIndex < 6){
            switch (RxDataBuff[FrameDecodeIndex])
            {
            case '+':
                FrameDecodeIndex++;
                break;
            case 'I':
                FrameDecodeIndex++;
                break;      
            case 'P':
                FrameDecodeIndex++;
                break;   
            case 'D':
                FrameDecodeIndex++;
                break;     
            case ',':
                FrameDecodeIndex++;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                Frame_DataLenght = RxDataBuff[FrameDecodeIndex] - '0';
                // printf("Frame_DataLenght = %d\r\n", Frame_DataLenght);
                FrameDecodeIndex++;
                FrameDecodeIndex++;
                break;
            case ':':
                FrameDecodeIndex++;
                // printf("FrameDecodeIndex = %d\r\n", FrameDecodeIndex);
                break;

            default:
                break;
            }
        }else{
            for(uint8_t i = 0;i < Frame_DataLenght;i++){
                Frame_DataBuff[i] = RxDataBuff[FrameDecodeIndex++];
                printf("%X ", Frame_DataBuff[i]);
            }
            printf("\r\n");
            FrameDecodeIndex = 0;
            RxDataBuffIndex = 0;
        }

    }
}

void DecodeFrameData(void){
    if(Frame_DataLenght > 0){
        // printf("Frame_DataLenght = %d\r\n", Frame_DataLenght);
        // DEBUG_PRINT
        switch (Frame_DataBuff[Frame_Data_Index])
        {
        case 0xAA:
            DEBUG_PRINT
            Frame_Data_Index++;
            break;
        case 0x55:
            DEBUG_PRINT
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //Spik data len
            break;
        case DRIVER_FIRE:
            Frame_Data_Index++;
            // printf("[DRIVER_FIRE]\r\n");
            WiFi_UDP_SendFrame(DRIVER_FIRE, 0x01, SensorValue.DriverFire);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case DRIVER_HUMIDITY:
            Frame_Data_Index++;
            // printf("[DRIVER_HUMIDITY]\r\n");
            WiFi_UDP_SendFrame(DRIVER_HUMIDITY, 0x01, SensorValue.DriverHumidity);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case DRIVER_INFRARED_RANGING:
            Frame_Data_Index++;
            // printf("[DRIVER_INFRARED_RANGING]\r\n");
            WiFi_UDP_SendFrame(DRIVER_INFRARED_RANGING, 0x01, SensorValue.DriverInfraredBeam);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case DRIVER_SMOKE:
            Frame_Data_Index++;
            // printf("[DRIVER_SMOKE]\r\n");
            WiFi_UDP_SendFrame(DRIVER_SMOKE, 0x01, SensorValue.DriverSmoke);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case DRIVER_TEMPTRUE:
            Frame_Data_Index++;
            // printf("[DRIVER_TEMPTRUE]\r\n");
            WiFi_UDP_SendFrame(DRIVER_TEMPTRUE, 0x01, SensorValue.DriverTemptrue);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case SET_FIRE_WARN:
            Frame_Data_Index++;
            // printf("[SET_FIRE_WARN] = %d\r\n", Frame_DataBuff[Frame_Data_Index]);
            SensorWarn.DriverFire = (uint16_t)(Frame_DataBuff[Frame_Data_Index] / 0.0625);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case SET_HUMIDITY_WARN:
            Frame_Data_Index++;
            // printf("[SET_HUMIDITY_WARN]\r\n");
            SensorWarn.DriverHumidity = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case SET_INFRARED_RANGING_WARN:
            Frame_Data_Index++;
            // printf("[SET_INFRARED_RANGING_WARN]\r\n");
            SensorWarn.DriverInfraredBeam = (uint16_t)(Frame_DataBuff[Frame_Data_Index] / 0.0625);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case SET_SMOKE_WARN:
            Frame_Data_Index++;
            // printf("[SET_SMOKE_WARN]\r\n");
            SensorWarn.DriverSmoke = (uint16_t)(Frame_DataBuff[Frame_Data_Index] / 0.0256);
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case SET_TEMPTRUE_WARN:
            Frame_Data_Index++;
            // printf("[SET_TEMPTRUE_WARN]\r\n");
            SensorWarn.DriverTemptrue = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_AUDIOPLAY_DRIVER:
            Frame_Data_Index++;
            // printf("[CTL_AUDIOPLAY_DRIVER]\r\n");
            CtrlDriver.Driver_AudioPlay = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;          
        case CTL_BUZZER_DRIVER:
            Frame_Data_Index++;
            // printf("[CTL_BUZZER_DRIVER]\r\n");
            CtrlDriver.Driver_Buzzer = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_COLD_RELAY:
            Frame_Data_Index++;
            // printf("[CTL_COLD_RELAY]\r\n");
            CtrlDriver.Driver_Relay_1 = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_DRY_RELAY:
            Frame_Data_Index++;
            // printf("[CTL_DRY_RELAY]\r\n");
            CtrlDriver.Driver_Relay_2 = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_FIRE_RELAY:
            Frame_Data_Index++;
            // printf("[CTL_FIRE_RELAY]\r\n");
            CtrlDriver.Driver_Relay_3 = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_WIND_RELAY:
            Frame_Data_Index++;
            // printf("[CTL_WIND_RELAY]\r\n");
            CtrlDriver.Driver_Relay_4 = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_WINDO_MOTOR:
            Frame_Data_Index++;
            // printf("[CTL_WINDO_MOTOR]\r\n");
            CtrlDriver.Driver_Motor = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit            
            break;        
        case CTL_RUN_MODE:
            Frame_Data_Index++;
            // printf("[CTL_RUN_MODE]\r\n");
            Run_Mode = Frame_DataBuff[Frame_Data_Index];
            Frame_Data_Index++;
            DEBUG_PRINT
            Frame_Data_Index++; //CRC bit
            break;                                                                           
        
        default:
            break;
        }
        if(Frame_DataBuff[Frame_Data_Index] == 0x55 && Frame_Data_Index > 2){
            Frame_Data_Index++;
            if(Frame_DataBuff[Frame_Data_Index] == 0xAA){
                Frame_Data_Index = 0;
                Frame_DataLenght = 0;
                printf("Get frame done!\r\n");
            }
        }
    }
}

void WiFi_UDP_SendByte(uint8_t byte){
    UART4_SendString("AT+CIPSEND=1\r\n");
    Delay_ms(100);
    UART4_SendByte(byte);
    Delay_ms(100);
}

void WiFi_UDP_SendFrame(uint8_t UUID, uint8_t DataLen, uint8_t Data){
    WiFi_UDP_SendByte(0xaa);
    WiFi_UDP_SendByte(0x55);
    WiFi_UDP_SendByte(DataLen);
    WiFi_UDP_SendByte(UUID);
    WiFi_UDP_SendByte(Data);
    WiFi_UDP_SendByte((DataLen + UUID + Data) & 0xFF);
    WiFi_UDP_SendByte(0x55);
    WiFi_UDP_SendByte(0xaa);
}

/**
  * @brief  UART4中断服务函数
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
    uint8_t data;
    
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        /* 读取接收到的数据 */
        data = USART_ReceiveData(UART4);
        
        /* 这里可以处理接收到的数据 */
        RxDataBuff[RxDataBuffIndex++] = data;
        if(data == '\r' || data == '\n') RxDataBuffIndex = 0;
        if(SerialFeedback) USART_SendData(USART1, data);
        // ... 你的处理代码 ...
        
        /* 清除中断标志 */
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
    
    /* 其他中断处理 */
    if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
    {
        /* 发送中断处理 */
        USART_ClearITPendingBit(UART4, USART_IT_TXE);
    }
    
    if(USART_GetITStatus(UART4, USART_IT_ORE) != RESET)
    {
        /* 过载错误处理 */
        USART_ClearITPendingBit(UART4, USART_IT_ORE);
    }
}