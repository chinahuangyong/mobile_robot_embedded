/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    tests_stmotor.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-20
  * @brief   电机测试函数
  *
  * @history
  *          2019-7-20 HuangYong 创建文件
*********************************************************************************/
#ifndef __TESTS_MOTOR_H_
#define __TESTS_MOTOR_H_

#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "xil_printf.h"
#include "bsp_stmotor.h"

#ifdef __cplusplus
extern "C"{
#endif

void tests_stmotor_set(void);
void tests_stmotor_get(void);

#ifdef __cplusplus
}
#endif

#endif




