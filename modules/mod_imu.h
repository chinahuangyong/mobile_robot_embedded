/*
 * imu.h
 *
 *  Created on: 2019��7��6��
 *      Author: HuangYong
 */

#ifndef SRC_MOD_IMU_H_
#define SRC_MOD_IMU_H_

#include <stdint.h>
#include <stdio.h>

extern float roll;
extern float pitch;

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

uint8_t mod_imu_init(float _kp, float _ki, float _halfT);
uint8_t mod_imu_update(float gx, float gy, float gz, float ax, float ay, float az);
uint8_t mid_imu_get_state(imu_data_t* _imu_data);

#endif /* SRC_TASK_IMU_H_ */
