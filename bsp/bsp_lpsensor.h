/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_lpsensor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该文件为IMU的协议解析文件，该文件目前针对LPMS-ALTTL驱动，后续会增加其他驱动
  * @history
  *          2019-7-15 HuangYong 创建文件
*********************************************************************************/

#ifndef __BSP_LPSENSOR_H
#define __BSP_LPSENSOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
                              
#define MAX_PACKET_DATA_LENS       100

//LPMS Modbus数据包结构体
typedef struct lpmsttl_packet_struct
{
    uint8_t start;                          
    uint16_t sensor_id;    
    uint16_t cmd;                           
    uint16_t len;
    uint8_t data[MAX_PACKET_DATA_LENS];
    uint16_t sum_check;
    uint16_t end;
} lpmsttl_packet_t;

//LPMS数据存储结构体
typedef struct lpmsttl_data_struct
{
	float sensor_ts;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
    float mag_x;
    float mag_y;
    float mag_z;
    float qw;
    float qx;
    float qy;
    float qz;
    float euler_x;
    float euler_y;
    float euler_z;
    float lin_acc_x;
    float lin_acc_y;
    float lin_acc_z;
} lpmsttl_data_t;

//包含本地时间戳的数据包
typedef struct lpmsttl_data_ts_struct
{
	lpmsttl_data_t lpmsttl_data;
	uint64_t local_ts;  //本地时间戳
}lpmsttl_data_ts_t;

/* @beief     lpms初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_lpmsttl_init(void);

/* @beief     lpms获取完整一帧数据，该协议解析主要帧对lpms-ttlal2
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_get_frame(void);

/* @beief     lpms获取完整一帧数据
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_get_packet(void);

/* @beief     lpms解析数据包
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_parse_packet(void);

/* @beief     lpms解析数据，获取对应的IMU的数据值
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_parse_data(void);

/* @beief     lpms解析数据，获取对应的IMU的数据值
 * @return    0 - 成功 <0 表示失败
 * */
int bsp_lpmsttl_get_data(lpmsttl_data_ts_t* lpmsttl_data_ts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif/*__LPSENSOR_H*/
