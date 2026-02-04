#include "AudioPlay.h"
#include "stm32f10x.h"
#include "Delay.h"

void AudioPlay_Init(void){
    //PD6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);					//将PA3引脚初始化为模拟输入    

}

void AudioPlay_Play00001(void){
    GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_RESET);
    Delay_ms(100);
    GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_SET);
    Delay_ms(100);
}

void AudioPlay_Play00002(void){
    GPIO_WriteBit(GPIOD, GPIO_Pin_6, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_RESET);    
}