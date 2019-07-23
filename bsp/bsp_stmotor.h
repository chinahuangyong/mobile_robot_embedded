/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_motor.h
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *			 该文件为电机驱动程序，电机驱动器为和利时(SYNTRON)驱动器
  * @history
  *          2019-7-18 HuangYong 创建文件
*********************************************************************************/
#ifndef __BSP_STMOTOR_H_
#define __BSP_STMOTOR_H_

#include "bsp_motor.h"
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//运动方向控制变量 0-表示向前（默认） 1-表示向后
extern uint8_t run_direction;

/* @beief     电机初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_init(void);

/* @beief     电机使能函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_enable(void);

/* @beief     电机去使能函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_disable(void);

/* @beief     电机转速设置函数
 * @param[in] left_rpm - 电机左轮转速rpm
 * @param[in] right_rpm - 电机右轮转速rpm
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_set_rpm_speed(int16_t left_rpm, int16_t right_rpm);

/* @beief     电机转速获取函数
 * @param[out] left_motor_rpm - 电机左轮转速rpm
 * @param[out] right_motor_rpm - 电机右轮转速rpm
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_rpm_speed(int16_t* left_motor_rpm, int16_t* right_motor_rpm);

/* @beief     电机转速设置函数
 * @param[in] left_motor_mps - 电机左轮转速mmps(m/s)
 * @param[in] right_motor_mps - 电机右轮转速mmps(mm/s)
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_set_mps_speed(float left_motor_mps, float right_motor_mps);

/* @beief     电机转速获取函数
 * @param[out] left_motor_mps - 电机左轮转速mmps(m/s)
 * @param[out] right_motor_mps - 电机右轮转速mmps(m/s)
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_mps_speed(float* left_motor_mps, float* right_motor_mps);

/* @beief     电机编码器脉冲个数获取
 * @param[in] left_motor_position - 电机左轮脉冲个数
 * @param[in] right_motor_position - 电机右轮脉冲个数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_pluse_position(int32_t* left_motor_position, int32_t* right_motor_position);

/* @beief     电机编码器毫米数获取
 * @param[in] left_motor_position - 电机左轮脉冲米数
 * @param[in] right_motor_position - 电机右轮脉冲米数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_m_position(float* left_motor_position, float* right_motor_position);

/* @beief     中断回调函数
 * @return    无
 * */
void bsp_stmotor_can0_callback(void* data);

#ifdef __cplusplus
}
#endif

#endif /* SRC_BSP_BSP_STMOTOR_H_ */
