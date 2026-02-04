// DHT11.h
#ifndef __DHT11_H
#define __DHT11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

/**
 * @file DHT11.h
 * @brief DHT11温湿度传感器驱动程序
 * @note STM32F103VCT6, 系统时钟36MHz, PB11引脚
 * @note 针对HAL库优化
 */

// 引脚配置
#define DHT11_GPIO_PORT      GPIOB
#define DHT11_GPIO_PIN       GPIO_PIN_11
#define DHT11_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

// 错误代码
typedef enum {
    DHT11_OK = 0,
    DHT11_ERROR_TIMEOUT,
    DHT11_ERROR_CHECKSUM,
    DHT11_ERROR_NO_RESPONSE,
    DHT11_ERROR_BUS_BUSY
} DHT11_Status_t;

// 数据结构
typedef struct {
    uint8_t humidity_int;
    uint8_t humidity_decimal;
    uint8_t temp_int;
    uint8_t temp_decimal;
    uint8_t checksum;
} DHT11_Data_t;

// 函数声明
void DHT11_Init(void);
DHT11_Status_t DHT11_Read_Data(DHT11_Data_t *data);
DHT11_Status_t DHT11_Get_Values(float *temp, float *humi);
DHT11_Status_t DHT11_Read_With_Retry(DHT11_Data_t *data, uint8_t max_retries);
bool DHT11_Is_Connected(void);
void DHT11_Reset_Connection(void);

// 调试相关函数
#ifdef DHT11_DEBUG
void DHT11_Print_Debug_Info(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DHT11_H */