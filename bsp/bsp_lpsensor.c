/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_lpsensor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.2
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该文件为IMU的协议解析文件，该驱动针对型号lpms-ttlal2
  * @history
  *          2019-7-10 HuangYong 创建文件
  * 
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bsp_lpsensor.h"
#include "bsp_uart0.h"

uint8_t rx_buffer[MAX_PACKET_LENS] = {0};
lpmsttl_packet_t lpmsttl_packet;
lpmsttl_data_ts_t lpmsttl_data_ts;

/* @beief     lpms将整数转化成浮点数，此处注意和类型强制转换的区别
 * @return    0 - 成功 <0 表示返回失败
 * */
static float uint2float(uint32_t data_u32)
{
	float data_f32;
    memcpy(&data_f32, &data_u32, 4);
}

/* @beief     lpms数据发送函数
 * @return    0 - 成功 <0 表示返回失败
 * */
static int bsp_lpmsttl_send(uint8_t* p_data, uint16_t size)
{
	bsp_uart0_send(p_data, size);

	return 1;
}

/* @beief     lpms数据接收函数
 * @param[out]p_data - 接收数据变量指针
 * @return    0 - 成功 <0 表示返回失败
 * @note      注意，该函数在线程调用时会被阻塞
 * */
static int bsp_lpmsttl_recv(uint8_t* p_data)
{
	uint32_t data;
	if (xQueueReceive(uart_rec_queue, &data, portMAX_DELAY) == pdTRUE)
	{
		*p_data = (uint8_t)data;
		return 0;
	}
	else
	{
		*p_data = 0;
		return -1;
	}
}

/* @beief     lpms初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_lpmsttl_init(void)
{
	int ret;
	ret = bsp_uart0_init();
    return ret;
}

/* @beief     lpms获取完整一帧数据，该协议解析主要帧对lpms-ttlal2
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_get_frame(void)
{
	static uint16_t rx_buffer_cnt = 0;
	static uint16_t rx_data_lens = 0;

	uint8_t data;

	if(bsp_lpmsttl_recv(&data) == 0)
	{
		rx_buffer[rx_buffer_cnt] = data;
    
		//判断数据是否为帧头
		if(rx_buffer[0] != 0x3A)
		{
			rx_buffer_cnt = 0;
			rx_data_lens = 0;
			memset(rx_buffer, 0, sizeof(rx_buffer));

			goto unsuccess;
		}
		else
		{
			if (rx_buffer_cnt == 6) //获取数据段的长度
			{
				rx_data_lens = rx_buffer[5] + ((uint16_t)(rx_buffer[6]) << 8);
				if(rx_data_lens > 80)
				{
					rx_data_lens = 0;
					rx_buffer_cnt = 0;
					memset(rx_buffer, 0, sizeof(rx_buffer));
					goto unsuccess;
				}
			}
			else if(rx_buffer_cnt > 6)
			{
				if(rx_buffer_cnt == (rx_data_lens + 10))
				{
					//校验数据段
					if((rx_buffer[rx_buffer_cnt - 1] == 0x0D) && (rx_buffer[rx_buffer_cnt] == 0x0A))
					{
						rx_buffer_cnt = 0;
						rx_data_lens = 0;
						return 0;
					}
					else
					{
						rx_buffer_cnt = 0;
						rx_data_lens = 0;
						memset(rx_buffer, 0, sizeof(rx_buffer));
						goto unsuccess;
					}
				}
			}
			rx_buffer_cnt++;
		}
	}
	else
	{
		goto unsuccess;
	}
	
unsuccess:
	return -1;
}

/* @beief     lpms获取完整一帧数据
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_get_packet(void)
{
    uint16_t check_sum = 0;
    uint16_t i = 0;
    /* 获取帧头*/
    lpmsttl_packet.start = rx_buffer[0];
    /* 获取传感器ID */
    lpmsttl_packet.sensor_id = rx_buffer[1] | ((uint16_t)(rx_buffer[2])<<8);
    /* 获取命令 */
    lpmsttl_packet.cmd =  rx_buffer[3] | ((uint16_t)(rx_buffer[4])<<8);
    /* 获取数据段的数据长度 */
    lpmsttl_packet.len = rx_buffer[5] | ((uint16_t)(rx_buffer[6])<<8);
    /* 将数据段的数据进行拷贝 */
    for(i = 0; i < lpmsttl_packet.len; i++)
    {
    	lpmsttl_packet.data[i] = rx_buffer[7 + i];
    }
    /* 计算校验值 */
    for(i = 1; i< 7+lpmsttl_packet.len; i++)
    {
        check_sum += lpmsttl_packet.data[i];
    }

    lpmsttl_packet.sum_check = lpmsttl_packet.data[lpmsttl_packet.len+7] |
                            ((uint16_t)(lpmsttl_packet.data[lpmsttl_packet.len+8])<<8);
    /* 校验校验值 */
    if(check_sum != lpmsttl_packet.sum_check)
    {
        memset(rx_buffer, 0, sizeof(rx_buffer));
        memset(&lpmsttl_packet, 0, sizeof(lpmsttl_packet_t));
        return -1;
    }
    /* 获取帧尾 */
    lpmsttl_packet.end = (uint16_t)((lpmsttl_packet.data[9 + lpmsttl_packet.len] << 8) |
    		lpmsttl_packet.data[10 + lpmsttl_packet.len]);

    memset(rx_buffer, 0, sizeof(rx_buffer));

    return 0;
}

/* @beief     lpms解析数据包
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_parse_packet(void)
{
    if(!bsp_lpmsttl_get_packet())
        return -1;

    switch(lpmsttl_packet.cmd)
    {
    case 0x0009:    //
    	bsp_lpmsttl_parse_data();
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
    return 0;
}

/* @beief     lpms解析数据，获取对应的IMU的数据值
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_parse_data(void)
{
	uint32_t data_u32;

	lpmsttl_data_ts.lpmsttl_data.sensor_ts = (((uint32_t)lpmsttl_packet.data[3])<<24 | ((uint32_t)lpmsttl_packet.data[2])<<16 |
			((uint32_t)lpmsttl_packet.data[1])<<8 | ((uint32_t)lpmsttl_packet.data[0]))*0.0025;

	data_u32 = (((uint32_t)lpmsttl_packet.data[7])<<24 | ((uint32_t)lpmsttl_packet.data[6])<<16 |
			   ((uint32_t)lpmsttl_packet.data[5])<<8 | ((uint32_t)lpmsttl_packet.data[4]));
	lpmsttl_data_ts.lpmsttl_data.gyro_x = uint2float(data_u32);

	data_u32 = (((uint32_t)lpmsttl_packet.data[11])<<24 | ((uint32_t)lpmsttl_packet.data[10])<<16 |
			((uint32_t)lpmsttl_packet.data[9])<<8 | ((uint32_t)lpmsttl_packet.data[8]));
	lpmsttl_data_ts.lpmsttl_data.gyro_y = uint2float(data_u32);

	data_u32 = (((uint32_t)lpmsttl_packet.data[15])<<24 | ((uint32_t)lpmsttl_packet.data[14])<<16 |
			((uint32_t)lpmsttl_packet.data[13])<<8 | ((uint32_t)lpmsttl_packet.data[12]));
	lpmsttl_data_ts.lpmsttl_data.gyro_z = uint2float(data_u32);

	data_u32 = (((uint32_t)lpmsttl_packet.data[19])<<24 | ((uint32_t)lpmsttl_packet.data[18])<<16 |
			((uint32_t)lpmsttl_packet.data[17])<<8 | ((uint32_t)lpmsttl_packet.data[16]));
	lpmsttl_data_ts.lpmsttl_data.acc_x = uint2float(data_u32);

	data_u32 = (((uint32_t)lpmsttl_packet.data[23])<<24 | ((uint32_t)lpmsttl_packet.data[22])<<16 |
			((uint32_t)lpmsttl_packet.data[21])<<8 | ((uint32_t)lpmsttl_packet.data[20]));
	lpmsttl_data_ts.lpmsttl_data.acc_y = uint2float(data_u32);

	data_u32 = (((uint32_t)lpmsttl_packet.data[27])<<24 | ((uint32_t)lpmsttl_packet.data[26])<<16 |
			((uint32_t)lpmsttl_packet.data[25])<<8 | ((uint32_t)lpmsttl_packet.data[24]));
	lpmsttl_data_ts.lpmsttl_data.acc_z = uint2float(data_u32);

	return 0;
}
