/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_start.c
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-05-06
  * @brief   ������������
  * @history
       2019.06.20  ��������
       2019.06.27
********************************************************************************/
#include <bsp_can0.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xil_printf.h"

#include "bsp_timer.h"
#include "bsp_stmotor.h"

#include "task_start.h"
#include "task_camera_sync.h"
#include "task_net.h"

#define TASK_START_PRIO 2
#define TASK_START_STK_SIZE 1024
TaskHandle_t handle_start;

void task_start(void* argument);

//start task init
void task_start_init(void)
{
	xTaskCreate((TaskFunction_t)task_start,
	              (const char *)"task_start",
	              (uint16_t)TASK_START_STK_SIZE,
	              (void *)NULL,
	              (UBaseType_t)TASK_START_PRIO,
	              (TaskHandle_t *)&handle_start);
}

void task_start(void *argument)
{
	int ret;

	taskENTER_CRITICAL();

//	bsp_timer_init();
//	bsp_timer_start();

	bsp_can0_init();
	bsp_stmotor_init();
//	task_lpsensor_init();


//	task_net_init();
//	task_camera_sync_init();

	taskEXIT_CRITICAL();

	for(;;)
	{
//		if(xQueueReceive(xTimeStamp, &time_u, portMAX_DELAY))
//		{
//			xil_printf("%u,%u,%u\r\n", time_u.time_h,time_u.time_l,time_u.len);
//		}
//		else
//		{
//		}
//		bsp_can0_send(can_msg);
//		vTaskDelay(100);

	}
}

