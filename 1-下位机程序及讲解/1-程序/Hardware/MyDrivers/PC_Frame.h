#ifndef __PC_FRAME_H__
#define __PC_FRAME_H__

#define FRAME_HEAD_1    0xaa
#define FRAME_HEAD_2    0x55
// DATA_LEN
#define DRIVER_MQ_9       0x01      //  0xff 0xff
#define DRIVER_MQ_135     0x02      //  0xff 0xff
#define DRIVER_DHT11      0x03      //  0xff
#define SET_CO_WARM          0x04      //  0xff
#define SET_FIRE_WARM          0x05      //  0xff
#define SET_AIR_LEVEL_WARM          0x06      //  0xff
#define SET_TEMPTRUE_WARM          0x07      //  0xff
#define DRIVER_Fire                 0x08
// DATA_CRC
#define FRAME_END_1     0x55
#define FRAME_END_2     0xaa


void Send_Driver_Info(unsigned char UUID);
unsigned char Receive_PC_Value(const unsigned char* DataBuff, unsigned short buffSize);

#endif