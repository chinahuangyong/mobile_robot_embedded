/***********************************************************************
**
**
** Copyright (C) 2013 LP-Research
** All rights reserved.
** Contact: LP-Research (info@lp-research.com)
***********************************************************************/

#ifndef __BSP__LPSENSOR_H
#define __BSP__LPSENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

////LPMS Command list
//#define COMMAND_START_ADDRESS   0
//// Acknowledged and Not-acknowledged identifier
//#define REPLY_ACK                           (COMMAND_START_ADDRESS + 0)
//#define REPLY_NACK                          (COMMAND_START_ADDRESS + 1)
////Sensor info
//#define GET_FIRMWARE_VERSION                (COMMAND_START_ADDRESS + 4)
//#define GET_HARDWARE_VERSION                (COMMAND_START_ADDRESS + 5)
//#define GET_SERIAL_NUMBER                   (COMMAND_START_ADDRESS + 6)
//// Configuration and status
//#define GET_CONFIG                          (COMMAND_START_ADDRESS + 7)
//#define GET_STATUS                          (COMMAND_START_ADDRESS + 8)
//// Mode switching
//#define GOTO_COMMAND_MODE                   (COMMAND_START_ADDRESS + 9)
//#define GOTO_STREAM_MODE                    (COMMAND_START_ADDRESS + 10)
//// Data transmission
//#define GET_SENSOR_DATA                     (COMMAND_START_ADDRESS + 11)
//#define GET_STREAM_FREQ                     (COMMAND_START_ADDRESS + 12)
//#define SET_STREAM_FREQ                     (COMMAND_START_ADDRESS + 19)
//
//
//// Sensor reset
//#define RESET_BIAS                         (COMMAND_START_ADDRESS + 23)
//#define RESET_HEADING                      (COMMAND_START_ADDRESS + 24)
//#define RESET_SENSOR                       (COMMAND_START_ADDRESS + 25)
//
//// Set Angle Output Range
//#define SET_360_OUTPUT                      (COMMAND_START_ADDRESS + 14)
//#define SET_180_OUTPUT                      (COMMAND_START_ADDRESS + 15)
//// UART
//#define SET_UART_BAUDRATE                   (COMMAND_START_ADDRESS + 20)
//#define GET_UART_BAUDRATE                   (COMMAND_START_ADDRESS + 21)
//// Software Sync
//#define START_SYNC                          (COMMAND_START_ADDRESS + 96)
//#define STOP_SYNC                           (COMMAND_START_ADDRESS + 97)
//
//// Stream frequency data values
//#define LPMS_STREAM_FREQ_10HZ                   (uint32_t)10
//#define LPMS_STREAM_FREQ_25HZ                   (uint32_t)25
//#define LPMS_STREAM_FREQ_50HZ                   (uint32_t)50
//#define LPMS_STREAM_FREQ_100HZ                  (uint32_t)100

//#define LPMS_DATA_MASK  (uint32_t)(0x17BF<<9)
                                                        
#define LPMS_CMD_PACKET_MAX_SIZE     100
#define MAX_PACKET_DATA_LENGTH       100

typedef struct _LpmsModbusPacket
{
    uint8_t start;
    uint16_t sensorID;
    uint16_t cmd;
    uint16_t len;
    uint8_t data[LPMS_CMD_PACKET_MAX_SIZE];
    uint16_t sumCheck;
    uint16_t end;
} LpmsPacket;

typedef struct _LpmsPacket
{
	LpmsPacket LpmsDataPacket;
	uint64_t timestamp;
}LpmsTimeStampPacket;

typedef struct
{
	float ts;
    float gyroX;
    float gyroY;
    float gyroZ;
    float accX;
    float accY;
    float accZ;
    float magX;
    float magY;
    float magZ;
    float qw;
    float qx;
    float qy;
    float qz;
    float eulerX;
    float eulerY;
    float eulerZ;
    float LinAccX;
    float LinAccY;
    float LinAccZ;
} LpmsData_t;

typedef struct
{
    uint32_t Config;
    uint32_t Status;
    uint32_t StreamFreq;
    uint32_t Com_Mode;
    void *pData;
} LpmsHandle_t;


/* 32bit的整形数字转化成float需要的数据结构 */
typedef union
{
	uint32_t data_u32;
	float data_float;
}u322float;

u322float data;

#define R_TO_D    (57.295780f)//Radian to Degree

extern QueueHandle_t uartRecQueue;
extern LpmsTimeStampPacket lpmsTimeStampPacket;
extern LpmsData_t lpmsRawData;

uint8_t bsp_lpms_init(void);
uint8_t bsp_lpms_setCommunicationMode(uint32_t mode);
uint8_t bsp_lpms_setStreamFreq(uint32_t freq);
uint32_t bsp_lpms_getConfig(uint16_t id);

uint8_t bsp_lpms_getFrame(uint8_t data);
uint8_t bsp_lpms_isGetFrame(void);

uint8_t bsp_lpms_resetToFactory(void);
uint8_t bsp_lpms_setToAngle(void);

uint8_t bsp_lpms_getPacket(void);
uint8_t bsp_lpms_parsePacket(void);
uint8_t bsp_lpms_parseSensorData(void);
float int2float(float * _f,uint8_t * _u8);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif/*__LPSENSOR_H*/
