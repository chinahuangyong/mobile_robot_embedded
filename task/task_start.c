/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_start.c
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-05-06
  * @brief   任务启动函数
  * @history
       2019.06.20  新建文件
       2019.06.27
********************************************************************************/
#include <bsp_can0.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xil_printf.h"

#include "bsp_timer.h"
#include "bsp_motor.h"

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
	taskENTER_CRITICAL();

//	bsp_timer_init();
//	bsp_timer_start();

//	bsp_can0_init();

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

//		bsp_motor_set_speed_test(100, 100);
//		vTaskDelay(10000);
//		bsp_motor_set_speed_test(-100, -100);
//		vTaskDelay(10000);
	}
}

