#ifndef __REALY_H__
#define __REALY_H__

#include "stm32f10x.h"

#define RELAY_1 GPIO_Pin_12
#define RELAY_1_PORT GPIOE
#define RELAY_2 GPIO_Pin_13
#define RELAY_2_PORT GPIOE
#define RELAY_3 GPIO_Pin_15
#define RELAY_3_PORT GPIOE
#define RELAY_4 GPIO_Pin_14
#define RELAY_4_PORT GPIOE



//===================================

#define RELAY_FIRE RELAY_1

#define RELAY_WIND RELAY_2

#define RELAY_DRY RELAY_3

#define RELAY_CLOD RELAY_4


//==============================

void Relay_Init(void);
void Relay_Open(uint16_t Relay_Name);
void Relay_Close(uint16_t Relay_Name);

#endif