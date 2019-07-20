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

#include "bsp_axi0timer.h"

#include "xtmrctr.h"
#include "xscugic.h"
#include "platform_config.h"

XTmrCtr TimerInstancePtr1;	

/* @beief     axi 定时器初始化
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_axi0timer_init(void)
{
	int xStatus;
	xStatus = XTmrCtr_Initialize(&TimerInstancePtr1,XPAR_AXI_TIMER_0_DEVICE_ID);
	if(XST_SUCCESS != xStatus)
	{
		return -1;
	}

	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);

	XTmrCtr_SetOptions(&TimerInstancePtr1, XPAR_AXI_TIMER_0_DEVICE_ID,
					(XTC_CASCADE_MODE_OPTION | XTC_ENABLE_ALL_OPTION ));

	return 0;
}

/* @beief     axi 定时器启动
 * @return    0 - 成功 <0 表示返回失败
 * */
int bsp_axi0timer_start(void)
{
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);

	XTmrCtr_Reset(&TimerInstancePtr1, 0);
	XTmrCtr_Reset(&TimerInstancePtr1, 1);

	XTmrCtr_Start(&TimerInstancePtr1,0);

	return 0;
}

/* @beief     axi 定时器计数值获取函数
 * */
uint64_t bsp_axi0timer_get(void)
{
	uint32_t Timer_H = 0;
	uint32_t Timer_L = 0;
	uint64_t Timer = 0;

	Timer_H = XTmrCtr_GetValue(&TimerInstancePtr1, 0);
	Timer_L = XTmrCtr_GetValue(&TimerInstancePtr1, 1);
	Timer  = ((uint64_t)Timer_H<<32) | Timer_L;

	return Timer;
}


