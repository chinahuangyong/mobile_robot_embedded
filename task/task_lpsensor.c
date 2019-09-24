/*
 * task_lpsensor.c
 *
 *  Created on: 2019��6��28��
 *      Author: HuangYong
 */

#include "task_lpsensor.h"

#include "bsp_lpsensor.h"
#include "bsp_timer.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xil_printf.h"

#include "mod_imu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TASK_LPSENSOR_PRIO (configMAX_PRIORITIES - 2)
#define TASK_LPSENSOR_STK_SIZE 1024
TaskHandle_t handle_lpsensor;
void task_lpsensor(void *argument);

lpmsttl_data_ts_t lpmsttl_data_ts;

void task_lpsensor_init(void)
{
	bsp_lpms_init();
	bsp_imu_init();

	xTaskCreate((TaskFunction_t)task_lpsensor,
		              (const char *)"task_lpsensor",
		              (uint16_t)TASK_LPSENSOR_STK_SIZE,
		              (void *)NULL,
		              (UBaseType_t)TASK_LPSENSOR_PRIO,
		              (TaskHandle_t *)&handle_lpsensor);
}

uint64_t time_now;
void task_lpsensor(void* argument)
{
	uint8_t data = 0;

	for(;;)
	{
		if(bsp_lpmsttl_get_frame() == 0)
		{
			if(bsp_lpmsttl_get_packet() == 0)
			{
				if(bsp_lpmsttl_parse_packet() == 0)
				{
					bsp_lpmsttl_get_data(&lpmsttl_data_ts);
					mod_imu_update(lpmsttl_data_ts.lpmsttl_data.gyro_x,
							       lpmsttl_data_ts.lpmsttl_data.gyro_y,
								   lpmsttl_data_ts.lpmsttl_data.gyro_z,
								   lpmsttl_data_ts.lpmsttl_data.acc_x,
								   lpmsttl_data_ts.lpmsttl_data.acc_y,
								   lpmsttl_data_ts.lpmsttl_data.acc_z);
				}
			}
		}
	}
}


