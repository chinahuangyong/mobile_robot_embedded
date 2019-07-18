/*
 * bsp_timer.c
 *
 *  Created on: 2019年6月11日
 *      Author: HuangYong
 */

#include "bsp_timer.h"

#include "xtmrctr.h"
#include "xscugic.h"
#include "platform_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

XTmrCtr TimerInstancePtr1;	//系统时间戳计数器
XTmrCtr TimerInstancePtr2;	//系统同步定时器，定时输出1s

void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);


void bsp_timer_init(void)
{
	int xStatus;
	xStatus = XTmrCtr_Initialize(&TimerInstancePtr1,XPAR_AXI_TIMER_0_DEVICE_ID);
	if(XST_SUCCESS != xStatus)
		xil_printf("TIMER INIT FAILED \n\r");
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);//Change with generic value
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);//Change with generic value
	//Setting timer Option (Interrupt Mode And Auto Reload )
	XTmrCtr_SetOptions(&TimerInstancePtr1, XPAR_AXI_TIMER_0_DEVICE_ID,
			(XTC_CASCADE_MODE_OPTION | XTC_ENABLE_ALL_OPTION ));

//	xStatus = XTmrCtr_Initialize(&TimerInstancePtr2,XPAR_AXI_TIMER_1_DEVICE_ID);
//	if(XST_SUCCESS != xStatus)
//		xil_printf("TIMER INIT FAILED \n\r");
//	XTmrCtr_SetResetValue(&TimerInstancePtr2, 0, 0);//Change with generic value
//	XTmrCtr_SetOptions(&TimerInstancePtr2, XPAR_AXI_TIMER_1_DEVICE_ID,
//			XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);//Setting timer Option (Interrupt Mode And Auto Reload )

//	/*
//	 * Connect the interrupt handler that will be called when an
//	 * interrupt occurs for the device.
//	 */
//	xStatus = XScuGic_Connect(pxInterruptController, XPS_FPGA0_INT_ID,
//				 (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
//				 &TimerInstancePtr2);
//	if (xStatus != XST_SUCCESS)
//	{
//	}
//
//	XTmrCtr_SetHandler(&TimerInstancePtr2, TimerCounterHandler, &TimerInstancePtr2);
//
//	/*
//	 * Enable the interrupt for the Timer device.
//	 */
//	XScuGic_Enable(pxInterruptController, XPS_FPGA0_INT_ID);

//#define TMRCTR_INTERRUPT_ID	XPAR_INTC_0_TMRCTR_0_VEC_ID

}

void bsp_timer_start(void)
{
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);//Change with generic value
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);//Change with generic value

	XTmrCtr_Reset(&TimerInstancePtr1, 0);
	XTmrCtr_Reset(&TimerInstancePtr1, 1);

//	XTmrCtr_Start(&TimerInstancePtr1,1);
	XTmrCtr_Start(&TimerInstancePtr1,0);

//	XTmrCtr_SetResetValue(&TimerInstancePtr2, 0, 1000000-1);
//	XTmrCtr_Start(&TimerInstancePtr2,0);
}

uint64_t bsp_timer_get(void)
{
	uint32_t Timer_H = 0;
	uint32_t Timer_L = 0;
	uint64_t Timer = 0;

	Timer_H = XTmrCtr_GetValue(&TimerInstancePtr1, 0);
	Timer_L = XTmrCtr_GetValue(&TimerInstancePtr1, 1);
	Timer  = ((uint64_t)Timer_H<<32) | Timer_L;

	return Timer;
}

///*****************************************************************************/
///**
//* This function is the handler which performs processing for the timer counter.
//* It is called from an interrupt context such that the amount of processing
//* performed should be minimized.  It is called when the timer counter expires
//* if interrupts are enabled.
//*
//* This handler provides an example of how to handle timer counter interrupts
//* but is application specific.
//*
//* @param	CallBackRef is a pointer to the callback function
//* @param	TmrCtrNumber is the number of the timer to which this
//*		handler is associated with.
//*
//* @return	None.
//*
//* @note		None.
//*
//******************************************************************************/
//void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
//{
////	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
//
//	/*
//	 * Check if the timer counter has expired, checking is not necessary
//	 * since that's the reason this function is executed, this just shows
//	 * how the callback reference can be used as a pointer to the instance
//	 * of the timer counter that expired, increment a shared variable so
//	 * the main thread of execution can see the timer expired
//	 */
//
////	if()
//}


