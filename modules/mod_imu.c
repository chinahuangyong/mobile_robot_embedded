/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_motor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *
  * @history
  *          2019-7-12 HuangYong 创建文件
*********************************************************************************/
#include <math.h>

#include "mod_imu.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

float Kp = 0.2f;
float Ki=0.001f;
float halfT = 0.005f;

//四元数初始化
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
//积分变量初始化
static float exInt = 0, eyInt = 0, ezInt = 0;
//imu数据
static imu_data_t imu_data;


uint8_t mod_imu_init(float _kp, float _ki, float _halfT)
{
	Kp = _kp;
	Ki = _ki;
	halfT = _halfT;
}

uint8_t mod_imu_update(float gx, float gy, float gz, float ax, float ay, float az)
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;

	norm = sqrt(ax*ax + ay*ay + az*az);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	exInt = exInt + ex*Ki*halfT;
	eyInt = eyInt + ey*Ki*halfT;
	ezInt = ezInt + ez*Ki*halfT;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	imu_data.q0 = q0;
	imu_data.q1 = q1;
	imu_data.q2 = q2;
	imu_data.q3 = q3;

//	imu_data.yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; // yaw
	// pitch获取，单位为°
	imu_data.pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.32484; 
	// roll获取，单位为°
	imu_data.roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.32484; 

	return 0;
}

uint8_t mid_imu_get_state(imu_data_t* _imu_data)
{
	memcpy(_imu_data, &imu_data, sizeof(imu_data_t));
	return 0;
}

