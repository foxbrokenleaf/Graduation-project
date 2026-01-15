#ifndef __ESP01S_AT_H
#define __ESP01S_AT_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ESP-01S状态定义
typedef enum {
    ESP01S_OK       = 0,
    ESP01S_ERROR    = 1,
    ESP01S_TIMEOUT  = 2,
    ESP01S_BUSY     = 3
} ESP01S_Status;

// WiFi模式
typedef enum {
    ESP01S_MODE_STATION = 1,
    ESP01S_MODE_AP      = 2,
    ESP01S_MODE_BOTH    = 3
} ESP01S_WifiMode;

// UDP模式
typedef enum {
    UDP_MODE_UNICAST    = 0,
    UDP_MODE_CHANGEABLE = 1,
    UDP_MODE_BROADCAST  = 2
} ESP01S_UdpMode;

// 传输模式
typedef enum {
    CIP_MODE_NORMAL = 0,
    CIP_MODE_PASSTHROUGH = 1
} ESP01S_CipMode;

// 连接状态
typedef enum {
    ESP_STATUS_INIT = 0,
    ESP_STATUS_WIFI_INIT = 1,
    ESP_STATUS_CONNECTED = 2,
    ESP_STATUS_TRANSMISSION = 3,
    ESP_STATUS_DISCONNECTED = 4,
    ESP_STATUS_WIFI_ERROR = 5
} ESP01S_ConnectionStatus;

// 接收状态机
typedef enum {
    RECV_STATE_IDLE = 0,
    RECV_STATE_PREFIX,      // 收到"+"
    RECV_STATE_IPD,         // 收到"IPD,"
    RECV_STATE_LENGTH,      // 解析长度
    RECV_STATE_COLON,       // 等待":"
    RECV_STATE_DATA         // 接收数据
} ESP01S_RecvState;

// 数据接收回调函数类型
typedef void (*ESP01S_DataCallback)(uint8_t link_id, uint8_t *data, uint16_t length);

// 接收数据包结构
typedef struct {
    uint8_t link_id;        // 连接ID
    uint8_t data[1024];     // 数据缓冲区
    uint16_t length;        // 数据长度
    uint16_t received;      // 已接收字节数
} ESP01S_Packet;

// AT指令缓存大小
#define AT_BUFFER_SIZE     1024
#define AT_TIMEOUT_DEFAULT 3000
#define AT_TIMEOUT_LONG    10000
#define AT_TIMEOUT_WIFI    15000

// 函数声明
void ESP01S_Init(UART_HandleTypeDef *huart);
ESP01S_Status ESP01S_SendCommand(const char *cmd, const char *expect, uint32_t timeout);
ESP01S_Status ESP01S_WaitResponse(const char *expect, uint32_t timeout);
void ESP01S_ClearBuffer(void);
void ESP01S_Delay(uint32_t ms);
char* ESP01S_GetResponseBuffer(void);
uint16_t ESP01S_GetResponseLength(void);
void ESP01S_PrintBuffer(void);

// 基本AT指令
ESP01S_Status ESP01S_Test(void);
ESP01S_Status ESP01S_Restart(void);
ESP01S_Status ESP01S_GetVersion(char *version);
ESP01S_Status ESP01S_SetEcho(uint8_t enable);
ESP01S_Status ESP01S_GetUARTConfig(uint8_t config_type);
ESP01S_Status ESP01S_SetUARTConfig(uint32_t baudrate, uint8_t databits, 
                                   uint8_t stopbits, uint8_t parity, 
                                   uint8_t flowctrl, uint8_t config_type);

// WiFi功能
ESP01S_Status ESP01S_GetWifiMode(uint8_t *mode);
ESP01S_Status ESP01S_SetWifiMode(ESP01S_WifiMode mode);
ESP01S_Status ESP01S_ScanAP(void);
ESP01S_Status ESP01S_ConnectAP(const char *ssid, const char *password);
ESP01S_Status ESP01S_DisconnectAP(void);
ESP01S_Status ESP01S_GetClientMAC(char *mac);
ESP01S_Status ESP01S_GetClientIP(char *ip, char *netmask);
ESP01S_Status ESP01S_GetAPMAC(char *mac);
ESP01S_Status ESP01S_GetAPIP(char *ip, char *gateway, char *netmask);
ESP01S_Status ESP01S_GetSoftAPConfig(char *ssid, char *password, 
                                     uint8_t *channel, uint8_t *encryption);

// TCP/IP状态
ESP01S_Status ESP01S_GetConnectionStatus(ESP01S_ConnectionStatus *status);
ESP01S_Status ESP01S_Ping(const char *ip, uint32_t *avg_time_ms);
ESP01S_Status ESP01S_GetLocalIP(char *ip);

// TCP/UDP连接管理
ESP01S_Status ESP01S_StartTCP(const char *ip, uint16_t port, uint8_t link_id, uint8_t ipv6);
ESP01S_Status ESP01S_StartUDP(const char *ip, uint16_t remote_port, 
                             uint16_t local_port, ESP01S_UdpMode mode, uint8_t link_id);
ESP01S_Status ESP01S_StartUDPv6(const char *ipv6, uint16_t remote_port, 
                               uint16_t local_port, uint8_t mode, uint8_t link_id);
ESP01S_Status ESP01S_StartSSL(const char *ip, uint16_t port, uint8_t link_id);
ESP01S_Status ESP01S_CloseConnection(uint8_t link_id);
ESP01S_Status ESP01S_SendData(uint8_t link_id, const char *data, uint16_t length);
ESP01S_Status ESP01S_SendDataEx(uint8_t link_id, const char *data, uint16_t length);
ESP01S_Status ESP01S_SendDataWithResult(uint8_t link_id, const char *data, uint16_t length, uint32_t timeout);
ESP01S_Status ESP01S_EnterPassthroughMode(uint8_t link_id);
ESP01S_Status ESP01S_ExitPassthroughMode(void);
ESP01S_Status ESP01S_SendDataPassthrough(const char *data, uint16_t length);
ESP01S_Status ESP01S_Send(uint8_t link_id, const char *data);

// 多连接模式
ESP01S_Status ESP01S_GetMultiplexMode(uint8_t *mode);
ESP01S_Status ESP01S_SetMultiplexMode(uint8_t enable);
ESP01S_Status ESP01S_StartServer(uint16_t port, uint8_t ssl_enable, uint8_t ssl_version);
ESP01S_Status ESP01S_StopServer(uint8_t link_id);
ESP01S_Status ESP01S_GetServerMaxConn(uint8_t *max_conn);
ESP01S_Status ESP01S_SetServerTimeout(uint32_t timeout_sec);
ESP01S_Status ESP01S_GetServerTimeout(uint32_t *timeout_sec);

// SNTP功能
ESP01S_Status ESP01S_GetSNTPTime(char *time_str);
ESP01S_Status ESP01S_GetSNTPConfig(char *timezone, char *server1, char *server2, char *server3);
ESP01S_Status ESP01S_SetSNTPConfig(const char *timezone, const char *server1, 
                                  const char *server2, const char *server3);

// 传输模式
ESP01S_Status ESP01S_GetTransmissionMode(uint8_t *mode);
ESP01S_Status ESP01S_SetTransmissionMode(ESP01S_CipMode mode);

// 数据接收处理
void ESP01S_ProcessData(uint8_t byte);
void ESP01S_SetDataCallback(ESP01S_DataCallback callback);
uint8_t ESP01S_HasData(void);
uint16_t ESP01S_ReadData(uint8_t *buffer, uint16_t max_len);
ESP01S_Packet* ESP01S_GetCurrentPacket(void);
uint8_t ESP01S_ParseIPD(const char *buffer, uint8_t *link_id, uint16_t *length, const char **data_start);
uint16_t ESP01S_ProcessBuffer(void);
void ESP01S_StartAutoReceive(void);
ESP01S_Status ESP01S_GetLatestData(uint8_t *link_id, uint8_t *buffer, uint16_t *length, uint16_t max_len);

// 工具函数
uint8_t ESP01S_CheckResponse(const char *str);

void HAL_UART_RxCpltCallback_WiFi(UART_HandleTypeDef *huart);

#endif /* __ESP01S_AT_H */