/* * rtmotor.c
 *
 *  Created on: 2019?7?19?
 *      Author: root
 */

#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <stdlib.h>

#include "xil_printf.h"

#include "bsp_stmotor.h"

void tests_stmotor_test(void)
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


