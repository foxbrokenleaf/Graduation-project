#include "stm32f10x.h"                  // Device header
#include "DHT11.h"
#include "delay.h"

uint8_t DHT11_ReadData(DHT11_Data *dat)
{
	DHT11_Rst();		//发送信号
	GPIO_SetBits(DHT11_PROT,DHT11_PIN);		//拉高电平
	
	if(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 0)		//判断DHT11是否有响应
	{
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 0);	//等待低电平结束
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 1);	//等待高电平结束
		
		uint8_t Data[5] = {0};
		for(int i=0;i<5;i++)		//循环读取5次
		{
			Data[i] = DHT11_ReadByte();
		}
		
		GPIO_ResetBits(DHT11_PROT,DHT11_PIN);		//拉低电平
		Delay_us(55);			//延迟等待
		GPIO_SetBits(DHT11_PROT,DHT11_PIN);		//拉高电平
		
		if(Data[0]+Data[1]+Data[2]+Data[3] == Data[4])		//数据校验
		{
			dat->humi_int = Data[0];     //湿度整数部分
			dat->humi_dec = Data[1];     //湿度小数部分
			dat->temp_int = Data[2];     //温度整数部分
			dat->temp_dec = Data[3];     //温度小数部分
			return 1;
		}
		
		return 1;  // 校验成功
	}
	return 2;  // 通信失败
}

uint8_t DHT11_ReadByte(void)
{
	uint8_t Byte=0;
	for(int i=0;i<8;i++)		//循环读取8次 一个字节
	{
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 0);	//等待上一个低电平结束
		
		Delay_us(30);	//延迟30us 再判断
		
		Byte <<= 1;		//数据左移1位 准备接收新的bit
		
		if(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 1)		//如果此时是高电平 证明发送的是1 否则是0
		{
			Byte |= 1;		//或上新的bit
		}
		
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11_PIN) == 1);	//等待高电平结束
	}
	
	return Byte;
}
 
void DHT11_Rst(void)
{
	Master_OutputMode();		//主机切换为输出模式
	GPIO_ResetBits(DHT11_PROT,DHT11_PIN);		//输出低电平
	Delay_ms(20);		//至少20ms
	GPIO_SetBits(DHT11_PROT,DHT11_PIN);		//输出高电平
	Delay_us(30);		//拉高20-40us
	Master_InputMode();		//主机切换为输入模式
}
 
void Master_OutputMode(void)		//主机输出模式		用于发送指令
{
	GPIO_InitTypeDef GPIO_InitStrcuture;
	GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrcuture.GPIO_Pin = DHT11_PIN;
	GPIO_InitStrcuture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_PROT,&GPIO_InitStrcuture);
}
 
void Master_InputMode(void)			//主机输入模式		用于接收数据
{
	GPIO_InitTypeDef GPIO_InitStrcuture;
	GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStrcuture.GPIO_Pin = DHT11_PIN;
	GPIO_InitStrcuture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_PROT,&GPIO_InitStrcuture);
}
 
 
void DHT11_Init(void)
{
	//开启GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//开启默认输入模式
	GPIO_InitTypeDef GPIO_InitStrcuture;
	GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStrcuture.GPIO_Pin = DHT11_PIN;
	GPIO_InitStrcuture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_PROT,&GPIO_InitStrcuture);
}
