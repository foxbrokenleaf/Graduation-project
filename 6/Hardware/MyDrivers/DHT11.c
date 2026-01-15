// DHT11.c
#include "DHT11.h"
#include "main.h"
#include "stdio.h"

// 系统时钟频率 (根据您的配置是36MHz)
#ifndef SYSTEM_CORE_CLOCK
#define SYSTEM_CORE_CLOCK 36000000U
#endif

// 调试统计
#ifdef DHT11_DEBUG
static uint32_t dht11_success_count = 0;
static uint32_t dht11_fail_count = 0;
static uint32_t dht11_timeout_count = 0;
static uint32_t dht11_checksum_error = 0;
#endif

static DHT11_Status_t last_error = DHT11_OK;

// 使用TIM2作为微秒延时定时器（或其他空闲定时器）
static TIM_HandleTypeDef *dht11_timer = NULL;

/**
 * @brief 设置用于微秒延时的定时器
 * @param htim 定时器句柄
 * @note 需要在main.c中初始化一个定时器，如TIM2
 */
void DHT11_Set_Timer(TIM_HandleTypeDef *htim) {
    dht11_timer = htim;
}

/**
 * @brief 微秒级延时（使用硬件定时器）
 * @param us 微秒数
 * @note 需要先调用DHT11_Set_Timer设置定时器
 */
static void DHT11_Delay_us(uint32_t us) {
    if (dht11_timer == NULL) {
        // 如果没有设置定时器，使用软件延时
        volatile uint32_t count = us * 9; // 36MHz下的经验值
        while(count--) {
            __NOP();
        }
        return;
    }
    
    __HAL_TIM_SET_COUNTER(dht11_timer, 0);
    while (__HAL_TIM_GET_COUNTER(dht11_timer) < us);
}

/**
 * @brief 精确微秒延时（针对36MHz优化）
 * @param us 微秒数
 */
static void DHT11_Delay_us_Optimized(uint32_t us) {
    // 针对36MHz优化的软件延时
    // 36MHz下，每个循环约4个时钟周期
    // 每微秒需要 36/4 = 9 次循环
    uint32_t count = us * 9;
    
    while(count--) {
        __NOP();
        __NOP();
        __NOP();
    }
}

/**
 * @brief 设置PB11为输出模式
 */
static void DHT11_Set_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief 设置PB11为输入模式
 */
static void DHT11_Set_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief 读取PB11引脚电平
 */
static uint8_t DHT11_Read_Pin(void) {
    return HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

/**
 * @brief 设置PB11引脚电平
 */
static void DHT11_Write_Pin(uint8_t state) {
    HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, 
                     state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief 等待引脚达到指定状态（带超时）
 * @param state 期望的状态
 * @param timeout_us 超时时间（微秒）
 * @return DHT11状态
 */
static DHT11_Status_t DHT11_Wait_Pin(uint8_t state, uint32_t timeout_us) {
    uint32_t timeout = timeout_us;
    
    while (DHT11_Read_Pin() != state) {
        if (timeout-- == 0) {
            #ifdef DHT11_DEBUG
            if (state == 0) dht11_timeout_count++;
            #endif
            return DHT11_ERROR_TIMEOUT;
        }
        DHT11_Delay_us(1); // 1微秒延时
    }
    return DHT11_OK;
}

/**
 * @brief 读取一位数据
 * @return 数据位或0xFF（错误）
 */
static uint8_t DHT11_Read_Bit(void) {
    DHT11_Status_t status;
    
    // 等待低电平开始（50us低电平）
    status = DHT11_Wait_Pin(0, 60);
    if (status != DHT11_OK) {
        return 0xFF;
    }
    
    // 等待高电平开始
    status = DHT11_Wait_Pin(1, 60);
    if (status != DHT11_OK) {
        return 0xFF;
    }
    
    // 延时30us后检测电平
    // 0: 26-28us高电平
    // 1: 70us高电平
    DHT11_Delay_us(30);
    
    // 读取数据位
    uint8_t bit_value = DHT11_Read_Pin();
    
    // 等待高电平结束（总共约100us）
    DHT11_Delay_us(50);
    
    return bit_value;
}

/**
 * @brief 读取一个字节
 * @return 读取到的字节或0xFF（错误）
 */
static uint8_t DHT11_Read_Byte(void) {
    uint8_t byte = 0;
    uint8_t bit;
    
    for (int i = 0; i < 8; i++) {
        byte <<= 1;
        bit = DHT11_Read_Bit();
        
        if (bit == 0xFF) {
            return 0xFF;
        }
        
        if (bit) {
            byte |= 1;
        }
    }
    
    return byte;
}

/**
 * @brief 启动DHT11通信
 */
static DHT11_Status_t DHT11_Start(void) {
    DHT11_Status_t status;
    
    // 设置输出模式
    DHT11_Set_Output();
    
    // 主机拉低至少18ms（实际使用20ms更可靠）
    DHT11_Write_Pin(0);
    HAL_Delay(20);  // 使用HAL_Delay确保足够长时间
    
    // 主机拉高20-40us（使用30us）
    DHT11_Write_Pin(1);
    DHT11_Delay_us(30);
    
    // 切换为输入模式，等待DHT11响应
    DHT11_Set_Input();
    
    // 等待DHT11拉低响应（80us）
    status = DHT11_Wait_Pin(0, 100);
    if (status != DHT11_OK) {
        return DHT11_ERROR_NO_RESPONSE;
    }
    
    // 等待DHT11拉高（80us）
    status = DHT11_Wait_Pin(1, 100);
    if (status != DHT11_OK) {
        return DHT11_ERROR_TIMEOUT;
    }
    
    // 等待DHT11再次拉低开始传输数据
    status = DHT11_Wait_Pin(0, 100);
    if (status != DHT11_OK) {
        return DHT11_ERROR_TIMEOUT;
    }
    
    return DHT11_OK;
}

/**
 * @brief 初始化DHT11
 */
void DHT11_Init(void) {
    // 使能GPIO时钟
    DHT11_GPIO_CLK_ENABLE();
    
    // 初始化GPIO为输出模式，默认高电平
    DHT11_Set_Output();
    DHT11_Write_Pin(1);
    
    // 上电后等待至少1秒让传感器稳定
    HAL_Delay(1200);
    
    #ifdef DHT11_DEBUG
    printf("DHT11 Initialized (36MHz, PB11)\r\n");
    #endif
}

/**
 * @brief 重置DHT11连接
 */
void DHT11_Reset_Connection(void) {
    DHT11_Set_Output();
    DHT11_Write_Pin(1);
    HAL_Delay(100);
}

/**
 * @brief 读取DHT11数据
 */
DHT11_Status_t  DHT11_Read_Data(DHT11_Data_t *data) {
    DHT11_Status_t status;
    
    if (data == NULL) {
        return DHT11_ERROR_NO_RESPONSE;
    }
    
    // 启动通信
    status = DHT11_Start();
    if (status != DHT11_OK) {
        last_error = status;
        #ifdef DHT11_DEBUG
        dht11_fail_count++;
        #endif
        return status;
    }
    
    // 读取5个字节数据
    data->humidity_int = DHT11_Read_Byte();
    data->humidity_decimal = DHT11_Read_Byte();
    data->temp_int = DHT11_Read_Byte();
    data->temp_decimal = DHT11_Read_Byte();
    data->checksum = DHT11_Read_Byte();
    
    // 检查读取错误
    if (data->humidity_int == 0xFF || data->humidity_decimal == 0xFF ||
        data->temp_int == 0xFF || data->temp_decimal == 0xFF ||
        data->checksum == 0xFF) {
        last_error = DHT11_ERROR_TIMEOUT;
        #ifdef DHT11_DEBUG
        dht11_fail_count++;
        #endif
        return DHT11_ERROR_TIMEOUT;
    }
    
    // 校验和检查
    uint8_t sum = data->humidity_int + data->humidity_decimal + 
                  data->temp_int + data->temp_decimal;
    
    if (sum != data->checksum) {
        last_error = DHT11_ERROR_CHECKSUM;
        #ifdef DHT11_DEBUG
        dht11_checksum_error++;
        dht11_fail_count++;
        #endif
        return DHT11_ERROR_CHECKSUM;
    }
    
    last_error = DHT11_OK;
    
    #ifdef DHT11_DEBUG
    dht11_success_count++;
    #endif
    
    return DHT11_OK;
}

/**
 * @brief 获取温湿度值
 */
DHT11_Status_t DHT11_Get_Values(float *temp, float *humi) {
    DHT11_Data_t data;
    DHT11_Status_t status;
    
    status = DHT11_Read_Data(&data);
    
    if (status == DHT11_OK) {
        if (temp != NULL) {
            // DHT11温度通常只有整数部分
            *temp = (float)data.temp_int;
            printf("Temptrue=%d\r\n", data.temp_int);
            if (data.temp_decimal > 0) {
                *temp += (float)data.temp_decimal * 0.1f;
            }
        }
        if (humi != NULL) {
            // DHT11湿度通常只有整数部分
            *humi = (float)data.humidity_int;
            if (data.humidity_decimal > 0) {
                *humi += (float)data.humidity_decimal * 0.1f;
            }
        }
    }
    
    return status;
}

/**
 * @brief 带自动重试的读取
 * @param data 数据指针
 * @param max_retries 最大重试次数
 */
DHT11_Status_t DHT11_Read_With_Retry(DHT11_Data_t *data, uint8_t max_retries) {
    DHT11_Status_t status;
    
    for (uint8_t i = 0; i < max_retries; i++) {
        status = DHT11_Read_Data(data);
        
        if (status == DHT11_OK) {
            return DHT11_OK;
        }
        
        // 重试前延时
        HAL_Delay(10);
        
        // 重置连接
        DHT11_Reset_Connection();
    }
    
    return status;
}

/**
 * @brief 检查DHT11是否连接
 */
bool DHT11_Is_Connected(void) {
    DHT11_Status_t status;
    
    // 发送启动信号测试响应
    DHT11_Set_Output();
    
    DHT11_Write_Pin(0);
    HAL_Delay(20);
    
    DHT11_Write_Pin(1);
    DHT11_Delay_us(30);
    
    DHT11_Set_Input();
    
    // 等待响应
    status = DHT11_Wait_Pin(0, 100);
    
    // 恢复初始状态
    DHT11_Set_Output();
    DHT11_Write_Pin(1);
    
    return (status == DHT11_OK);
}

#ifdef DHT11_DEBUG
/**
 * @brief 打印调试信息
 */
void DHT11_Print_Debug_Info(void) {
    printf("=== DHT11 Debug Info ===\r\n");
    printf("System Clock: %lu Hz\r\n", SystemCoreClock);
    printf("Success Reads: %lu\r\n", dht11_success_count);
    printf("Failed Reads: %lu\r\n", dht11_fail_count);
    printf("Timeout Errors: %lu\r\n", dht11_timeout_count);
    printf("Checksum Errors: %lu\r\n", dht11_checksum_error);
    
    if ((dht11_success_count + dht11_fail_count) > 0) {
        float success_rate = (float)dht11_success_count / 
                           (dht11_success_count + dht11_fail_count) * 100.0f;
        printf("Success Rate: %.1f%%\r\n", success_rate);
    }
    printf("=======================\r\n");
}
#endif