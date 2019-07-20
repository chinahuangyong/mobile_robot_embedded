/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_start.c
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-07-06
  * @brief   系统任务启动函数
  * @history
       2019.07.06  创建文件，创建start任务
       2019.06.27  将task_start文件移植转Linux的编程环境下，并优化文件代码
********************************************************************************/

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xil_printf.h"

#include "bsp_stmotor.h"

#include "task_start.h"

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

	bsp_can0_init();
	bsp_stmotor_init();

	taskEXIT_CRITICAL();

	for(;;)
	{

	}
}

