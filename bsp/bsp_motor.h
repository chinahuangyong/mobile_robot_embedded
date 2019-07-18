/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_motor.h
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *			 该文件为电机驱动的接口程序
  * @history
  *          2019-7-12 HuangYong 创建文件
*********************************************************************************/
#ifndef _BSP_MOTOR_H_
#define _BSP_MOTOR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct bsp_motor_struct
{
	int16_t left_motor_id;	//左轮的ID
	int16_t right_motor_id;	//右轮的ID

	int16_t left_motor_rpm;	//左轮的转速	r/min
	int16_t right_motor_rpm;//右轮的转速	r/min

	float left_motor_mmps;	//左轮的转速 mm/s
	float right_motor_mmps;	//右轮的转速 mm/s

	uint32_t pluse_pre_round;	//编码器线数，即旋转一圈的脉冲个数
	float mm_pre_round;			//轮子的周长

	int (*init)(void);
	int (*enable)(void);
	int (*disable)(void);
	int (*set_rpm_speed)(int16_t left_rpm, int16_t right_rpm);
	int (*get_rpm_speed)(int16_t* left_rpm, int16_t right_rpm);
	int (*set_mmps_speed)(float left_mmps, float right_mmps);
	int (*get_mmps_speed)(float* left_mmps, float* right_mmps);
	void (*data_receive_isr_callback)(void);
}bsp_motor_t;

#ifdef __cplusplus
}
#endif

#endif

