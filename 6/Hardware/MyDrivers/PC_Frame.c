#include "PC_Frame.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdio.h"
#include "MQ.h"

typedef struct DATAFRAME_{
    uint8_t h1;
    uint8_t h2;
    uint8_t data_len;
    uint8_t uuid;
    uint8_t data[10];
    uint8_t crc;
    uint8_t e1;
    uint8_t e2;
}DataFrame;

void CRC_Cal(DataFrame *df){
    df->crc = (FRAME_HEAD_1 + FRAME_HEAD_2 + df->data_len + df->uuid + FRAME_END_1 + FRAME_END_2);
    for(uint8_t i = 0;i < df->data_len;i++) df->crc += df->data[i];
}

void DataFrame_Set_Value(DataFrame *df){
    switch (df->uuid)
    {
    case DRIVER_MQ_9:
        df->data[0] = ((CO_Value & 0xFF00) >> 8);
        df->data[1] = (CO_Value & 0x00FF);
        break;
    case DRIVER_Fire:
        df->data[0] = ((Fire_Value & 0xFF00) >> 8);
        df->data[1] = (Fire_Value & 0x00FF);
        break;
    case DRIVER_MQ_135:
        df->data[0] = ((Air_Level & 0xFF00) >> 8);
        df->data[1] = (Air_Level & 0x00FF);
        break;
    case DRIVER_DHT11:
        df->data[0] = Temptrue_Value;
        break;             
    default:
        break;
    }
}

DataFrame DataFrame_Build(uint8_t uuid){
    DataFrame res = {
        .h1 = FRAME_HEAD_1,
        .h2 = FRAME_HEAD_2,
        .data_len = 0,
        .uuid = uuid,
        .crc = 0x00,
        .e1 = FRAME_END_1,
        .e2 = FRAME_END_2
    };

    switch (uuid)
    {
    case DRIVER_MQ_9:
        res.uuid = DRIVER_MQ_9;
        res.data_len = 2;
        break;
    case DRIVER_Fire:
        res.uuid = DRIVER_Fire;
        res.data_len = 2;
        break;
    case DRIVER_MQ_135:
        res.uuid = DRIVER_MQ_135;
        res.data_len = 2;
        break;
    case DRIVER_DHT11:
        res.uuid = DRIVER_DHT11;
        res.data_len = 1;
        break;
    case SET_CO_WARM:
        res.uuid = SET_CO_WARM;
        res.data_len = 1;
    default:
        res.uuid = 0x00;
        res.data_len = 0;
        break;
    }
    DataFrame_Set_Value(&res);
    CRC_Cal(&res);

    return res;
}

uint8_t DataFrameTotalLenght(DataFrame *df){
    return (df->data_len + 7);
}

void DataFrame2Arrary(DataFrame *df, uint8_t ary[]){
    uint8_t index = 0;
    ary[index++] = FRAME_HEAD_1;
    ary[index++] = FRAME_HEAD_2;
    ary[index++] = df->data_len;
    ary[index++] = df->uuid;
    for(uint8_t i = 0;i < df->data_len;i++) ary[index++] = df->data[i];
    ary[index++] = df->crc;
    ary[index++] = df->e1;
    ary[index++] = df->e2;
}

void Send_Driver_Info(uint8_t UUID){

    uint8_t TxAry[10];
    DataFrame tmpTx = DataFrame_Build(UUID);
    DataFrame2Arrary(&tmpTx, TxAry);
    HAL_UART_Transmit(&huart1, TxAry, DataFrameTotalLenght(&tmpTx), HAL_MAX_DELAY);
}

/**
 * 从串口接收缓冲区DataBuff中解析数据帧，提取标识符和数据
 * 根据你的校验和计算方法，校验范围包括：
 * 帧头(2) + 数据长度 + 标识符 + 数据 + 帧尾(2)
 * 
 * @param DataBuff 串口接收缓冲区
 * @param buffSize 缓冲区大小
 * 
 * @return 0: 解析失败, 1: 解析成功
 */
uint8_t Receive_PC_Value(const uint8_t* DataBuff, uint16_t buffSize)
{
    // 1. 定义内部变量
    uint8_t identifier = 0;          // 标识符变量
    uint8_t dat[256] = {0};          // 数据变量数组
    uint16_t dataLength = 0;         // 实际数据长度
    uint8_t receivedChecksum = 0;    // 接收到的校验和
    uint8_t calculatedChecksum = 0;  // 计算得到的校验和
    
    // 2. 检查缓冲区大小
    if (DataBuff == 0 || buffSize < 7)  // 最小帧长度：帧头(2)+长度(1)+标识(1)+数据(0)+校验(1)+帧尾(2)=7
    {
        return 0;  // 失败
    }
    
    // 3. 在缓冲区中查找帧头(AA 55)
    uint16_t headerPos = 0;
    uint16_t footerPos = 0;
    uint8_t headerFound = 0;
    uint8_t footerFound = 0;
    
    // 查找帧头 (AA 55)
    for (headerPos = 0; headerPos <= buffSize - 2; headerPos++)
    {
        if (DataBuff[headerPos] == FRAME_HEAD_1 && DataBuff[headerPos + 1] == FRAME_HEAD_2)
        {
            headerFound = 1;
            break;
        }
    }
    
    if (!headerFound)
    {
        return 0;  // 未找到帧头
    }
    printf("Found freame of head\r\n");
    
    // 4. 查找帧尾 (55 AA)，从帧头后开始查找
    for (footerPos = headerPos + 4; footerPos <= buffSize - 2; footerPos++)  // 从长度字节后开始
    {
        if (DataBuff[footerPos] == FRAME_END_1 && DataBuff[footerPos + 1] == FRAME_END_2)
        {
            footerFound = 1;
            break;
        }
    }
    
    if (!footerFound)
    {
        return 0;  // 未找到帧尾
    }
    printf("Found freame of end\r\n");
    // 5. 解析数据长度
    dataLength = DataBuff[headerPos + 2];
    
    // 6. 验证帧长度是否完整
    // 理论帧长度 = 帧头(2) + 长度(1) + 标识(1) + 数据(dataLength) + 校验(1) + 帧尾(2)
    uint16_t expectedFrameLength = 7 + dataLength;
    uint16_t actualFrameLength = footerPos - headerPos + 2;  // +2包含帧尾的2字节
    
    if (actualFrameLength != expectedFrameLength)
    {
        return 0;  // 帧长度不匹配
    }
    
    // 7. 检查数据长度是否超过dat数组容量
    if (dataLength > 256)
    {
        return 0;  // 数据长度超过内部数组容量
    }
    
    // 8. 提取标识符
    identifier = DataBuff[headerPos + 3];
    
    // 9. 提取数据
    uint16_t dataStartPos = headerPos + 4;
    for (uint16_t i = 0; i < dataLength; i++)
    {
        dat[i] = DataBuff[dataStartPos + i];
    }
    
    // 10. 获取接收到的校验和（在数据之后，帧尾之前）
    receivedChecksum = DataBuff[dataStartPos + dataLength];
    
    // 11. 按照你的CRC_Cal函数计算校验和
    calculatedChecksum = 0;
    
    // 计算帧头部分
    calculatedChecksum += FRAME_HEAD_1;  // 0xAA
    calculatedChecksum += FRAME_HEAD_2;  // 0x55
    
    // 计算数据长度
    calculatedChecksum += DataBuff[headerPos + 2];  // data_len
    
    // 计算标识符
    calculatedChecksum += DataBuff[headerPos + 3];  // uuid
    
    // 计算数据部分
    for (uint16_t i = 0; i < dataLength; i++)
    {
        calculatedChecksum += DataBuff[dataStartPos + i];  // 每个数据字节
    }
    
    // 计算帧尾部分
    calculatedChecksum += FRAME_END_1;  // 0x55
    calculatedChecksum += FRAME_END_2;  // 0xAA
    
    // 12. 验证校验和
    if (calculatedChecksum != receivedChecksum)
    {
        // 调试输出，帮助你分析
        // printf("CRC Error! Calculated: 0x%02X, Received: 0x%02X\r\n", calculatedChecksum, receivedChecksum);
        return 0;  // 校验和错误
    }
    
    // 13. 数据提取成功，可以在这里使用identifier和dat数组
    switch (identifier)
    {
    case SET_CO_WARM:
        printf("SET_CO_WARM\r\n");
        CO_Warn_Value = (dat[0] << 8) | dat[1];
        printf("CO_Warn_Value = %d\r\n", CO_Warn_Value);
        Clear_ReceiveBuff();
        break;
    case SET_FIRE_WARM:
        printf("SET_FIRE_WARM\r\n");
        Fire_Warn_Value = (dat[0] << 8) | dat[1];
        printf("Fire_Warn_Value = %d\r\n", Fire_Warn_Value);
        Clear_ReceiveBuff();
        break;
    case SET_AIR_LEVEL_WARM:
        printf("SET_AIR_LEVEL_WARM\r\n");
        Air_Warn_Level = (dat[0] << 8) | dat[1];
        printf("Air_Warn_Level = %d\r\n", Air_Warn_Level);
        Clear_ReceiveBuff();
        break;
    case SET_TEMPTRUE_WARM:
        printf("SET_TEMPTRUE_WARM\r\n");
        Temptrue_Value = dat[0];
        printf("Temptrue_Value = %d\r\n", Temptrue_Value);
        Clear_ReceiveBuff();
        break;                        
    
    default:
        Clear_ReceiveBuff();
        break;
    }
    
    return 1;  // 解析成功
}
