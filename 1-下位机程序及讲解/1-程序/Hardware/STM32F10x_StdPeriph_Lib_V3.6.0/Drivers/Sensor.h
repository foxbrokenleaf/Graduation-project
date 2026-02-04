#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "stm32f10x.h"

typedef struct __SENSOR_STRUCT__
{
    // uint8_t DriverCombustibleGas;
    uint16_t DriverFire;
    uint16_t DriverSmoke;
    uint8_t DriverTemptrue;
    uint8_t DriverHumidity;
    uint16_t DriverInfraredBeam;
}sensor_;

typedef struct __CTRL_DRIVER_STRUCT__
{
    uint8_t Driver_Led;
    uint8_t Driver_Buzzer;
    uint8_t Driver_Relay_1; //½µÎÂ¼ÌµçÆ÷
    uint8_t Driver_Relay_2; //³ýÊª¼ÌµçÆ÷
    uint8_t Driver_Relay_3; //Ãð»ð¼ÌµçÆ÷
    uint8_t Driver_Relay_4; //ÅÅ·ç¼ÌµçÆ÷
    uint8_t Driver_AudioPlay;
    uint8_t Driver_Motor;
}ctrl_driver_;


extern sensor_ SensorValue;
extern sensor_ SensorWarn;
extern ctrl_driver_ CtrlDriver;
extern uint8_t Run_Mode;

uint16_t ADC_ReadMQ2Sensor(void);
uint16_t ADC_ReadFireSensor(void);
uint8_t ReadInfraredRangingSensor(void);

#endif