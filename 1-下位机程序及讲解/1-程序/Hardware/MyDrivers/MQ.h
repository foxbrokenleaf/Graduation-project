#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file gas_sensors.h
 * @brief MQ-9和MQ-135气体传感器驱动接口
 * @version 1.0
 */

// 传感器类型定义
typedef enum {
    SENSOR_MQ9 = 0,
    SENSOR_MQ135 = 1
} SensorType;

// 校准状态定义
typedef enum {
    CALIBRATION_NEEDED = 0,      ///< 需要校准
    CALIBRATION_IN_PROGRESS,     ///< 校准中
    CALIBRATION_COMPLETED        ///< 校准完成
} CalibrationState;

// 传感器状态结构体
typedef struct {
    CalibrationState cal_state;  ///< 校准状态
    float r0_value;              ///< R0校准值（千欧姆）
    float base_temperature;      ///< 校准时的温度（摄氏度）
    uint32_t cal_timestamp;      ///< 校准时间戳
    bool is_heating;             ///< 是否在加热状态
    uint32_t power_on_time;      ///< 上电时间（毫秒）
} SensorStatus;

// 气体浓度范围（安全/警告/危险）
typedef struct {
    uint16_t safe_min;      ///< 安全最小值（ppm）
    uint16_t safe_max;      ///< 安全最大值（ppm）
    uint16_t warning_min;   ///< 警告最小值（ppm）
    uint16_t warning_max;   ///< 警告最大值（ppm）
    uint16_t danger_min;    ///< 危险最小值（ppm）
    uint16_t danger_max;    ///< 危险最大值（ppm）
} GasConcentrationRange;

// 空气质量等级
typedef enum {
    AIR_QUALITY_EXCELLENT = 0,   ///< 优 (0-50)
    AIR_QUALITY_GOOD,           ///< 良 (51-100)
    AIR_QUALITY_MODERATE,       ///< 轻度污染 (101-150)
    AIR_QUALITY_POOR,           ///< 中度污染 (151-200)
    AIR_QUALITY_VERY_POOR,      ///< 重度污染 (201-300)
    AIR_QUALITY_HAZARDOUS       ///< 严重污染 (>300)
} AirQualityLevel;

/**
 * @brief 初始化气体传感器模块
 * @note 调用此函数初始化内部状态和校准标志
 */
void GasSensors_Init(void);

/**
 * @brief 校准MQ-9传感器
 * @note 应在清洁空气环境中调用，建议预热3分钟后校准
 * @return true: 校准成功, false: 校准失败
 */
void Calibrate_MQ9(void);

/**
 * @brief 校准MQ-135传感器
 * @note 应在清洁空气环境中调用，建议预热1分钟后校准
 * @return true: 校准成功, false: 校准失败
 */
void Calibrate_MQ135(void);

/**
 * @brief 同时校准两个传感器
 * @note 将两个传感器置于清洁空气中后调用
 * @return 成功校准的传感器数量
 */
void Calibrate_All_Sensors(void);

/**
 * @brief 读取MQ-9一氧化碳浓度
 * @return CO浓度(ppm)，0xFFFF表示错误或未校准
 * @note 自动进行温度和湿度补偿
 */
uint16_t MQ_9(void);
uint16_t MQ_9_Fire(void);
/**
 * @brief 读取MQ-135二氧化碳浓度
 * @return CO2浓度(ppm)，0xFFFF表示错误或未校准
 * @note 自动进行温度和湿度补偿
 */
uint16_t MQ_135(void);

/**
 * @brief 读取MQ-135综合空气质量指数(AQI)
 * @return AQI值 (0-500)，0xFFFF表示错误或未校准
 */
uint16_t MQ_135_GetAQI(void);

/**
 * @brief 根据CO2浓度获取空气质量等级
 * @param co2_ppm CO2浓度(ppm)
 * @return 空气质量等级
 */
AirQualityLevel Get_Air_Quality_Level(uint16_t co2_ppm);

/**
 * @brief 根据CO浓度获取安全等级
 * @param co_ppm CO浓度(ppm)
 * @return 安全等级 (0:安全, 1:注意, 2:警告, 3:危险)
 */
uint8_t Get_CO_Safety_Level(uint16_t co_ppm);

/**
 * @brief 获取MQ-9传感器状态
 * @param[out] status 传感器状态结构体指针
 * @return true: 获取成功, false: 获取失败
 */
bool Get_MQ9_Status(SensorStatus *status);

/**
 * @brief 获取MQ-135传感器状态
 * @param[out] status 传感器状态结构体指针
 * @return true: 获取成功, false: 获取失败
 */
bool Get_MQ135_Status(SensorStatus *status);

/**
 * @brief 获取MQ-9校准状态
 * @return 校准状态
 */
CalibrationState Get_MQ9_Calibration_State(void);

/**
 * @brief 获取MQ-135校准状态
 * @return 校准状态
 */
CalibrationState Get_MQ135_Calibration_State(void);

/**
 * @brief 获取MQ-9的R0校准值
 * @return R0值（千欧姆），-1表示未校准
 */
float Get_MQ9_R0(void);

/**
 * @brief 获取MQ-135的R0校准值
 * @return R0值（千欧姆），-1表示未校准
 */
float Get_MQ135_R0(void);

/**
 * @brief 检查传感器是否需要预热
 * @param sensor_type 传感器类型
 * @return true: 需要预热, false: 已预热完成
 */
bool Sensor_Need_Preheat(SensorType sensor_type);

/**
 * @brief 获取传感器预热剩余时间
 * @param sensor_type 传感器类型
 * @return 剩余预热时间（秒）
 */
uint16_t Get_Sensor_Preheat_Remaining(SensorType sensor_type);

/**
 * @brief 获取传感器加热器状态
 * @param sensor_type 传感器类型
 * @return true: 加热器开启, false: 加热器关闭或错误
 */
bool Get_Sensor_Heater_Status(SensorType sensor_type);

/**
 * @brief 控制传感器加热器
 * @param sensor_type 传感器类型
 * @param enable true: 开启加热, false: 关闭加热
 * @return true: 操作成功, false: 操作失败
 * @note 注意：长时间关闭加热会影响传感器性能
 */
bool Control_Sensor_Heater(SensorType sensor_type, bool enable);

/**
 * @brief 获取传感器硬件版本
 * @param sensor_type 传感器类型
 * @return 版本号字符串，NULL表示未知
 */
const char* Get_Sensor_Version(SensorType sensor_type);

/**
 * @brief 获取传感器故障代码
 * @param sensor_type 传感器类型
 * @return 故障代码：0-正常，其他值参考故障代码表
 */
uint8_t Get_Sensor_Fault_Code(SensorType sensor_type);

/**
 * @brief 重置传感器状态
 * @param sensor_type 传感器类型
 * @note 清除校准数据，需要重新校准
 */
void Reset_Sensor(SensorType sensor_type);

/**
 * @brief 保存传感器校准数据到非易失存储器
 * @return 成功保存的传感器数量
 */
uint8_t Save_Calibration_Data(void);

/**
 * @brief 从非易失存储器加载传感器校准数据
 * @return 成功加载的传感器数量
 */
uint8_t Load_Calibration_Data(void);

/**
 * @brief 设置环境参数
 * @param temperature 当前温度（摄氏度）
 * @param humidity 当前湿度（百分比，0-100）
 * @note 如果不设置，将使用默认值或传感器读取值
 */
void Set_Environment_Params(float temperature, float humidity);

/**
 * @brief 获取默认CO浓度安全范围
 * @return CO浓度范围结构体
 * @note 可根据实际应用场景调整
 */
GasConcentrationRange Get_Default_CO_Range(void);

/**
 * @brief 获取默认CO2浓度安全范围
 * @return CO2浓度范围结构体
 * @note 可根据实际应用场景调整
 */
GasConcentrationRange Get_Default_CO2_Range(void);

/**
 * @brief 设置自定义浓度范围
 * @param sensor_type 传感器类型
 * @param range 自定义浓度范围
 */
void Set_Custom_Concentration_Range(SensorType sensor_type, GasConcentrationRange range);

// 错误代码定义
#define GAS_SENSOR_ERROR_OK           0x00  ///< 正常
#define GAS_SENSOR_ERROR_NOT_CALIB    0x01  ///< 未校准
#define GAS_SENSOR_ERROR_ADC_READ     0x02  ///< ADC读取错误
#define GAS_SENSOR_ERROR_OUT_OF_RANGE 0x03  ///< 数值超出范围
#define GAS_SENSOR_ERROR_HEATER_FAIL  0x04  ///< 加热器故障
#define GAS_SENSOR_ERROR_SENSOR_FAIL  0x05  ///< 传感器故障
#define GAS_SENSOR_ERROR_COMM_FAIL    0x06  ///< 通信故障

// 常量定义
#define MQ9_MAX_CONCENTRATION     10000  ///< MQ-9最大测量浓度(ppm)
#define MQ135_MAX_CONCENTRATION   5000   ///< MQ-135最大测量浓度(ppm)
#define MQ9_MIN_VALID_ADC         10     ///< MQ-9最小有效ADC值
#define MQ135_MIN_VALID_ADC       10     ///< MQ-135最小有效ADC值
#define MQ9_PREHEAT_TIME          180    ///< MQ-9预热时间(秒)
#define MQ135_PREHEAT_TIME        60     ///< MQ-135预热时间(秒)

#endif /* GAS_SENSORS_H */