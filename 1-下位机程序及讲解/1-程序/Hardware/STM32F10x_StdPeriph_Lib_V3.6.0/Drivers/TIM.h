#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f10x.h"

extern uint32_t time;
extern uint8_t updata_flag;

void MyTIM_Init(void);

#endif