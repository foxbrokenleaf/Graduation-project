#include "WiFi.h"

// 调试配置
#define ESP01S_DEBUG               // 启用调试信息
// #define ESP01S_DEBUG_VERBOSE    // 启用详细调试信息（接收的原始数据）
// #define ESP01S_DEBUG_DATA       // 启用数据内容调试
// #define ESP01S_DEBUG_VERBOSE    // 启用详细调试信息
// #define ESP01S_DEBUG_STATE      // 启用状态转换跟踪
// #define ESP01S_DEBUG_VERBOSE_DATA // 启用数据字节详细输出
// #define ESP01S_DEBUG_DATA_SUMMARY // 启用数据包摘要
// #define ESP01S_DEBUG_TIMEOUT    // 启用状态超时检测

#ifdef ESP01S_DEBUG
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

// 全局变量
static UART_HandleTypeDef *esp_huart = NULL;
static uint8_t at_buffer[AT_BUFFER_SIZE];
static uint16_t at_buffer_index = 0;
static volatile uint8_t rx_complete = 0;

// 接收处理相关变量
static ESP01S_RecvState recv_state = RECV_STATE_IDLE;
static ESP01S_Packet current_packet;
static ESP01S_DataCallback data_callback = NULL;
static uint8_t packet_buffer[10];
static uint8_t packet_index = 0;
static uint8_t data_available = 0;

/**
  * @brief  UART接收中断回调
  */
void HAL_UART_RxCpltCallback_WiFi(UART_HandleTypeDef *huart)
{
    if (huart->Instance == esp_huart->Instance)
    {
        uint8_t byte = at_buffer[at_buffer_index];
        
        // 保存到AT指令缓冲区
        if (at_buffer_index < AT_BUFFER_SIZE - 1)
        {
            at_buffer[at_buffer_index] = byte;
            at_buffer_index++;
        }
        
        // 处理数据接收
        ESP01S_ProcessData(byte);
        
        // 继续接收下一个字节
        HAL_UART_Receive_IT(esp_huart, &at_buffer[at_buffer_index], 1);
    }
}

/**
  * @brief  处理接收到的字节（状态机）
  */
void ESP01S_ProcessData(uint8_t byte)
{
    static uint16_t expected_length = 0;
    static uint8_t link_id = 0;
    
    switch (recv_state)
    {
        case RECV_STATE_IDLE:
            if (byte == '+')
            {
                recv_state = RECV_STATE_PREFIX;
                packet_index = 0;
                expected_length = 0;
                link_id = 0;
            }
            break;
            
        case RECV_STATE_PREFIX:
            if (byte == 'I')
            {
                recv_state = RECV_STATE_IPD;
                packet_buffer[packet_index++] = 'I';
            }
            else
            {
                recv_state = RECV_STATE_IDLE;
            }
            break;
            
        case RECV_STATE_IPD:
            if (packet_index < 4)
            {
                packet_buffer[packet_index++] = byte;
                packet_buffer[packet_index] = '\0';
                
                if (packet_index == 4)
                {
                    if (strncmp((char*)packet_buffer, "IPD,", 4) == 0)
                    {
                        recv_state = RECV_STATE_LENGTH;
                    }
                    else
                    {
                        recv_state = RECV_STATE_IDLE;
                    }
                }
            }
            else
            {
                recv_state = RECV_STATE_IDLE;
            }
            break;
            
        case RECV_STATE_LENGTH:
            if (byte == ':')
            {
                recv_state = RECV_STATE_DATA;
                current_packet.received = 0;
                current_packet.length = expected_length;
                current_packet.link_id = link_id;
                
                if (expected_length == 0)
                {
                    recv_state = RECV_STATE_IDLE;
                    data_available = 1;
                }
            }
            else if (byte == ',')
            {
                link_id = expected_length;
                expected_length = 0;
            }
            else if (byte >= '0' && byte <= '9')
            {
                expected_length = expected_length * 10 + (byte - '0');
            }
            else
            {
                recv_state = RECV_STATE_IDLE;
            }
            break;
            
        case RECV_STATE_DATA:
            if (current_packet.received < current_packet.length &&
                current_packet.received < sizeof(current_packet.data))
            {
                current_packet.data[current_packet.received++] = byte;
                
                if (current_packet.received >= current_packet.length)
                {
                    data_available = 1;
                    
                    if (data_callback != NULL)
                    {
                        data_callback(current_packet.link_id, 
                                     current_packet.data, 
                                     current_packet.length);
                    }
                    
                    recv_state = RECV_STATE_IDLE;
                }
            }
            else
            {
                recv_state = RECV_STATE_IDLE;
            }
            break;
            
        default:
            recv_state = RECV_STATE_IDLE;
            break;
    }
}

/**
  * @brief  初始化ESP01S
  */
void ESP01S_Init(UART_HandleTypeDef *huart)
{
    esp_huart = huart;
    at_buffer_index = 0;
    rx_complete = 0;
    recv_state = RECV_STATE_IDLE;
    data_available = 0;
    
    memset(at_buffer, 0, AT_BUFFER_SIZE);
    memset(&current_packet, 0, sizeof(current_packet));
    
    HAL_UART_Receive_IT(esp_huart, &at_buffer[0], 1);
    
    ESP01S_Delay(2000);
}

/**
  * @brief  发送AT指令并等待响应
  */
ESP01S_Status ESP01S_SendCommand(const char *cmd, const char *expect, uint32_t timeout)
{
    char cmd_buffer[256];
    uint32_t start_time = HAL_GetTick();
    
    ESP01S_ClearBuffer();
    
    if (cmd != NULL && strlen(cmd) > 0)
    {
        snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", cmd);
        HAL_UART_Transmit(esp_huart, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 200);
    }
    
    return ESP01S_WaitResponse(expect, timeout);
}

/**
  * @brief  等待响应
  */
ESP01S_Status ESP01S_WaitResponse(const char *expect, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();
    
    while ((HAL_GetTick() - start_time) < timeout)
    {
        if (at_buffer_index > 0)
        {
            if (at_buffer_index < AT_BUFFER_SIZE)
            {
                at_buffer[at_buffer_index] = '\0';
            }
            
            if (expect != NULL)
            {
                if (strstr((char*)at_buffer, expect) != NULL)
                {
                    return ESP01S_OK;
                }
            }
            
            if (strstr((char*)at_buffer, "ERROR") != NULL ||
                strstr((char*)at_buffer, "FAIL") != NULL)
            {
                return ESP01S_ERROR;
            }
        }
        ESP01S_Delay(1);
    }
    
    return ESP01S_TIMEOUT;
}

/**
  * @brief  清空接收缓冲区
  */
void ESP01S_ClearBuffer(void)
{
    memset(at_buffer, 0, AT_BUFFER_SIZE);
    at_buffer_index = 0;
}

/**
  * @brief  AT测试
  */
ESP01S_Status ESP01S_Test(void)
{
    return ESP01S_SendCommand("AT", "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  重启模块
  */
ESP01S_Status ESP01S_Restart(void)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+RST", "ready", 5000);
    if (status == ESP01S_OK)
    {
        ESP01S_Delay(3000);
        ESP01S_ClearBuffer();
    }
    return status;
}

/**
  * @brief  获取固件版本
  */
ESP01S_Status ESP01S_GetVersion(char *version)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+GMR", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && version != NULL)
    {
        strncpy(version, (char*)at_buffer, 100);
    }
    return status;
}

/**
  * @brief  设置回显
  */
ESP01S_Status ESP01S_SetEcho(uint8_t enable)
{
    char cmd[10];
    snprintf(cmd, sizeof(cmd), "ATE%d", enable);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  获取UART配置
  */
ESP01S_Status ESP01S_GetUARTConfig(uint8_t config_type)
{
    const char *cmd = (config_type == 0) ? "AT+UART_CUR?" : "AT+UART_DEF?";
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  设置UART配置
  */
ESP01S_Status ESP01S_SetUARTConfig(uint32_t baudrate, uint8_t databits, 
                                   uint8_t stopbits, uint8_t parity, 
                                   uint8_t flowctrl, uint8_t config_type)
{
    char cmd[100];
    const char *cmd_type = (config_type == 0) ? "AT+UART_CUR" : "AT+UART_DEF";
    
    snprintf(cmd, sizeof(cmd), "%s=%lu,%d,%d,%d,%d", 
             cmd_type, baudrate, databits, stopbits, parity, flowctrl);
    
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  获取WiFi模式
  */
ESP01S_Status ESP01S_GetWifiMode(uint8_t *mode)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+CWMODE?", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && mode != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "+CWMODE:");
        if (ptr != NULL)
        {
            *mode = atoi(ptr + 8);
        }
    }
    return status;
}

/**
  * @brief  设置WiFi模式
  */
ESP01S_Status ESP01S_SetWifiMode(ESP01S_WifiMode mode)
{
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "AT+CWMODE=%d", mode);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  扫描AP
  */
ESP01S_Status ESP01S_ScanAP(void)
{
    return ESP01S_SendCommand("AT+CWLAP", "OK", AT_TIMEOUT_LONG);
}

/**
  * @brief  连接AP
  */
ESP01S_Status ESP01S_ConnectAP(const char *ssid, const char *password)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_WIFI);
}

/**
  * @brief  断开AP连接
  */
ESP01S_Status ESP01S_DisconnectAP(void)
{
    return ESP01S_SendCommand("AT+CWQAP", "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  获取客户端MAC地址
  */
ESP01S_Status ESP01S_GetClientMAC(char *mac)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+CIPSTAMAC?", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && mac != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "+CIPSTAMAC:");
        if (ptr != NULL)
        {
            ptr += 12;
            char *start = strchr(ptr, '\"');
            char *end = strchr(start + 1, '\"');
            if (start && end)
            {
                uint16_t len = end - start - 1;
                strncpy(mac, start + 1, len);
                mac[len] = '\0';
            }
        }
    }
    return status;
}

/**
  * @brief  获取客户端IP地址
  */
ESP01S_Status ESP01S_GetClientIP(char *ip, char *netmask)
{
    return ESP01S_SendCommand("AT+CIPSTA?", "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  获取连接状态
  */
ESP01S_Status ESP01S_GetConnectionStatus(ESP01S_ConnectionStatus *status)
{
    ESP01S_Status ret = ESP01S_SendCommand("AT+CIPSTATUS", "OK", AT_TIMEOUT_DEFAULT);
    if (ret == ESP01S_OK && status != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "STATUS:");
        if (ptr != NULL)
        {
            *status = (ESP01S_ConnectionStatus)atoi(ptr + 7);
        }
    }
    return ret;
}

/**
  * @brief  Ping测试
  */
ESP01S_Status ESP01S_Ping(const char *ip, uint32_t *avg_time_ms)
{
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+PING=\"%s\"", ip);
    return ESP01S_SendCommand(cmd, "OK", 10000);
}

/**
  * @brief  建立TCP连接
  */
ESP01S_Status ESP01S_StartTCP(const char *ip, uint16_t port, uint8_t link_id, uint8_t ipv6)
{
    char cmd[128];
    if (ipv6)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"TCPv6\",\"%s\",%d", 
                 link_id, ip, port);
    }
    else
    {
        if (link_id == 0)
        {
            snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);
        }
        else
        {
            snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", 
                     link_id, ip, port);
        }
    }
    return ESP01S_SendCommand(cmd, "OK", 10000);
}

/**
  * @brief  建立UDP连接
  */
ESP01S_Status ESP01S_StartUDP(const char *ip, uint16_t remote_port, 
                             uint16_t local_port, ESP01S_UdpMode mode, uint8_t link_id)
{
    char cmd[128];
    if (link_id == 0)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d,%d",
                 ip, remote_port, local_port, mode);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d,%d",
                 link_id, ip, remote_port, local_port, mode);
    }
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  建立UDPv6连接
  */
ESP01S_Status ESP01S_StartUDPv6(const char *ipv6, uint16_t remote_port, 
                               uint16_t local_port, uint8_t mode, uint8_t link_id)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"UDPv6\",\"%s\",%d,%d,%d",
             link_id, ipv6, remote_port, local_port, mode);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  建立SSL连接
  */
ESP01S_Status ESP01S_StartSSL(const char *ip, uint16_t port, uint8_t link_id)
{
    char cmd[128];
    if (link_id == 0)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"SSL\",\"%s\",%d", ip, port);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSTART=%d,\"SSL\",\"%s\",%d", 
                 link_id, ip, port);
    }
    return ESP01S_SendCommand(cmd, "OK", 15000);
}

/**
  * @brief  关闭连接
  */
ESP01S_Status ESP01S_CloseConnection(uint8_t link_id)
{
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE=%d", link_id);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  发送数据
  * @note   添加了调试信息输出
  */
ESP01S_Status ESP01S_SendData(uint8_t link_id, const char *data, uint16_t length)
{
    char cmd[32];
    uint32_t start_time;
    uint8_t found_arrow = 0;
    
    DEBUG_PRINT("[ESP01S_SendData] 开始发送数据: link_id=%d, length=%d\r\n", link_id, length);
    
    ESP01S_ClearBuffer();
    DEBUG_PRINT("[ESP01S_SendData] 缓冲区已清空\r\n");
    
    if (link_id == 0)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", length);
        DEBUG_PRINT("[ESP01S_SendData] 单连接模式: %s\r\n", cmd);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d,%d", link_id, length);
        DEBUG_PRINT("[ESP01S_SendData] 多连接模式: %s\r\n", cmd);
    }
    
    DEBUG_PRINT("[ESP01S_SendData] 发送AT命令...\r\n");
    HAL_UART_Transmit(esp_huart, (uint8_t*)cmd, strlen(cmd), 100);
    HAL_UART_Transmit(esp_huart, (uint8_t*)"\r\n", 2, 100);
    
    DEBUG_PRINT("[ESP01S_SendData] 等待'>'提示符...\r\n");
    start_time = HAL_GetTick();
    while ((HAL_GetTick() - start_time) < 3000)
    {
        if (at_buffer_index > 0)
        {
            if (at_buffer_index < AT_BUFFER_SIZE)
            {
                at_buffer[at_buffer_index] = '\0';
            }
            
            // 检查是否有'>'字符
            for (uint16_t i = 0; i < at_buffer_index; i++)
            {
                if (at_buffer[i] == '>')
                {
                    found_arrow = 1;
                    DEBUG_PRINT("[ESP01S_SendData] 收到'>'提示符\r\n");
                    break;
                }
            }
            
            // 检查错误响应
            if (strstr((char*)at_buffer, "ERROR") != NULL)
            {
                DEBUG_PRINT("[ESP01S_SendData] ERROR: %s\r\n", at_buffer);
                return ESP01S_ERROR;
            }
            else if (strstr((char*)at_buffer, "FAIL") != NULL)
            {
                DEBUG_PRINT("[ESP01S_SendData] FAIL: %s\r\n", at_buffer);
                return ESP01S_ERROR;
            }
            
            // 可选：输出接收到的原始数据用于调试
            #ifdef ESP01S_DEBUG_VERBOSE
            DEBUG_PRINT("[ESP01S_SendData] 接收: %s\r\n", at_buffer);
            #endif
        }
        
        if (found_arrow) break;
        ESP01S_Delay(10);
    }
    
    if (!found_arrow)
    {
        DEBUG_PRINT("[ESP01S_SendData] 超时：未收到'>'提示符\r\n");
        DEBUG_PRINT("[ESP01S_SendData] 缓冲区内容: %s\r\n", at_buffer);
        return ESP01S_TIMEOUT;
    }
    
    DEBUG_PRINT("[ESP01S_SendData] 发送数据内容，长度=%d\r\n", length);
    ESP01S_ClearBuffer();
    
    // 可选：输出要发送的数据前几个字节用于调试
    #ifdef ESP01S_DEBUG_DATA
    DEBUG_PRINT("[ESP01S_SendData] 数据前16字节: ");
    for (int i = 0; i < (length < 16 ? length : 16); i++)
    {
        DEBUG_PRINT("%02X ", (uint8_t)data[i]);
    }
    DEBUG_PRINT("\r\n");
    #endif
    
    HAL_UART_Transmit(esp_huart, (uint8_t*)data, length, 2000);
    DEBUG_PRINT("[ESP01S_SendData] 数据已发送，等待响应...\r\n");
    
    // start_time = HAL_GetTick();
    // while ((HAL_GetTick() - start_time) < 5000)
    // {
    //     if (at_buffer_index > 0)
    //     {
    //         if (at_buffer_index < AT_BUFFER_SIZE)
    //         {
    //             at_buffer[at_buffer_index] = '\0';
    //         }
            
    //         // 可选：输出接收到的响应用于调试
    //         #ifdef ESP01S_DEBUG_VERBOSE
    //         DEBUG_PRINT("[ESP01S_SendData] 接收响应: %s\r\n", at_buffer);
    //         #endif
            
    //         if (strstr((char*)at_buffer, "SEND OK") != NULL)
    //         {
    //             DEBUG_PRINT("[ESP01S_SendData] 发送成功: SEND OK\r\n");
    //             return ESP01S_OK;
    //         }
    //         else if (strstr((char*)at_buffer, "ERROR") != NULL)
    //         {
    //             DEBUG_PRINT("[ESP01S_SendData] 发送失败: ERROR\r\n");
    //             DEBUG_PRINT("[ESP01S_SendData] 错误详情: %s\r\n", at_buffer);
    //             return ESP01S_ERROR;
    //         }
    //         else if (strstr((char*)at_buffer, "FAIL") != NULL)
    //         {
    //             DEBUG_PRINT("[ESP01S_SendData] 发送失败: FAIL\r\n");
    //             DEBUG_PRINT("[ESP01S_SendData] 失败详情: %s\r\n", at_buffer);
    //             return ESP01S_ERROR;
    //         }
    //     }
    //     ESP01S_Delay(10);
    // }
    
    DEBUG_PRINT("[ESP01S_SendData] 超时：等待SEND OK响应超时\r\n");
    DEBUG_PRINT("[ESP01S_SendData] 缓冲区内容: %s\r\n", at_buffer);
    return ESP01S_TIMEOUT;
}


/**
  * @brief  扩展发送数据
  */
ESP01S_Status ESP01S_SendDataEx(uint8_t link_id, const char *data, uint16_t length)
{
    char cmd[32];
    
    if (link_id == 0)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSENDEX=%d", length);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSENDEX=%d,%d", link_id, length);
    }
    
    ESP01S_Status status = ESP01S_SendCommand(cmd, ">", AT_TIMEOUT_DEFAULT);
    if (status != ESP01S_OK)
        return status;
    
    HAL_UART_Transmit(esp_huart, (uint8_t*)data, length, 2000);
    
    return ESP01S_WaitResponse("SEND OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  发送数据并等待响应
  */
ESP01S_Status ESP01S_SendDataWithResult(uint8_t link_id, const char *data, uint16_t length, uint32_t timeout)
{
    ESP01S_Status status = ESP01S_SendData(link_id, data, length);
    
    if (status == ESP01S_OK)
    {
        uint32_t start_time = HAL_GetTick();
        while ((HAL_GetTick() - start_time) < timeout)
        {
            if (data_available)
            {
                break;
            }
            ESP01S_Delay(10);
        }
    }
    
    return status;
}

/**
  * @brief  获取本地IP地址
  */
ESP01S_Status ESP01S_GetLocalIP(char *ip)
{
    return ESP01S_SendCommand("AT+CIPSTA?", "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  获取多连接模式
  */
ESP01S_Status ESP01S_GetMultiplexMode(uint8_t *mode)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+CIPMUX?", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && mode != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "+CIPMUX:");
        if (ptr != NULL)
        {
            *mode = atoi(ptr + 8);
        }
    }
    return status;
}

/**
  * @brief  设置多连接模式
  */
ESP01S_Status ESP01S_SetMultiplexMode(uint8_t enable)
{
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "AT+CIPMUX=%d", enable);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  创建服务器
  */
ESP01S_Status ESP01S_StartServer(uint16_t port, uint8_t ssl_enable, uint8_t ssl_version)
{
    char cmd[64];
    if (ssl_enable)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d,\"SSL\",%d", port, ssl_version);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d", port);
    }
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  停止服务器
  */
ESP01S_Status ESP01S_StopServer(uint8_t link_id)
{
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=0,%d", link_id);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  进入透传模式
  */
ESP01S_Status ESP01S_EnterPassthroughMode(uint8_t link_id)
{
    char cmd[32];
    ESP01S_Status status;
    
    status = ESP01S_SetTransmissionMode(CIP_MODE_PASSTHROUGH);
    if (status != ESP01S_OK)
        return status;
    
    if (link_id == 0)
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSEND");
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", link_id);
    }
    
    HAL_UART_Transmit(esp_huart, (uint8_t*)cmd, strlen(cmd), 100);
    HAL_UART_Transmit(esp_huart, (uint8_t*)"\r\n", 2, 100);
    
    uint32_t start_time = HAL_GetTick();
    while ((HAL_GetTick() - start_time) < 2000)
    {
        if (at_buffer_index > 0 && at_buffer[at_buffer_index - 1] == '>')
        {
            ESP01S_ClearBuffer();
            return ESP01S_OK;
        }
        ESP01S_Delay(10);
    }
    
    return ESP01S_TIMEOUT;
}

/**
  * @brief  退出透传模式
  */
ESP01S_Status ESP01S_ExitPassthroughMode(void)
{
    ESP01S_Delay(1000);
    
    const char exit_seq[] = "+++";
    HAL_UART_Transmit(esp_huart, (uint8_t*)exit_seq, 3, 100);
    
    ESP01S_Delay(1000);
    ESP01S_ClearBuffer();
    
    uint8_t mode;
    ESP01S_Status status = ESP01S_GetTransmissionMode(&mode);
    if (status == ESP01S_OK && mode == CIP_MODE_NORMAL)
    {
        return ESP01S_OK;
    }
    
    return ESP01S_ERROR;
}

/**
  * @brief  发送数据（透传模式）
  */
ESP01S_Status ESP01S_SendDataPassthrough(const char *data, uint16_t length)
{
    uint8_t mode;
    ESP01S_Status status = ESP01S_GetTransmissionMode(&mode);
    if (status != ESP01S_OK || mode != CIP_MODE_PASSTHROUGH)
    {
        return ESP01S_ERROR;
    }
    
    HAL_UART_Transmit(esp_huart, (uint8_t*)data, length, 2000);
    return ESP01S_OK;
}

/**
  * @brief  简化版发送数据
  */
ESP01S_Status ESP01S_Send(uint8_t link_id, const char *data)
{
    uint16_t length = strlen(data);
    uint8_t mux_mode;
    ESP01S_Status status;
    
    status = ESP01S_GetMultiplexMode(&mux_mode);
    if (status != ESP01S_OK)
        return status;
    
    if (mux_mode == 0)
    {
        return ESP01S_SendData(0, data, length);
    }
    else
    {
        return ESP01S_SendData(link_id, data, length);
    }
}

/**
  * @brief  获取SNTP时间
  */
ESP01S_Status ESP01S_GetSNTPTime(char *time_str)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+CIPSNTPTIME?", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && time_str != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "+CIPSNTPTIME:");
        if (ptr != NULL)
        {
            ptr += 13;
            char *end = strchr(ptr, '\r');
            if (end)
            {
                uint16_t len = end - ptr;
                strncpy(time_str, ptr, len);
                time_str[len] = '\0';
            }
        }
    }
    return status;
}

/**
  * @brief  获取传输模式
  */
ESP01S_Status ESP01S_GetTransmissionMode(uint8_t *mode)
{
    ESP01S_Status status = ESP01S_SendCommand("AT+CIPMODE?", "OK", AT_TIMEOUT_DEFAULT);
    if (status == ESP01S_OK && mode != NULL)
    {
        char *ptr = strstr((char*)at_buffer, "+CIPMODE:");
        if (ptr != NULL)
        {
            *mode = atoi(ptr + 9);
        }
    }
    return status;
}

/**
  * @brief  设置传输模式
  */
ESP01S_Status ESP01S_SetTransmissionMode(ESP01S_CipMode mode)
{
    char cmd[20];
    snprintf(cmd, sizeof(cmd), "AT+CIPMODE=%d", mode);
    return ESP01S_SendCommand(cmd, "OK", AT_TIMEOUT_DEFAULT);
}

/**
  * @brief  设置数据接收回调函数
  */
void ESP01S_SetDataCallback(ESP01S_DataCallback callback)
{
    data_callback = callback;
}

/**
  * @brief  检查是否有数据可用
  */
uint8_t ESP01S_HasData(void)
{
    return data_available;
}

/**
  * @brief  读取接收到的数据
  */
uint16_t ESP01S_ReadData(uint8_t *buffer, uint16_t max_len)
{
    if (!data_available || buffer == NULL)
        return 0;
    
    uint16_t copy_len = (current_packet.length < max_len) ? 
                        current_packet.length : max_len;
    
    memcpy(buffer, current_packet.data, copy_len);
    data_available = 0;
    
    return copy_len;
}

/**
  * @brief  获取当前数据包信息
  */
ESP01S_Packet* ESP01S_GetCurrentPacket(void)
{
    return &current_packet;
}

/**
  * @brief  从缓冲区解析IPD数据包
  */
uint8_t ESP01S_ParseIPD(const char *buffer, uint8_t *link_id, uint16_t *length, const char **data_start)
{
    const char *ptr = buffer;
    
    if (strncmp(ptr, "+IPD,", 5) != 0)
        return 0;
    
    ptr += 5;
    
    if (link_id != NULL)
    {
        *link_id = 0;
        while (*ptr >= '0' && *ptr <= '9')
        {
            *link_id = *link_id * 10 + (*ptr - '0');
            ptr++;
        }
    }
    else
    {
        while (*ptr >= '0' && *ptr <= '9')
            ptr++;
    }
    
    if (*ptr == ',')
        ptr++;
    
    if (length != NULL)
    {
        *length = 0;
        while (*ptr >= '0' && *ptr <= '9')
        {
            *length = *length * 10 + (*ptr - '0');
            ptr++;
        }
    }
    else
    {
        while (*ptr >= '0' && *ptr <= '9')
            ptr++;
    }
    
    if (*ptr != ':')
        return 0;
    
    ptr++;
    
    if (data_start != NULL)
    {
        *data_start = ptr;
    }
    
    return 1;
}

/**
  * @brief  处理接收缓冲区中的IPD数据
  */
uint16_t ESP01S_ProcessBuffer(void)
{
    uint16_t processed = 0;
    char *buffer = (char*)at_buffer;
    
    while (1)
    {
        char *ipd_start = strstr(buffer, "+IPD,");
        if (ipd_start == NULL)
            break;
        
        uint8_t link_id;
        uint16_t length;
        const char *data_start;
        
        if (ESP01S_ParseIPD(ipd_start, &link_id, &length, &data_start))
        {
            char *data_end = (char*)data_start + length;
            if (data_end <= (char*)at_buffer + at_buffer_index)
            {
                if (data_callback != NULL)
                {
                    data_callback(link_id, (uint8_t*)data_start, length);
                }
                
                buffer = data_end;
                processed += length + (data_start - ipd_start);
            }
            else
            {
                break;
            }
        }
        else
        {
            buffer = ipd_start + 1;
        }
    }
    
    if (processed > 0)
    {
        uint16_t remaining = at_buffer_index - processed;
        if (remaining > 0)
        {
            memmove(at_buffer, at_buffer + processed, remaining);
        }
        at_buffer_index = remaining;
        at_buffer[at_buffer_index] = '\0';
    }
    
    return processed;
}

/**
  * @brief  启动自动接收模式
  */
void ESP01S_StartAutoReceive(void)
{
    ESP01S_ClearBuffer();
}

/**
  * @brief  获取最近接收的数据包
  */
ESP01S_Status ESP01S_GetLatestData(uint8_t *link_id, uint8_t *buffer, uint16_t *length, uint16_t max_len)
{
    if (!data_available)
        return ESP01S_BUSY;
    
    if (link_id != NULL)
        *link_id = current_packet.link_id;
    
    if (length != NULL)
        *length = current_packet.length;
    
    if (buffer != NULL && max_len > 0)
    {
        uint16_t copy_len = (current_packet.length < max_len) ? 
                           current_packet.length : max_len;
        memcpy(buffer, current_packet.data, copy_len);
    }
    
    data_available = 0;
    return ESP01S_OK;
}

/**
  * @brief  延时函数
  */
void ESP01S_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
  * @brief  获取响应缓冲区
  */
char* ESP01S_GetResponseBuffer(void)
{
    return (char*)at_buffer;
}

/**
  * @brief  获取响应长度
  */
uint16_t ESP01S_GetResponseLength(void)
{
    return at_buffer_index;
}

/**
  * @brief  打印缓冲区内容
  */
void ESP01S_PrintBuffer(void)
{
    printf("Buffer[%d]: ", at_buffer_index);
    for (uint16_t i = 0; i < at_buffer_index; i++)
    {
        if (at_buffer[i] >= 32 && at_buffer[i] <= 126)
        {
            printf("%c", at_buffer[i]);
        }
        else
        {
            printf("[0x%02X]", at_buffer[i]);
        }
    }
    printf("\r\n");
}

/**
  * @brief  检查是否接收到特定字符串
  */
uint8_t ESP01S_CheckResponse(const char *str)
{
    if (at_buffer_index == 0) return 0;
    
    if (at_buffer_index < AT_BUFFER_SIZE)
    {
        at_buffer[at_buffer_index] = '\0';
    }
    
    return (strstr((char*)at_buffer, str) != NULL);
}