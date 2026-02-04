#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

// Frame struct
// AA 55 XX XX XX XX 55 AA

//Reading sensor value
#define DRIVER_FIRE 0x01
#define DRIVER_SMOKE 0x02
#define DRIVER_TEMPTRUE 0x03
#define DRIVER_HUMIDITY 0x04
#define DRIVER_INFRARED_RANGING 0x05
//Setting sensor warn value
#define SET_FIRE_WARN 0x11
#define SET_SMOKE_WARN 0x12
#define SET_TEMPTRUE_WARN 0x13
#define SET_HUMIDITY_WARN 0x14
#define SET_INFRARED_RANGING_WARN 0x15
//Ctrl driver
#define CTL_FIRE_RELAY 0x21
#define CTL_WIND_RELAY 0x22
#define CTL_WINDO_MOTOR 0x23
#define CTL_COLD_RELAY 0x24
#define CTL_DRY_RELAY 0x25
#define CTL_BUZZER_DRIVER 0x26
#define CTL_AUDIOPLAY_DRIVER 0x27

//Programmer run mode
#define CTL_RUN_MODE 0xFF

extern uint8_t SerialFeedback;

void UART4_Init(uint32_t baudrate);
void UART4_NVIC_Configuration(void);
void UART4_EnableRxInterrupt(void);
void USART1_Init_Debug(uint32_t baudrate);
void USART_SendString(USART_TypeDef* USARTx, char* str);
uint8_t UART4_ReceiveByte(void);
void UART4_SendByte(uint8_t data);
void UART4_SendString(char* str);
void WiFi_UDP_GetData(void);
void WiFi_UDP_SendByte(uint8_t byte);
void WiFi_UDP_SendFrame(uint8_t UUID, uint8_t DataLen, uint8_t Data);
void DecodeFrameData(void);

#endif /* __SERIAL_H */