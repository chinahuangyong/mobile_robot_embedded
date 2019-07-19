/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_uart0.h
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该通信为UART0的驱动程序，包含UART0的初始化函数，数据发送函数，中断处理函数
  * @history
  *          2019-7-10 HuangYong 创建文件，完成初始化函数，数据发送函数，中断处理函数；
*********************************************************************************/
#ifndef SRC_BSP_UART_H_
#define SRC_BSP_UART_H_

#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xstatus.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef _cplusplus
extern "C"{
#endif

extern QueueHandle_t uartRecQueue;

/* @beief     UART0初始化函数
 * @return    0-成功 <0-失败
 * */
int bsp_uart0_init(void);

/* @beief     UART0数据发送函数
 * @param[in] 需要发送的字符串数组指针
 * @param[in] 数据大小
 * @return    0-成功 <0-失败
 * */
int bsp_uart0_send(uint8_t *p_data, uint16_t size);

#ifdef _cplusplus
}
#endif


#endif /* SRC_BSP_UART_H_ */
