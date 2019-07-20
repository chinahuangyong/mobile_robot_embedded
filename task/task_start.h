/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_start.h
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-07-06
  * @brief   系统任务启动函数
  * @history
       2019.07.06  创建文件，创建start任务
       2019.06.27  将task_start文件移植转Linux的编程环境下，并优化文件代码
********************************************************************************/

#ifndef __TASK_START_H_
#define __TASK_START_H_

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
