/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_net.c
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-05-06
  * @brief   网络管理任务启动函数
  * @history
       2019.06.20  新建文件
********************************************************************************/

#ifndef SRC_TASK_NET_C_
#define SRC_TASK_NET_C_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

extern QueueHandle_t xTimeStamp;

typedef struct
{
	uint32_t time_h;
	uint32_t time_l;
	uint16_t len;
}time_u_t;

extern time_u_t time_u;

void task_net_init(void);

#ifdef __cpluplus
}
#endif

#endif /* SRC_TASK_NET_C_ */





