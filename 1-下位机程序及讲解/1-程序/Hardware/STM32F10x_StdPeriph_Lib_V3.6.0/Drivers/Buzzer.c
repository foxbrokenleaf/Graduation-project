#include "Buzzer.h"

#define BUZZER_GROUP GPIOE
#define BUZZER_PIN GPIO_Pin_4

void Buzzer_Init(void){
    
    GPIO_InitTypeDef tmp = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Pin = BUZZER_PIN,
        .GPIO_Speed = GPIO_Speed_2MHz
    };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_Init(BUZZER_GROUP, &tmp);

    GPIO_WriteBit(BUZZER_GROUP, BUZZER_PIN, Bit_SET);
}

void Buzzer_Rang(void){
    GPIO_WriteBit(BUZZER_GROUP, BUZZER_PIN, Bit_RESET);
}

void Buzzer_Mute(void){
    GPIO_WriteBit(BUZZER_GROUP, BUZZER_PIN, Bit_SET);
}