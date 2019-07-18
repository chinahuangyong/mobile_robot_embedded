/*
 * bsp_uart.c
 *
 *  Created on: 2019锟斤拷6锟斤拷14锟斤拷
 *      Author: HuangYong
 */

#include "bsp_uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//宏定义
#define UART_RECE_QUEUE_SIZE 200	//串口接收队列数据长度

//全局变量
XUartPs Uart0InstPtr;		/* Instance of the UART Device */
XUartPs *pxUart0InstPtr = &Uart0InstPtr;		/* Instance of the UART Device */

QueueHandle_t uartRecQueue = NULL;	//串口接收队列句柄

void Uart0_ISR_Handle(void);
void Uart_ISR_Handler(void *CallBackRef, u32 Event, unsigned int EventData);

//串口初始化函数
void bsp_uart_init(void)
{
	int Status;
	u32 IntrMask;

	XUartPs_Config *Config;

	uartRecQueue = xQueueCreate(UART_RECE_QUEUE_SIZE, sizeof(u32));

	/*********************UART INIT**************************/
	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
	Config = XUartPs_LookupConfig(UART_DEVICE_ID);
	if (NULL == Config)
	{
	}

	Status = XUartPs_CfgInitialize(&Uart0InstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
	}

	/* Check hardware build */
	Status = XUartPs_SelfTest(&Uart0InstPtr);
	if (Status != XST_SUCCESS)
	{
	}

	/*********************UART INT INIT**************************/
	/*
	 * Enable the interrupt of the UART so interrupts will occur, setup
	 * a local normal so data that is sent will be received.
	 */

	XUartPs_SetOperMode(&Uart0InstPtr, XUARTPS_OPER_MODE_NORMAL);
//	XUartPs_SetHandler(&Uart0InstPtr, (XUartPs_Handler)Uart_ISR_Handler, NULL);
	XUartPs_SetFifoThreshold(&Uart0InstPtr, 1);

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler
	 * performs the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(pxInterruptController, UART_INT_IRQ_ID,
				  (Xil_ExceptionHandler) Uart0_ISR_Handle,
				  (void *) &Uart0InstPtr);
	if (Status != XST_SUCCESS)
	{
	}

	XUartPs_SetRecvTimeout(&Uart0InstPtr, 8);

	IntrMask = XUARTPS_IXR_RXOVR;
	XUartPs_SetInterruptMask(&Uart0InstPtr, IntrMask);

	/* Enable the interrupt for the device */
	XScuGic_Enable(pxInterruptController, UART_INT_IRQ_ID);
}


//串口中断处理函数
void Uart0_ISR_Handle(void)
{
	u32 IsrStatus;
	u32 CsrRegister;

	u32 recData;

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	Xil_AssertVoid(pxUart0InstPtr != NULL);
	Xil_AssertVoid(pxUart0InstPtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Read the interrupt ID register to determine which
	 * interrupt is active
	 */
	IsrStatus = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);

	IsrStatus &= XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
				   XUARTPS_ISR_OFFSET);

	/* Dispatch an appropriate handler. */
	if((IsrStatus & ((u32)XUARTPS_IXR_RXOVR | (u32)XUARTPS_IXR_RXEMPTY |
			(u32)XUARTPS_IXR_RXFULL)) != (u32)0)
	{
		CsrRegister = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,XUARTPS_SR_OFFSET);//e0001000+2c=regaddr=e000102c
		while((CsrRegister & XUARTPS_SR_RXEMPTY)== (u32)0)
		{
			//XUartPs_WriteReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET, XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET));
			recData = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
			xQueueSendFromISR(uartRecQueue, &recData, &xHigherPriorityTaskWoken);

			CsrRegister = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_SR_OFFSET);
		}
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_TXEMPTY | (u32)XUARTPS_IXR_TXFULL))
									 != (u32)0)
	{
		/* Transmit data interrupt */
	}

	/* XUARTPS_IXR_RBRK is applicable only for Zynq Ultrascale+ MP */
	if ((IsrStatus & ((u32)XUARTPS_IXR_OVER | (u32)XUARTPS_IXR_FRAMING |
			(u32)XUARTPS_IXR_PARITY | (u32)XUARTPS_IXR_RBRK)) != (u32)0)
	{
		/* Received Error Status interrupt */
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_TOUT)) != (u32)0)
	{
		/* Received Timeout interrupt */
	}

	if((IsrStatus & ((u32)XUARTPS_IXR_DMS)) != (u32)0)
	{
		/* Modem status interrupt */
	}

	IsrStatus = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
					   XUARTPS_IMR_OFFSET);

	IsrStatus &= XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
					   XUARTPS_ISR_OFFSET);


	/* Clear the interrupt status. */
	XUartPs_WriteReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_ISR_OFFSET,
		IsrStatus);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

//串口发送函数
void bsp_uart_send(uint8_t *pData, uint16_t Size)
{
	u32 SentCount = 0U;
	SentCount = Size ;

	while(!XUartPs_IsTransmitFull(pxUart0InstPtr->Config.BaseAddress))
	{
		if(SentCount > 0)
		{
			SentCount-- ;
			/* Fill the FIFO from the buffer */
			XUartPs_WriteReg(pxUart0InstPtr->Config.BaseAddress,XUARTPS_FIFO_OFFSET,
					(*pData++ & (uint8_t)0xFFU));
		}
		else
		{

		}
	}

}
