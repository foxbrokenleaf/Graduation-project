#ifndef __DHT11_H
#define __DHT11_H
 
#define DHT11_PROT	GPIOB
#define DHT11_PIN		GPIO_Pin_11

typedef struct DHT11DATA
{
    uint8_t temp_int;
    uint8_t temp_dec;
    uint8_t humi_int;
    uint8_t humi_dec;
}DHT11_Data;


uint8_t DHT11_ReadData(DHT11_Data *dat);
uint8_t DHT11_ReadByte(void);
void DHT11_Rst(void);
void Master_OutputMode(void);
void Master_InputMode(void);
void DHT11_Init(void);
 
#endif
