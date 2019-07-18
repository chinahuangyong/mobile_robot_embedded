/*
 * bsp_camera_time.c
 *
 *  Created on: 2019年6月13日
 *      Author: HuangYong
 */

#include "bsp_camera_time.h"

#include "bsp_timer.h"

#include "xparameters.h"
#include "xiicps.h"
#include "xgpiops.h"

#include <stdint.h>


#define BSP_TIMER_PLUSE_IO_NUM 9

XIicPs Iic;				/* Instance of the IIC Device */
XGpioPs psGpioInstancePtr;


void bsp_camera_time_init(void)
{
	int xStatus;
	XIicPs_Config *Config;
	XGpioPs_Config* GpioConfigPtr;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(IIC_DEVICE_ID);
	if (NULL == Config)
	{
	}

	xStatus = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (xStatus != XST_SUCCESS)
	{
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	xStatus = XIicPs_SelfTest(&Iic);
	if (xStatus != XST_SUCCESS)
	{
	}

	//IO Init
	GpioConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	if(GpioConfigPtr == NULL)
	{

	}

	xStatus = XGpioPs_CfgInitialize(&psGpioInstancePtr,GpioConfigPtr, GpioConfigPtr->BaseAddr);
	if(XST_SUCCESS != xStatus)
	{

	}
	XGpioPs_WritePin(&psGpioInstancePtr, BSP_TIMER_PLUSE_IO_NUM, 0);//输出0
	XGpioPs_SetOutputEnablePin(&psGpioInstancePtr, BSP_TIMER_PLUSE_IO_NUM, 1); 	//1表示使能
	XGpioPs_SetDirectionPin(&psGpioInstancePtr, BSP_TIMER_PLUSE_IO_NUM, 1);//1表示输出

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
}

//在同步线程中调用
void bsp_timer_sync(void)
{
	uint64_t time_now;
	uint8_t data[16];
	int32_t xStatus;

	uint32_t time_h;
	uint32_t time_l;

	time_now = bsp_timer_get();
	time_now  = time_now >> 32;
	XGpioPs_WritePin(&psGpioInstancePtr, BSP_TIMER_PLUSE_IO_NUM, 0);

	data[0] =(uint8_t)(time_now);
	data[1] =(uint8_t)(time_now>>8);
	data[2] =(uint8_t)(time_now>>16);
	data[3] =(uint8_t)(time_now>>24);
	data[4] =(uint8_t)(time_now>>32);
	data[5] =(uint8_t)(time_now>>40);
	data[6] =(uint8_t)(time_now>>48);
	data[7] =(uint8_t)(time_now>>56);

	data[8] =(uint8_t)(time_now);
	data[9] =(uint8_t)(time_now>>8);
	data[10] =(uint8_t)(time_now>>16);
	data[11] =(uint8_t)(time_now>>24);
	data[12] =(uint8_t)(time_now>>32);
	data[13] =(uint8_t)(time_now>>40);
	data[14] =(uint8_t)(time_now>>48);
	data[15] =(uint8_t)(time_now>>56);

	time_h = time_now>>32;
	time_l = time_now;
	xil_printf("%d,%d\r\n", time_h, time_l);

	xStatus = XIicPs_MasterSendPolled(&Iic, data, 16, IIC_SLAVE_ADDR);
	if (xStatus != XST_SUCCESS)
	{
	}
	while (XIicPs_BusIsBusy(&Iic));
	XGpioPs_WritePin(&psGpioInstancePtr, BSP_TIMER_PLUSE_IO_NUM, 1);
}


