/***********************************************************************
**
**
** Copyright (C) 2013 LP-Research
** All rights reserved.
** Contact: LP-Research (info@lp-research.com)
***********************************************************************/

/***********************************************************************
**
**  This file is platform independent
***********************************************************************/

#include "bsp_lpsensor.h"
#include "bsp_uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

LpmsData_t lpmsRawData;

uint8_t rxBuffer[MAX_PACKET_DATA_LENGTH] = {0};
uint8_t isGetFrame = 0;

//静态变量
static LpmsPacket rxPacket;
static LpmsHandle_t hme1;

LpmsTimeStampPacket lpmsTimeStampPacket;


/* 32位的整数转化成浮点数 */
static float uint2float(uint32_t data_tmp)
{
	data.data_u32 = data_tmp;
	return data.data_float;
}

static uint8_t bsp_lpms_send(uint8_t* pData, uint16_t size)
{
	bsp_uart_send(pData, size);

	return 1;
}

/**
  *@brief: get a transmit buffer that contain a command packet
  *@param cmd: LPMS sensor command number
  *@param length: transmit data length, must be 0(no data) or 4(one word) in bytes
  *@param data: transmit data
  *@param buffer: pointer to save transmit packet buffer
  *@param buffer_size: pointer to save transmit buffer size
  *@ret: 1
  */
static uint8_t bsp_lpms_getCmdTxBuffer(uint16_t cmd,uint16_t length,uint32_t data,
									   uint8_t *buffer,uint8_t *buffer_size)
{
    uint16_t check_sum = 0,i = 0;

    uint8_t *p;

    buffer[0] = 0x3A;
    buffer[1] = (cmd & 0xFF);
    //buffer[2] = 0x01 ;
    buffer[2] = (length & 0xFF);


    if(length != 0)
    {
        p = (uint8_t *)&data;
        for(i = 0; i < 4; i++)
        {
            buffer[3+i] = p[i];
        }
    }

    for(i = 1; i < 3 + length; i++)
    {
        check_sum += buffer[i];
    }

    buffer[3 + length] = (check_sum & 0xFF);

    buffer[4 + length] = 0x0D;
    buffer[5 + length] = 0x0A;

    *buffer_size = 6 + length;

    return 1;
}

/**
  *@brief: send a command to imu
  *@param cmd: LPMS sensor command number
  *@param length: transmit data length, must be 0 or 4 in bytes
  *@param data: transmit data
  *@ret:
  */
static uint8_t bsp_lpms_sendCmd(uint16_t cmd,uint16_t length,uint32_t data)
{
    uint8_t txBuffer[LPMS_CMD_PACKET_MAX_SIZE];
    uint8_t size = 0;

    bsp_lpms_getCmdTxBuffer(cmd, length, data, txBuffer, &size);

    if(size != 0)
    {
    	bsp_lpms_send(txBuffer, size);   //
        return 1;
    }
    else
        return 0;
}

//LPMS初始化
uint8_t bsp_lpms_init(void)
{
	bsp_uart_init();

    return 1;
}

/**
  *@brief: set communication mode
  *@param mode: GOTO_COMMAND_MODE or GOTO_STREAM_MODE
  *@ret:
  */
uint8_t bsp_lpms_setCommunicationMode(uint32_t mode)
{
    hme1.Com_Mode = mode;
    bsp_lpms_sendCmd(mode, 0, 0);
    return 1;
}

uint8_t bsp_lpms_setStreamFreq(uint32_t freq)
{
//    hme1.StreamFreq = freq;
//    bsp_lpms_sendCmd(SET_STREAM_FREQ, 4,freq);
    return 1;
}

uint8_t LPMS_resetToFactory(void)
{
//	bsp_lpms_sendCmd(RESET_SENSOR, 0, 0);
    return 1;
}

uint8_t LPMS_setToAngle(void)
{
//	bsp_lpms_sendCmd(SET_360_OUTPUT, 0, 0);
	return 1;
}

uint32_t bsp_lpms_getConfig(uint16_t id)
{
//    LPMS_sendCmd(GET_CONFIG, 0,0);
//    while(rxCompelete == 0) ;
//   /* if(communication_mode ==  4)
//    {
//
//         rxSize =  10 ;
//    }
//    else
//    {
//         rxSize =  6 ;
//    }*/
//    LPMS_getPacket();
//    if(rxPacket.function == GET_CONFIG)
//    {
//        hme1.Config = *(uint32_t *)rxPacket.data;
//        return hme1.Config;
//    }
//    else
//        return 0;
	return 0;
}

uint8_t bsp_lpms_getFrame(uint8_t data)
{
	static uint16_t rxBufferCnt = 0;
	static uint16_t rxLength = 0;

	rxBuffer[rxBufferCnt] = data;

	if(rxBuffer[0] != 0x3A)
	{
		rxBufferCnt = 0;
		rxLength = 0;
		memset(rxBuffer, 0, MAX_PACKET_DATA_LENGTH);
	}
	else
	{
		if (rxBufferCnt == 6)
		{
			rxLength = rxBuffer[5] + ((uint16_t)(rxBuffer[6])<<8);
			if(rxLength > 80)
			{
				rxLength = 0;
				rxBufferCnt = 0;
				memset(rxBuffer, 0, MAX_PACKET_DATA_LENGTH);
				return 0;
			}
		}
		else if(rxBufferCnt > 6)
		{
			if(rxBufferCnt == (rxLength + 10))
			{
				if((rxBuffer[rxBufferCnt - 1] == 0x0D) && (rxBuffer[rxBufferCnt] == 0x0A))
				{
					isGetFrame = 1;
					rxBufferCnt = 0;
					rxLength = 0;
					return 0;
				}
				else
				{
					rxBufferCnt = 0;
					rxLength = 0;
					memset(rxBuffer, 0, MAX_PACKET_DATA_LENGTH);
					return 0;
				}
			}
		}

		rxBufferCnt++;
	}

	return 0;
}

uint8_t bsp_lpms_isGetFrame(void)
{
	if(isGetFrame)
	{
		isGetFrame = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  *@brief: get a lpms packet from recieved data buffer
  *@param:
  *@ret:
  */
uint8_t bsp_lpms_getPacket(void)
{
    uint16_t check_sum = 0;
    uint16_t i = 0;
    /* 获取帧头 */
    rxPacket.start = rxBuffer[0];
    /* 获取传感器的ID号 */
    rxPacket.sensorID = rxBuffer[1] + ((uint16_t)(rxBuffer[2])<<8);
    /* 获取传感器的命令 */
    rxPacket.cmd =  rxBuffer[3] + ((uint16_t)(rxBuffer[4])<<8);
    /* 获取传感器的数据长度 */
    rxPacket.len = rxBuffer[5] + ((uint16_t)(rxBuffer[6])<<8);
    /* 获取传感器的数据值 */
    for(i = 0; i < rxPacket.len; i++)
    {
        rxPacket.data[i] = rxBuffer[7 + i];
    }
    /* 对传感器数据进行校验计算 */
    for(i = 1; i< 7+rxPacket.len; i++)
    {
        check_sum += rxBuffer[i];
    }
    //check_sum =(check_sum&0x00FF);
    rxPacket.sumCheck = rxBuffer[rxPacket.len+7] + ((uint16_t)(rxBuffer[rxPacket.len+8])<<8);
    /* 验证和校验是否通过 */
    if(check_sum != rxPacket.sumCheck)
    {
        memset((uint8_t *)&rxPacket,0,sizeof(rxPacket));
        memset(rxBuffer, 0, MAX_PACKET_DATA_LENGTH);
        return 0;
    }
    /* 获取帧尾 */
    rxPacket.end = (uint16_t)((rxBuffer[9 + rxPacket.len] << 8) |
    		rxBuffer[10 + rxPacket.len]);
    memset(rxBuffer, 0, MAX_PACKET_DATA_LENGTH);

    return 1;
}


uint8_t bsp_lpms_parsePacket(void)
{
    if(!bsp_lpms_getPacket())
        return 0;

    switch(rxPacket.cmd)
    {
    case 0x0009:
    	bsp_lpms_parseSensorData();
        break;
//    case GET_CONFIG:
////        hme1.Config = *(uint32_t *)rxPacket.data;
//        break;
//    case REPLY_ACK:
//        break;
//    case REPLY_NACK:
//        break;
    default:
        break;
    }
    return 1;
}

uint8_t bsp_lpms_parseSensorData(void)
{
	uint32_t data_tmp;
	lpmsRawData.ts = (((uint32_t)rxPacket.data[3])<<24 | ((uint32_t)rxPacket.data[2])<<16 |
			         ((uint32_t)rxPacket.data[1])<<8 | ((uint32_t)rxPacket.data[0]))*0.0025;

//	xil_printf("lpmsRawData ts is %u\r\n", (uint32_t)(lpmsRawData.ts*1000000));
//	printf("lpmsRawData ts is %f\r\n", (lpmsRawData.ts));

	data_tmp = (((uint32_t)rxPacket.data[7])<<24 | ((uint32_t)rxPacket.data[6])<<16 |
			   ((uint32_t)rxPacket.data[5])<<8 | ((uint32_t)rxPacket.data[4]));
	lpmsRawData.gyroX = uint2float(data_tmp);

	data_tmp = (float)(((uint32_t)rxPacket.data[11])<<24 | ((uint32_t)rxPacket.data[10])<<16 |
					   ((uint32_t)rxPacket.data[9])<<8 | ((uint32_t)rxPacket.data[8]));
	lpmsRawData.gyroY = uint2float(data_tmp);

	data_tmp = (float)(((uint32_t)rxPacket.data[15])<<24 | ((uint32_t)rxPacket.data[14])<<16 |
					   ((uint32_t)rxPacket.data[13])<<8 | ((uint32_t)rxPacket.data[12]));
	lpmsRawData.gyroZ = uint2float(data_tmp);

	data_tmp = (float)(((uint32_t)rxPacket.data[19])<<24 | ((uint32_t)rxPacket.data[18])<<16 |
					   ((uint32_t)rxPacket.data[17])<<8 | ((uint32_t)rxPacket.data[16]));
	lpmsRawData.accX = uint2float(data_tmp);

	data_tmp = (float)(((uint32_t)rxPacket.data[23])<<24 | ((uint32_t)rxPacket.data[22])<<16 |
					   ((uint32_t)rxPacket.data[21])<<8 | ((uint32_t)rxPacket.data[20]));
	lpmsRawData.accY = uint2float(data_tmp);

	data_tmp = (float)(((uint32_t)rxPacket.data[27])<<24 | ((uint32_t)rxPacket.data[26])<<16 |
					   ((uint32_t)rxPacket.data[25])<<8 | ((uint32_t)rxPacket.data[24]));
	lpmsRawData.accZ = uint2float(data_tmp);

//	printf("lpmsRawData:%f\t%f\t%f\t%f\t%f\t%f\r\n", lpmsRawData.gyroX, lpmsRawData.gyroY, lpmsRawData.gyroZ,
//			lpmsRawData.accX, lpmsRawData.accY, lpmsRawData.accZ);

	return 1;

}

