/*
 * task_camera_sync.c
 *
 *  Created on: 2019Äê6ÔÂ20ÈÕ
 *      Author: HuangYong
 */

#include "FreeRTOS.h"
#include "task.h"

#include "task_camera_sync.h"

#include "bsp_camera_time.h"

#define TASK_CAMERA_SYNC_PRIO (configMAX_PRIORITIES - 4)
#define TASK_CAMERA_SYNC_STK_SIZE 1024
TaskHandle_t handle_camera_sync;

void task_camera_sync(void* argument);

void task_camera_sync_init(void)
{
	bsp_camera_time_init();

	xTaskCreate((TaskFunction_t)task_camera_sync,
				  (const char *)"task_camera_sync",
				  (uint16_t)TASK_CAMERA_SYNC_STK_SIZE,
				  (void *)NULL,
				  (UBaseType_t)TASK_CAMERA_SYNC_PRIO,
				  (TaskHandle_t *)&handle_camera_sync);
}

void task_camera_sync(void* argument)
{
	for(;;)
	{
		vTaskDelay(500);
//		bsp_timer_sync();
	}
}

