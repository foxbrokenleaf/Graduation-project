#include "Relay.h"






void Relay_Init(void){
    GPIO_InitTypeDef tmp = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Pin = RELAY_1
    };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_Init(RELAY_1_PORT, &tmp);

    tmp.GPIO_Pin = RELAY_2;
    GPIO_Init(RELAY_2_PORT, &tmp);

    tmp.GPIO_Pin = RELAY_3;
    GPIO_Init(RELAY_3_PORT, &tmp);

    tmp.GPIO_Pin = RELAY_4;
    GPIO_Init(RELAY_4_PORT, &tmp);

    GPIO_WriteBit(RELAY_1_PORT, RELAY_1, Bit_SET);
    GPIO_WriteBit(RELAY_2_PORT, RELAY_2, Bit_SET);
    GPIO_WriteBit(RELAY_3_PORT, RELAY_3, Bit_SET);
    GPIO_WriteBit(RELAY_4_PORT, RELAY_4, Bit_SET);
}

void Relay_Open(uint16_t Relay_Name){
    switch (Relay_Name)
    {
    case RELAY_1:
        GPIO_WriteBit(RELAY_1_PORT, RELAY_1, Bit_RESET);
        break;

    case RELAY_2:
        GPIO_WriteBit(RELAY_2_PORT, RELAY_2, Bit_RESET);
        break;

    case RELAY_3:
        GPIO_WriteBit(RELAY_3_PORT, RELAY_3, Bit_RESET);
        break;

    case RELAY_4:
        GPIO_WriteBit(RELAY_4_PORT, RELAY_4, Bit_RESET);
        break;   

    default:
        break;
    }
}

void Relay_Close(uint16_t Relay_Name){
    switch (Relay_Name)
    {
    case RELAY_1:
        GPIO_WriteBit(RELAY_1_PORT, RELAY_1, Bit_SET);
        break;

    case RELAY_2:
        GPIO_WriteBit(RELAY_2_PORT, RELAY_2, Bit_SET);
        break;

    case RELAY_3:
        GPIO_WriteBit(RELAY_3_PORT, RELAY_3, Bit_SET);
        break;

    case RELAY_4:
        GPIO_WriteBit(RELAY_4_PORT, RELAY_4, Bit_SET);
        break;   

    default:
        break;
    }
}