/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_can0.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该通信为AXI接口的Timer的驱动程序，包含Timer的初始化程序，定时器启动函数，定时器计数器获取程序
  * @history
  *          2019-7-10 HuangYong 创建文件，完成Timer的初始化程序，定时器启动函数，定时器计数器获取程序
*********************************************************************************/

#ifndef _BSP_AXI0TIMER_H_
#define _BSP_AXI0TIMER_H_

#include "xparameters.h"
#include "xtmrctr.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/* @beief     axi 定时器初始化
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_axi0timer_init(void);

/* @beief     axi 定时器启动
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_axi0timer_start(void);

/* @beief     axi 定时器计数值获取函数
 * */
uint64_t bsp_axi0timer_get(void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BSP_TIMER_H_ */
