/*
 * task_lpsensor.c
 *
 *  Created on: 2019Äê6ÔÂ28ÈÕ
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


void task_lpsensor_init(void)
{
	bsp_lpms_init();

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
//	uint8_t length = 0;

	for(;;)
	{
		if (xQueueReceive(uartRecQueue, &data, portMAX_DELAY) == pdTRUE)
		{
			bsp_lpms_getFrame(data);
			if(bsp_lpms_isGetFrame())
			{
				if(bsp_lpms_parsePacket())
				{
					mod_imu_update(lpmsRawData.gyroX, lpmsRawData.gyroY, lpmsRawData.gyroZ,
							lpmsRawData.accX*-9.8, lpmsRawData.accY*-9.8, lpmsRawData.accZ*-9.8);

					static float data1 = 0;
					static float data2 = 0;
				//
					data1 += lpmsRawData.gyroX*0.01*57.3;
					data2 += lpmsRawData.gyroY*0.01*57.3;

//					#include "utils_osc.h"
//					utils_osc_send(pitch, roll, data1, data2);
				}
			}
		}
	}
}


