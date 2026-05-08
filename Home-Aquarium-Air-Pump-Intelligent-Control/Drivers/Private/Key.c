#include "Key.h"
#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#include "main.h"
#else
#include "stm32f10x.h"
#include "Delay.h"
#endif

#define KEY_NUM 14

#ifdef USE_HAL_DRIVER
ButtonType KeyInputBuff[KEY_NUM] = {
	{.GPIO_Port = K1_GPIO_Port, .GPIO_Pin = K1_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = K2_GPIO_Port, .GPIO_Pin = K2_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = K3_GPIO_Port, .GPIO_Pin = K3_Pin, .KeyState = KEY_UP, .HoldCnt = 0},

};
#else
GPIO_TypeDef *GPIO_Port_x[5] = {
    GPIOA,
    GPIOB,
    GPIOB,
    GPIOB,
    GPIOB
};
uint16_t GPIO_Pin_x[5] = {
    GPIO_Pin_8,
    GPIO_Pin_15,
    GPIO_Pin_14,
    GPIO_Pin_13,
    GPIO_Pin_12
};
#endif

#ifdef USE_HAL_DRIVER
#define ReadPin HAL_GPIO_ReadPin(GPIO_Port_x[index], GPIO_Pin_x[index])
#else
#define ReadPin GPIO_ReadInputDataBit(GPIO_Port_x[index], GPIO_Pin_x[index])
#endif

#ifdef USE_HAL_DRIVER
#define DelayMs(x) HAL_Delay(x)
#else
#define DelayMs(x) Delay_ms(x)
#endif

uint8_t KeepClick = 0;
uint8_t KeyState[5] = {0, 0, 0, 0, 0};

void Key_Init(){
    
    #ifdef USE_HAL_DRIVER

    #else    
    uint8_t index = 0;
    GPIO_InitTypeDef tmp;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    for(index = 0;index < KEY_NUM;index++){
        tmp.GPIO_Mode = GPIO_Mode_IPD;
        tmp.GPIO_Pin = GPIO_Pin_x[index];
        tmp.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIO_Port_x[index], &tmp);
    }
    #endif

}


void KeyScan(ButtonType *bt){
	if(HAL_GPIO_ReadPin(bt->GPIO_Port, bt->GPIO_Pin) == GPIO_PIN_RESET){
		bt->HoldCnt++;
		if(bt->HoldCnt > 100) bt->KeyState = KEY_HOLD;
	}
	else{
		bt->KeyState = KEY_UP;
		bt->HoldCnt = 0;
	} 
}

// uint8_t Key_Reads(uint8_t index){
//     uint8_t res = 0;

//     if(ReadPin == 1){
//         DelayMs(20);
//         while(ReadPin == 1){
//             KeepClick = 1;
//             res = 1;
//         }
//         DelayMs(20);
//     }

//     return res;
// }