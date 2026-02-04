#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stm32f10x.h"

typedef enum{
    ANG_0 = 0,
    ANG_45,
    ANG_90,
    ANG_145,
    ANG_180
}MOTOR_ANG;

void Motor_Init(void);
void Motor_Turn(MOTOR_ANG ang);

#endif