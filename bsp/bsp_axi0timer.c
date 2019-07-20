/*
 * bsp_timer.c
 *
 *  Created on: 2019��6��11��
 *      Author: HuangYong
 */

#include "bsp_axi0timer.h"

#include "xtmrctr.h"
#include "xscugic.h"
#include "platform_config.h"

XTmrCtr TimerInstancePtr1;	//

void bsp_axi0timer_init(void)
{
	int xStatus;
	xStatus = XTmrCtr_Initialize(&TimerInstancePtr1,XPAR_AXI_TIMER_0_DEVICE_ID);
	if(XST_SUCCESS != xStatus)
		xil_printf("TIMER INIT FAILED \n\r");

	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);

	XTmrCtr_SetOptions(&TimerInstancePtr1, XPAR_AXI_TIMER_0_DEVICE_ID,
			(XTC_CASCADE_MODE_OPTION | XTC_ENABLE_ALL_OPTION ));
}

void bsp_axi0timer_start(void)
{
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 0, 0);
	XTmrCtr_SetResetValue(&TimerInstancePtr1, 1, 0);

	XTmrCtr_Reset(&TimerInstancePtr1, 0);
	XTmrCtr_Reset(&TimerInstancePtr1, 1);

	XTmrCtr_Start(&TimerInstancePtr1,0);
}

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


