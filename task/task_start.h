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
********************************************************************************/

#ifndef SRC_TASK_START_H_
#define SRC_TASK_START_H_

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void task_start_init(void);

#ifdef _cplusplus
}
#endif

#endif /* SRC_TASK_START_H_ */
