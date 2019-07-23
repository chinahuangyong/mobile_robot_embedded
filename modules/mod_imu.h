/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_motor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief   IMU状态解算相关操作文件
  *
  * @history
  *          2019-7-12 HuangYong 创建文件
*********************************************************************************/

#ifndef __MOD_IMU_H_
#define __MOD_IMU_H_

#include <stdint.h>
#include <stdio.h>

typedef struct imu_data_struct
{
	float roll;
	float pitch;
	float yaw;
	float q0;
	float q1;
	float q2;
	float q3;
}imu_data_t;

/* @beief     imu计算函数初始化
 * @return    0 - 成功 <0 表示返回失败
 * */
int mod_imu_init(float _kp, float _ki, float _halfT);

/* @beief     imu计算核心代码
 * @return    0 - 成功 <0 表示返回失败
 * */
int mod_imu_update(float gx, float gy, float gz, float ax, float ay, float az);

/* @beief     获取imu的计算数据
 * @return    0 - 成功 <0 表示返回失败
 * */
int mid_imu_get_data(imu_data_t* _imu_data);

#endif /* SRC_TASK_IMU_H_ */
