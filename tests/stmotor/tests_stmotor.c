/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    tests_stmotor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-20
  * @brief   电机测试函数
  *
  * @history
  *          2019-7-20 HuangYong 创建文件
*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "xil_printf.h"
#include "bsp_stmotor.h"

void tests_stmotor_set(void)
{
	int ret;

	ret = bsp_stmotor_disable();
	if(ret != 0)
	{
		xil_printf("bsp stmotor disable error\r\n");
	}
	else
	{
		xil_printf("bsp stmotor disable success\r\n");
	}
	vTaskDelay(10000);

	ret = bsp_stmotor_enable();
	if(ret != 0)
	{
		xil_printf("bsp stmotor enable error\r\n");
	}
	else
	{
		xil_printf("bsp stmotor enable success\r\n");
	}
	vTaskDelay(10000);

	ret = bsp_stmotor_set_rpm_speed(50, 50);
	if(ret != 0)
	{
		xil_printf("bsp stmotor set speed error 1\r\n");
	}
	else
	{
		xil_printf("bsp stmotor set speed success 1\r\n");
	}
	vTaskDelay(10000);

	ret = bsp_stmotor_set_rpm_speed(-50, -50);
	if(ret != 0)
	{
		xil_printf("bsp stmotor set speed error 2\r\n");
	}
	else
	{
		xil_printf("bsp stmotor set speed success 2\r\n");

	}
	vTaskDelay(10000);

}

void tests_stmotor_get(void)
{
	int ret;

	int16_t left_motor_rpm, right_motor_rpm;
	int32_t left_motor_pulse, right_motor_pulse;

	ret = bsp_stmotor_get_rpm_speed(&left_motor_rpm, &right_motor_rpm);
	if(ret == 0)
	{
		xil_printf("speed:left_motor:%d, right_motor:%d\r\n", left_motor_rpm, right_motor_rpm);
	}
	else
	{
		xil_printf("read rpm failed\r\n");
	}

	ret = bsp_stmotor_get_pluse_position(&left_motor_pulse, &right_motor_pulse);
	if(ret == 0)
	{
		xil_printf("position:left_motor:%d, right_motor:%d\r\n", left_motor_pulse, right_motor_pulse);
	}
	else
	{
		xil_printf("read position failed\r\n");
	}
}


