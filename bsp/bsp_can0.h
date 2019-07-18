/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_can0.h
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-11-10
  * @brief
  *          该通信为CAN0的驱动程序，包含CAN0的初始化函数，数据发送函数，中断处理函数
  * @history
  *          2019-7-10 HuangYong 创建文件，完成初始化函数，数据发送函数，中断处理函数；
*********************************************************************************/

#ifndef __BSP_CAN0_H
#define __BSP_CAN0_H

#include "xparameters.h"	/* SDK generated parameters */
#include "xplatform_info.h"
#include "xcanps.h"
#include "xscugic.h"		/* Interrupt controller device driver */
#include "xil_exception.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//////////////////////////////////////////////////////////////////////////////////	 
#ifdef __cplusplus
extern "C" {
#endif

//
typedef struct
{
  uint32_t std_id;       /*!< Specifies the standard identifier.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF */

  uint32_t ext_id;       /*!< Specifies the extended identifier.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF */

  uint32_t ide;          /*!< Specifies the type of identifier for the message that will be received.
                             This parameter can be a value of @ref CAN_Identifier_Type */

  uint32_t rtr;          /*!< Specifies the type of frame for the received message.
                             This parameter can be a value of @ref CAN_remote_transmission_request */

  uint32_t dlc;          /*!< Specifies the length of the frame that will be received.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 8 */

  uint8_t data[8];       /*!< Contains the data to be received.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF */

}can_msg_t;

/* @beief     CAN0初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int8_t bsp_can0_init(void);

/* @beief     CAN0初始化函数
 * @param[in] can_msg - 需要发送的CAN数据
 * @return    0 - 成功 <0 表示返回失败
 * */
int8_t bsp_can0_send(can_msg_t can_msg);

#ifdef __cplusplus
}
#endif


#endif