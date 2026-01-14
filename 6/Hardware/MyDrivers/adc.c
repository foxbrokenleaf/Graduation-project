#include "adc.h"
#include "stm32f1xx_hal.h"
#include "main.h"

uint16_t Read_MQ_9(){

    ADC_ChannelConfTypeDef tmpCfg = {
        .Channel = ADC_CHANNEL_3,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };

    if (HAL_ADC_ConfigChannel(&hadc1, &tmpCfg) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    HAL_ADC_Stop(&hadc1);

    return HAL_ADC_GetValue(&hadc1);
    
}

uint16_t Read_MQ_135(){
    ADC_ChannelConfTypeDef tmpCfg = {
        .Channel = ADC_CHANNEL_4,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };

    if (HAL_ADC_ConfigChannel(&hadc1, &tmpCfg) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    HAL_ADC_Stop(&hadc1);

    return HAL_ADC_GetValue(&hadc1);
}
