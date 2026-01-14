#include <stdint.h>
#include <math.h>
#include "adc.h"
#include <stdio.h>
#include "main.h"

// 传感器参数配置
#define VREF 5.0                      // ADC参考电压
#define ADC_RESOLUTION 4096.0         // 10位ADC分辨率

// MQ-9参数
#define RL_MQ9 20.0                   // MQ-9负载电阻(千欧姆)
#define RO_CLEAN_AIR_MQ9 9.6          // MQ-9清洁空气RS/RO比值
#define MQ9_HEATER_VOLTAGE 5.0        // MQ-9加热电压
#define MQ9_PREHEAT_MINUTES 3         // MQ-9预热时间(分钟)

// MQ-135参数
#define RL_MQ135 10.0                 // MQ-135负载电阻(千欧姆)
#define RO_CLEAN_AIR_MQ135 3.6        // MQ-135清洁空气RS/RO比值
#define MQ135_HEATER_VOLTAGE 5.0      // MQ-135加热电压
#define MQ135_PREHEAT_MINUTES 1       // MQ-135预热时间(分钟)

// 校准状态标志
typedef enum {
    CALIBRATION_NEEDED = 0,
    CALIBRATION_IN_PROGRESS,
    CALIBRATION_COMPLETED
} CalibrationState;

// 全局校准数据
static struct {
    // MQ-9校准数据
    float r0_mq9;
    float base_temp_mq9;
    uint32_t calibration_time_mq9;
    
    // MQ-135校准数据
    float r0_mq135;
    float base_temp_mq135;
    uint32_t calibration_time_mq135;
    
    // 校准状态
    CalibrationState mq9_state;
    CalibrationState mq135_state;
} sensor_calibration = {
    .r0_mq9 = -1.0,
    .r0_mq135 = -1.0,
    .mq9_state = CALIBRATION_NEEDED,
    .mq135_state = CALIBRATION_NEEDED
};

/**
 * @brief 读取当前湿度（如果需要的话）
 * @return 湿度百分比，如果没有湿度传感器返回50.0（默认）
 */
static float Read_Humidity(void) {
    // 如果有湿度传感器，在这里实现
    // 否则返回默认值
    return 50.0f;
}

/**
 * @brief 计算MQ-9的RS值
 * @param adc_value ADC原始值
 * @return 传感器电阻RS(千欧姆)
 */
static float Calculate_RS_MQ9(uint16_t adc_value) {
    float voltage = (adc_value * VREF) / ADC_RESOLUTION;
    
    // 防止除零
    if (voltage < 0.001f) {
        voltage = 0.001f;
    }
    
    // RS = (VCC - Vout) * RL / Vout
    return (VREF - voltage) * RL_MQ9 / voltage;
}

/**
 * @brief 计算MQ-135的RS值
 * @param adc_value ADC原始值
 * @return 传感器电阻RS(千欧姆)
 */
static float Calculate_RS_MQ135(uint16_t adc_value) {
    float voltage = (adc_value * VREF) / ADC_RESOLUTION;
    
    // 防止除零
    if (voltage < 0.001f) {
        voltage = 0.001f;
    }
    
    return (VREF - voltage) * RL_MQ135 / voltage;
}

/**
 * @brief 温度补偿函数
 * @param ppm 原始ppm值
 * @param current_temp 当前温度
 * @param base_temp 校准时的温度
 * @param sensor_type 传感器类型：0-MQ9, 1-MQ135
 * @return 温度补偿后的ppm值
 */
static float Temperature_Compensation(float ppm, float current_temp, 
                                     float base_temp, uint8_t sensor_type) {
    float delta_temp = current_temp - base_temp;
    float compensation_factor;
    
    // 不同传感器的温度补偿系数不同
    if (sensor_type == 0) {  // MQ-9
        // MQ-9温度补偿：-0.3%/°C 到 -0.5%/°C
        compensation_factor = 1.0f + (delta_temp * -0.004f); // -0.4%/°C
    } else {  // MQ-135
        // MQ-135温度补偿：-0.1%/°C 到 -0.3%/°C
        compensation_factor = 1.0f + (delta_temp * -0.002f); // -0.2%/°C
    }
    
    return ppm * compensation_factor;
}

/**
 * @brief 湿度补偿函数
 * @param ppm 原始ppm值
 * @param humidity 当前湿度
 * @param sensor_type 传感器类型
 * @return 湿度补偿后的ppm值
 */
static float Humidity_Compensation(float ppm, float humidity, uint8_t sensor_type) {
    float compensation_factor;
    
    if (sensor_type == 0) {  // MQ-9
        // MQ-9对湿度相对不敏感
        compensation_factor = 1.0f - ((humidity - 50.0f) * 0.001f);
    } else {  // MQ-135
        // MQ-135对湿度比较敏感
        compensation_factor = 1.0f - ((humidity - 50.0f) * 0.005f);
    }
    
    return ppm * compensation_factor;
}

/**
 * @brief 校准MQ-9传感器
 * @note 应在清洁空气中，稳定温度下调用
 */
void Calibrate_MQ9(void) {
    float temp = Read_Temperature();
    uint16_t adc_value;
    float rs, r0;
    
    // 多次采样取平均
    int samples = 10;
    float rs_sum = 0.0f;
    
    for (int i = 0; i < samples; i++) {
        adc_value = Read_MQ_9();
        rs = Calculate_RS_MQ9(adc_value);
        rs_sum += rs;
        // 短暂延迟
        for (volatile int j = 0; j < 10000; j++);
    }
    
    rs = rs_sum / samples;
    r0 = rs / RO_CLEAN_AIR_MQ9;
    
    // 保存校准数据
    sensor_calibration.r0_mq9 = r0;
    sensor_calibration.base_temp_mq9 = temp;
    sensor_calibration.calibration_time_mq9 = 0; // 可以用系统时间
    sensor_calibration.mq9_state = CALIBRATION_COMPLETED;
}

/**
 * @brief 校准MQ-135传感器
 * @note 应在清洁空气中，稳定温度下调用
 */
void Calibrate_MQ135(void) {
    float temp = Read_Temperature();
    uint16_t adc_value;
    float rs, r0;
    
    int samples = 10;
    float rs_sum = 0.0f;
    
    for (int i = 0; i < samples; i++) {
        adc_value = Read_MQ_135();
        rs = Calculate_RS_MQ135(adc_value);
        rs_sum += rs;
        for (volatile int j = 0; j < 10000; j++);
    }
    
    rs = rs_sum / samples;
    r0 = rs / RO_CLEAN_AIR_MQ135;
    
    sensor_calibration.r0_mq135 = r0;
    sensor_calibration.base_temp_mq135 = temp;
    sensor_calibration.calibration_time_mq135 = 0;
    sensor_calibration.mq135_state = CALIBRATION_COMPLETED;
}

/**
 * @brief 读取MQ-9一氧化碳浓度
 * @return CO浓度(ppm)，0xFFFF表示错误或未校准
 */
uint16_t MQ_9(void) {
    // 检查校准状态
    if (sensor_calibration.mq9_state != CALIBRATION_COMPLETED) {
        return 0xFFF;
    }
    // printf("calibration done\r\n");
    
    // 读取当前环境参数
    float current_temp = Read_Temperature();
    float current_humidity = Read_Humidity();
    
    // 读取ADC值
    uint16_t adc_value = Read_MQ_9();
    if (adc_value == 0 || adc_value >= ADC_RESOLUTION) {
        return 0xFFF; // 无效ADC值
    }
    // printf("read adc done\r\n");
    
    // 计算RS
    float rs = Calculate_RS_MQ9(adc_value);
    
    // 计算RS/R0比率
    float ratio = rs / sensor_calibration.r0_mq9;
    
    // 防止无效比率
    if (ratio <= 0.0f) {
        return 0xFFF;
    }
    // printf("ratio done\r\n");
    
    // 使用MQ-9的CO响应曲线公式
    // 根据数据手册：ppm = a * (RS/R0)^b
    // 典型值：a = 100.0, b = -1.8 (需要根据实际校准调整)
    const float a = 100.0f;
    const float b = -1.8f;
    float ppm = a * powf(ratio, b);
    
    // 温度补偿
    ppm = Temperature_Compensation(ppm, current_temp, 
                                   sensor_calibration.base_temp_mq9, 0);
    
    // 湿度补偿
    ppm = Humidity_Compensation(ppm, current_humidity, 0);
    
    // 限制范围
    if (ppm < 0.0f) ppm = 0.0f;
    if (ppm > 10000.0f) ppm = 10000.0f; // MQ-9最大测量范围
    
    return (uint16_t)(ppm + 0.5f); // 四舍五入
}

/**
 * @brief 读取MQ-135空气质量数据
 * @return CO2浓度(ppm)，0xFFFF表示错误或未校准
 */
uint16_t MQ_135(void) {
    // 检查校准状态
    if (sensor_calibration.mq135_state != CALIBRATION_COMPLETED) {
        return 0xFFF;
    }
    
    // 读取当前环境参数
    float current_temp = Read_Temperature();
    float current_humidity = Read_Humidity();
    
    // 读取ADC值
    uint16_t adc_value = Read_MQ_135();
    if (adc_value == 0 || adc_value >= ADC_RESOLUTION) {
        return 0xFFF;
    }
    
    // 计算RS
    float rs = Calculate_RS_MQ135(adc_value);
    
    // 计算RS/R0比率
    float ratio = rs / sensor_calibration.r0_mq135;
    
    if (ratio <= 0.0f) {
        return 0xFFF;
    }
    
    // MQ-135 CO2响应曲线公式
    // 典型值：a = 116.6020682, b = -2.769034857
    const float a = 116.6020682f;
    const float b = -2.769034857f;
    float ppm = a * powf(ratio, b);
    
    // 温度补偿
    ppm = Temperature_Compensation(ppm, current_temp, 
                                   sensor_calibration.base_temp_mq135, 1);
    
    // 湿度补偿
    ppm = Humidity_Compensation(ppm, current_humidity, 1);
    
    // 限制范围
    if (ppm < 300.0f) ppm = 300.0f;   // 大气CO2最低值
    if (ppm > 5000.0f) ppm = 5000.0f; // MQ-135合理最大范围
    
    return (uint16_t)(ppm + 0.5f);
}

/**
 * @brief 获取MQ-9校准状态
 * @return 校准状态
 */
CalibrationState Get_MQ9_Calibration_State(void) {
    return sensor_calibration.mq9_state;
}

/**
 * @brief 获取MQ-135校准状态
 * @return 校准状态
 */
CalibrationState Get_MQ135_Calibration_State(void) {
    return sensor_calibration.mq135_state;
}

/**
 * @brief 获取MQ-9的R0值
 * @return R0值，-1表示未校准
 */
float Get_MQ9_R0(void) {
    return sensor_calibration.r0_mq9;
}

/**
 * @brief 获取MQ-135的R0值
 * @return R0值，-1表示未校准
 */
float Get_MQ135_R0(void) {
    return sensor_calibration.r0_mq135;
}