#include "TIM.h"

uint32_t time = 0;
uint8_t updata_flag = 0;

void MyTIM_Init(void){

    TIM_TimeBaseInitTypeDef tmp = {
        .TIM_ClockDivision = TIM_CKD_DIV1,
        .TIM_CounterMode = TIM_CounterMode_Up,
        .TIM_Period = 500,
        .TIM_Prescaler = 8
    };
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = TIM1_UP_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 3,
        .NVIC_IRQChannelCmd = ENABLE
    };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseInit(TIM1, &tmp);
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM1, ENABLE);
}

void  TIM1_UP_IRQHandler (void)
{
    if ( TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET ) {
        time++;
        updata_flag = !updata_flag;
        // PC_Frame_delay_tick--;
        TIM_ClearITPendingBit(TIM1 , TIM_FLAG_Update);
    }
}