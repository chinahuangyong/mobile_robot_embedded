/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_motor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *			 该文件为电机驱动程序，电机驱动器为和利时(SYNTRON)驱动器
  * @history
  *          2019-7-18 HuangYong 创建文件，应后续可能更换驱动器，为专门针对该驱动器设计协议栈
*********************************************************************************/

//#include "bsp_motor.h"

#include "bsp_stmotor.h"
#include "bsp_can0.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include <stdio.h>
#include <math.h>

#define BSP_STMOTOR_EV_LF_ENABLE 		(BaseType_t)(1<<0)
#define BSP_STMOTOR_EV_RT_ENABLE 		(BaseType_t)(1<<1)
#define BSP_STMOTOR_EV_LF_DISABLE 		(BaseType_t)(1<<2)
#define BSP_STMOTOR_EV_RT_DISABLE 		(BaseType_t)(1<<3)
#define BSP_STMOTOR_EV_LF_SETSPEED 		(BaseType_t)(1<<4)
#define BSP_STMOTOR_EV_RT_SETSPEED 		(BaseType_t)(1<<5)
#define BSP_STMOTOR_EV_LF_GETSPEED 		(BaseType_t)(1<<6)
#define BSP_STMOTOR_EV_RT_GETSPEED 		(BaseType_t)(1<<7)
#define BSP_STMOTOR_EV_LF_GETPOSITION 	(BaseType_t)(1<<8)
#define BSP_STMOTOR_EV_RT_GETPOSITION 	(BaseType_t)(1<<9)
#define BSP_STMOTOR_EV_ALL (BSP_STMOTOR_EV_LF_ENABLE | BSP_STMOTOR_EV_RT_ENABLE | \
							BSP_STMOTOR_EV_LF_DISABLE | BSP_STMOTOR_EV_RT_DISABLE | \
							BSP_STMOTOR_EV_LF_SETSPEED | BSP_STMOTOR_EV_RT_SETSPEED | \
							BSP_STMOTOR_EV_LF_GETSPEED | BSP_STMOTOR_EV_RT_GETSPEED | \
							BSP_STMOTOR_EV_LF_GETPOSITION | BSP_STMOTOR_EV_RT_GETPOSITION)


/* 驱动器ID初始化，具体ID值通过厂家提供的上位机软件来设置 */
const uint8_t left_motor_id = 1;
const uint8_t right_motor_id = 2;
const uint8_t master_id = 0;

//轮子的周长
const float radius = 0.13;	//车辆的半径(m)
const float timeout = 10;	//通信超时时间设置(ms)
const float PULSE_NUM_PRE_ROUND = 5600;

//电机运动的方向
uint8_t run_direction = 0;	//车体运行的方向

//轮子转速，单位为rpm(转/分)
static uint32_t left_motor_rpm_speed = 0;
static uint32_t right_motor_rpm_speed = 0;

//电机编码器值
static int32_t left_motor_pulse_position = 0;
static int32_t right_motor_pulse_position = 0;

static EventGroupHandle_t stmotor_recv_ev;

/* @beief     电机初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_init(void)
{
	left_motor_rpm_speed = 0;
	right_motor_rpm_speed = 0;

	left_motor_pulse_position = 0;
	right_motor_pulse_position = 0;

	bsp_can0_init();

	stmotor_recv_ev =  xEventGroupCreate();
	if(stmotor_recv_ev == NULL)
	{
		return -1;
	}

	return 0;
}

/* @beief     电机使能函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_enable(void)
{
	can_msg_t can_msg;

	xEventGroupClearBits(stmotor_recv_ev, BSP_STMOTOR_EV_ALL);
	EventBits_t rev_eb;

	//采用应答模式，将电机使能
	can_msg.std_id = 0x0100 | (left_motor_id);
	can_msg.rtr = 0;
	can_msg.ide = 0;
	can_msg.ext_id = 0;
	can_msg.dlc = 6;
	can_msg.data[0] = master_id;
	can_msg.data[1] = 0x24;
	can_msg.data[2] = 0x00;
	can_msg.data[3] = 0x10;
	can_msg.data[4] = 0x00;
	can_msg.data[5] = 0x01;
	bsp_can0_send(can_msg);

	can_msg.std_id = 0x0100 | (right_motor_id);
	bsp_can0_send(can_msg);

	rev_eb = xEventGroupWaitBits(stmotor_recv_ev,
			BSP_STMOTOR_EV_LF_ENABLE | BSP_STMOTOR_EV_RT_ENABLE,
			pdTRUE,
			pdTRUE,
			pdMS_TO_TICKS(timeout));
	if(rev_eb == (EventBits_t)(BSP_STMOTOR_EV_LF_ENABLE | BSP_STMOTOR_EV_RT_ENABLE))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/* @beief     电机去使能函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_disable(void)
{
	can_msg_t can_msg;
	EventBits_t rev_eb;

	xEventGroupClearBits(stmotor_recv_ev, BSP_STMOTOR_EV_ALL);
	//采用应答模式，将电机去使能
	can_msg.std_id = 0x0100 | (left_motor_id);
	can_msg.rtr = 0;
	can_msg.ide = 0;
	can_msg.ext_id = 0;
	can_msg.dlc = 6;
	can_msg.data[0] = master_id;
	can_msg.data[1] = 0x24;
	can_msg.data[2] = 0x00;
	can_msg.data[3] = 0x10;
	can_msg.data[4] = 0x00;
	can_msg.data[5] = 0x00;
	bsp_can0_send(can_msg);

	can_msg.std_id = 0x0100 | (right_motor_id);
	bsp_can0_send(can_msg);

	rev_eb = xEventGroupWaitBits(stmotor_recv_ev,
				BSP_STMOTOR_EV_LF_DISABLE | BSP_STMOTOR_EV_RT_DISABLE,
				pdTRUE,
				pdTRUE,
				pdMS_TO_TICKS(timeout));
	if(rev_eb == (EventBits_t)(BSP_STMOTOR_EV_LF_DISABLE | BSP_STMOTOR_EV_RT_DISABLE))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/* @beief     电机转速设置函数
 * @param[in] left_rpm - 电机左轮转速rpm
 * @param[in] right_rpm - 电机右轮转速rpm
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_set_rpm_speed(int16_t left_rpm, int16_t right_rpm)
{
	can_msg_t can_msg;
	EventBits_t rev_eb;

	xEventGroupClearBits(stmotor_recv_ev, BSP_STMOTOR_EV_ALL);
	//采用应答模式发送电机转速
	can_msg.std_id = 0x0100 | (left_motor_id);
	can_msg.rtr = 0;
	can_msg.ide = 0;
	can_msg.ext_id = 0;
	can_msg.dlc = 6;
	can_msg.data[0] = master_id;
	can_msg.data[1] = 0x28;
	can_msg.data[2] = 0x00;
	can_msg.data[3] = 0x00;
	can_msg.data[4] = left_rpm>>8;
	can_msg.data[5] = left_rpm;
	bsp_can0_send(can_msg);

	can_msg.std_id = 0x0100 | (right_motor_id);
	can_msg.data[4] = right_rpm>>8;
	can_msg.data[5] = right_rpm;
	bsp_can0_send(can_msg);

	rev_eb = xEventGroupWaitBits(stmotor_recv_ev,
			BSP_STMOTOR_EV_LF_SETSPEED | BSP_STMOTOR_EV_RT_SETSPEED,
			pdTRUE,
			pdTRUE,
			pdMS_TO_TICKS(timeout));
	if(rev_eb == (EventBits_t)(BSP_STMOTOR_EV_LF_SETSPEED | BSP_STMOTOR_EV_RT_SETSPEED))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/* @beief     电机转速获取函数
 * @param[out] left_motor_rpm - 电机左轮转速rpm
 * @param[out] right_motor_rpm - 电机右轮转速rpm
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_rpm_speed(int16_t* left_motor_rpm, int16_t* right_motor_rpm)
{
	can_msg_t can_msg;
	EventBits_t rev_eb;

	xEventGroupClearBits(stmotor_recv_ev, BSP_STMOTOR_EV_ALL);
	//采用应答模式获取电机转速
	can_msg.std_id = 0x0100 | (left_motor_id);
	can_msg.rtr = 0;
	can_msg.ide = 0;
	can_msg.ext_id = 0;
	can_msg.dlc = 4;
	can_msg.data[0] = master_id;
	can_msg.data[1] = 0x0A;
	can_msg.data[2] = 0x00;
	can_msg.data[3] = 0x00;
	bsp_can0_send(can_msg);

	can_msg.std_id = 0x0100 | (right_motor_id);
	bsp_can0_send(can_msg);
	rev_eb = xEventGroupWaitBits(stmotor_recv_ev,
				BSP_STMOTOR_EV_LF_GETSPEED | BSP_STMOTOR_EV_RT_GETSPEED,
				pdTRUE,
				pdTRUE,
				pdMS_TO_TICKS(timeout));
	if(rev_eb == (EventBits_t)(BSP_STMOTOR_EV_LF_GETSPEED | BSP_STMOTOR_EV_RT_GETSPEED))
	{
		*left_motor_rpm = left_motor_rpm_speed;
		*right_motor_rpm = right_motor_rpm_speed;
		return 0;
	}
	else
	{
		*left_motor_rpm = 0;
		*right_motor_rpm = 0;
		return -1;
	}

}

/* @beief     电机转速设置函数
 * @param[in] left_motor_mps - 电机左轮转速mps(m/s)
 * @param[in] right_motor_mps - 电机右轮转速mps(m/s)
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_set_mps_speed(float left_motor_mps, float right_motor_mps)
{
	int ret;
	//将m/s的转速转换成r/min
	int16_t left_motor_rpm = (int16_t)(lround(left_motor_mps*60.0/(2.0*M_PI*radius)));
	int16_t right_motor_rpm = (int16_t)(lround(right_motor_mps*60.0/(2.0*M_PI*radius)));

	ret = bsp_stmotor_set_rpm_speed(left_motor_rpm, right_motor_rpm);
	if(ret == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/* @beief     电机转速获取函数
 * @param[out] left_motor_mps - 电机左轮转速mmps(m/s)
 * @param[out] right_motor_mps - 电机右轮转速mmps(m/s)
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_mps_speed(float* left_motor_mps, float* right_motor_mps)
{
	int ret;
	int16_t left_motor_rpm, right_motor_rpm;
	ret = bsp_stmotor_get_rpm_speed(&left_motor_rpm, &right_motor_rpm);

	if(ret == 0)
	{
		*left_motor_mps = (float)left_motor_rpm*2.0*M_PI*radius/60.0;
		*right_motor_mps = (float)right_motor_rpm*2.0*M_PI*radius/60.0;

		return 0;
	}
	else
	{
		return ret;
	}
}

/* @beief     电机编码器脉冲个数获取
 * @param[in] left_motor_position - 电机左轮脉冲个数
 * @param[in] right_motor_position - 电机右轮脉冲个数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_pluse_position(int32_t* left_motor_position, int32_t* right_motor_position)
{
	can_msg_t can_msg;
	EventBits_t rev_eb;

	xEventGroupClearBits(stmotor_recv_ev, BSP_STMOTOR_EV_ALL);
	//采用应答模式获取电机转速
	can_msg.std_id = 0x0100 | (left_motor_id);
	can_msg.rtr = 0;
	can_msg.ide = 0;
	can_msg.ext_id = 0;
	can_msg.dlc = 4;
	can_msg.data[0] = master_id;
	can_msg.data[1] = 0x0D;
	can_msg.data[2] = 0x00;
	can_msg.data[3] = 0x1D;
	bsp_can0_send(can_msg);

	can_msg.std_id = 0x0100 | (right_motor_id);
	bsp_can0_send(can_msg);
	rev_eb = xEventGroupWaitBits(stmotor_recv_ev,
			BSP_STMOTOR_EV_LF_GETPOSITION | BSP_STMOTOR_EV_RT_GETPOSITION,
			pdTRUE,
			pdTRUE,
			pdMS_TO_TICKS(timeout));
	if(rev_eb == (EventBits_t)(BSP_STMOTOR_EV_LF_GETPOSITION | BSP_STMOTOR_EV_RT_GETPOSITION))
	{
		*left_motor_position = left_motor_pulse_position;
		*right_motor_position = right_motor_pulse_position;
		return 0;
	}
	else
	{
		*left_motor_position = 0;
		*right_motor_position = 0;
		return -1;
	}
}

/* @beief     电机编码器毫米数获取
 * @param[in] left_motor_position - 电机左轮脉冲米数
 * @param[in] right_motor_position - 电机右轮脉冲米数
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_stmotor_get_m_position(float* left_motor_position, float* right_motor_position)
{
	int ret;
	int32_t left_position, right_position;
	ret = bsp_stmotor_get_pluse_position(&left_position, &right_position);
	if(ret == 0)
	{
		*left_motor_position = left_position*1.0/PULSE_NUM_PRE_ROUND*2.0*M_PI*radius;
		*right_motor_position = right_position*1.0/PULSE_NUM_PRE_ROUND*2.0*M_PI*radius;
		return 0;
	}
	else
	{
		return ret;
	}
}

/* @beief     中断回调函数
 * @return    无
 * */
void can0_isr_callback(can_msg_t* data)
{
	can_msg_t can_msg;
	memcpy(&can_msg, (can_msg_t *)data, sizeof(can_msg_t));
	BaseType_t xHigherPriorityTaskWoken;

	//判断接收的数据帧为标准帧，并且为数据帧时
	if(can_msg.ide == 0 && can_msg.rtr == 0)
	{
		if(can_msg.data[0] == left_motor_id)
		{
			if(can_msg.data[1] == 0x25)	//使能返回的正常功能码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x10)	//Fn010
				{
					if(can_msg.data[4] == 0x00 && can_msg.data[5] == 0x01)
					{
						//电机驱动器使能应答检测成功
						xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_LF_ENABLE,
								&xHigherPriorityTaskWoken);
					}
					else if(can_msg.data[4] == 0x00 && can_msg.data[5] == 0x00)
					{
						xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_LF_DISABLE,
								&xHigherPriorityTaskWoken);
					}
				}
			}
			else if(can_msg.data[1] == 0x29)//写转速返回的应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x00)	//Cn00
				{
					//设置电机转速应答成功
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_LF_SETSPEED,
								&xHigherPriorityTaskWoken);
				}
			}
			else if(can_msg.data[1] == 0x0B)//读转速返回成功应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x00)
				{
					left_motor_rpm_speed = ((uint16_t)(can_msg.data[4])<<8) | (can_msg.data[5]);
					//设置电机转速应答成功
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_LF_GETSPEED,
								&xHigherPriorityTaskWoken);
				}
			}
			else if(can_msg.data[1] == 0xE)	//读编码值返回的应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x1D)	//Dn1D Dn1E
				{
					left_motor_pulse_position =
							((uint32_t)can_msg.data[6]) << 24 |((uint32_t)(can_msg.data[7]))<<16 |
							((uint32_t)can_msg.data[4]) << 8 | ((uint32_t)can_msg.data[5]);
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_LF_GETPOSITION,
								&xHigherPriorityTaskWoken);
				}
			}
			// else if(can_msg)
		}
		else if(can_msg.data[0] == right_motor_id)
		{
			if(can_msg.data[1] == 0x25)	//使能返回的正常功能码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x10)	//Fn010
				{
					if(can_msg.data[4] == 0x00 && can_msg.data[5] == 0x01)
					{
						//电机驱动器使能应答检测成功
						xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_RT_ENABLE,
								&xHigherPriorityTaskWoken);
					}
					else if(can_msg.data[4] == 0x00 && can_msg.data[5] == 0x00)
					{
						xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_RT_DISABLE,
								&xHigherPriorityTaskWoken);
					}
				}
			}
			else if(can_msg.data[1] == 0x29)//写转速返回的应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x00)	//Cn00
				{
					//设置电机转速应答成功
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_RT_SETSPEED,
								&xHigherPriorityTaskWoken);
				}
			}
			else if(can_msg.data[1] == 0x0B)//读转速返回成功应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x00)
				{
					right_motor_rpm_speed = ((uint16_t)(can_msg.data[4])<<8) | (can_msg.data[5]);
					//设置电机转速应答成功
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_RT_GETSPEED,
								&xHigherPriorityTaskWoken);
				}
			}
			else if(can_msg.data[1] == 0xE)	//读编码值返回的应答码
			{
				if(can_msg.data[2] == 0x00 && can_msg.data[3] == 0x1D)	//Dn1D Dn1E
				{
					right_motor_pulse_position =
							((uint32_t)can_msg.data[6]) << 24 |((uint32_t)(can_msg.data[7]))<<16 |
							((uint32_t)can_msg.data[4]) << 8 | ((uint32_t)can_msg.data[5]);
					xEventGroupSetBitsFromISR(stmotor_recv_ev, BSP_STMOTOR_EV_RT_GETPOSITION,
								&xHigherPriorityTaskWoken);
				}
			}
		}
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}



