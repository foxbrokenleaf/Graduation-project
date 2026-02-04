#include "Motor.h"

#define MOTOR_PIN GPIO_Pin_12
#define MOTOR_PORT GPIOD

uint8_t Tick = 0;

void Motor_Init(void){
    
    GPIO_InitTypeDef tmp = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Pin = MOTOR_PIN
    };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_Init(MOTOR_PORT, &tmp);

    GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);

}

void Motor_Turn(MOTOR_ANG ang){

    if(Tick == 0)GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_SET);
    Tick++;

    switch (ang)
    {
    case ANG_0:
        if(Tick > 1) GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);
        break;
    case ANG_45:
        if(Tick > 2) GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);
        break;
    case ANG_90:
        if(Tick > 3) GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);
        break;
    case ANG_145:
        if(Tick > 4) GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);
        break;
    case ANG_180:
        if(Tick > 5) GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN, Bit_RESET);
        break;
                    
    default:
        break;
    }

    Tick %= 20;
}
