/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_uart0.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该通信为UART0的驱动程序，包含UART0的初始化函数，数据发送函数，中断处理函数
  * @history
  *          2019-7-10 HuangYong 创建文件，完成初始化函数，数据发送函数，中断处理函数；
*********************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "platform_config.h"

#include "bsp_uart0.h"

//宏定义
#define UART_RECE_QUEUE_SIZE 200	//串口接收队列数据长度

#define UART_DEVICE_ID		XPAR_PS7_UART_0_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_0_INTR

//全局变量
XUartPs Uart0InstPtr;
XUartPs *pxUart0InstPtr = &Uart0InstPtr;

//串口接收队列句柄
QueueHandle_t uart_rec_queue = NULL;

//中断处理函数
void uart0_isr_handle(void);

/* @beief     UART0初始化函数
 * @return    0-成功 <0-失败
 * */
int bsp_uart0_init(void)
{
	int status;
	XUartPs_Config *config;

	//创建队列
	uart_rec_queue = xQueueCreate(UART_RECE_QUEUE_SIZE, sizeof(u32));
	if(uart_rec_queue == NULL)
	{
		return -1;
	}
	
	//查找UART0的配置信息
	config = XUartPs_LookupConfig(UART_DEVICE_ID);
	if (NULL == config)
	{
		return -1;
	}

	//初始化UART0的相关寄存器
	status = XUartPs_CfgInitialize(pxUart0InstPtr, config, config->BaseAddress);
	if (status != XST_SUCCESS)
	{
		return -1;
	}

	//检测UART0硬件
	status = XUartPs_SelfTest(pxUart0InstPtr);
	if (status != XST_SUCCESS)
	{
		return -1;
	}

	//设置串口的操作模式为正常模式
	XUartPs_SetOperMode(pxUart0InstPtr, XUARTPS_OPER_MODE_NORMAL);
	//设置Fifo接收触发阈值为1
	XUartPs_SetFifoThreshold(pxUart0InstPtr, 1);
	//使用串口接收中断作为中断源
	XUartPs_SetInterruptMask(pxUart0InstPtr, XUARTPS_IXR_RXOVR);
	//设置串口的接收超时时间
	XUartPs_SetRecvTimeout(pxUart0InstPtr, 8);
	//将中断函数与中断向量绑定
	status = XScuGic_Connect(pxInterruptController, UART_INT_IRQ_ID,
				  (Xil_ExceptionHandler) uart0_isr_handle,
				  (void *) pxUart0InstPtr);
	if (status != XST_SUCCESS)
	{
		return -1;
	}
	//是能UART0模块
	XScuGic_Enable(pxInterruptController, UART_INT_IRQ_ID);

	return 0;


}

/* @beief     UART0数据发送函数
 * @param[in] 需要发送的字符串数组指针
 * @param[in] 数据大小
 * @return    0-成功 <0-失败
 * */
int bsp_uart0_send(uint8_t *p_data, uint16_t size)
{
	uint16_t SentCount = 0U;
	SentCount = size ;

	while(!XUartPs_IsTransmitFull(pxUart0InstPtr->Config.BaseAddress))
	{
		if(SentCount > 0)
		{
			SentCount-- ;
			//字符串填充写串口FIFO
			XUartPs_WriteReg(pxUart0InstPtr->Config.BaseAddress,XUARTPS_FIFO_OFFSET,
					(*p_data++ & (uint8_t)0xFFU));
		}
		else
		{

		}
	}
	return 0;
}

/* @beief     UART0中断函数
 * @return    无
 * */
void uart0_isr_handle(void)
{
	u32 isr_status;
	u32 csr_reg;

	u32 rec_data;

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	Xil_AssertVoid(pxUart0InstPtr != NULL);
	Xil_AssertVoid(pxUart0InstPtr->IsReady == XIL_COMPONENT_IS_READY);

	//读取中断状态寄存器
	isr_status = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);

	isr_status &= XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
				   XUARTPS_ISR_OFFSET);

	//读取中断寄存器，查询对应的状态位是否置位
	if((isr_status & ((u32)XUARTPS_IXR_RXOVR | (u32)XUARTPS_IXR_RXEMPTY |
			(u32)XUARTPS_IXR_RXFULL)) != (u32)0)
	{
		csr_reg = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,XUARTPS_SR_OFFSET);//e0001000+2c=regaddr=e000102c
		while((csr_reg & XUARTPS_SR_RXEMPTY)== (u32)0)
		{
			//读取FIFO中接收数据
			rec_data = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
			//将数据发送到队列中
			xQueueSendFromISR(uart_rec_queue, &rec_data, &xHigherPriorityTaskWoken);

			csr_reg = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_SR_OFFSET);
		}
	}

	if((isr_status & ((u32)XUARTPS_IXR_TXEMPTY | (u32)XUARTPS_IXR_TXFULL)) != (u32)0)
	{
		//数据传输中断
	}
	if ((isr_status & ((u32)XUARTPS_IXR_OVER | (u32)XUARTPS_IXR_FRAMING |
			(u32)XUARTPS_IXR_PARITY | (u32)XUARTPS_IXR_RBRK)) != (u32)0)
	{
		//接收错误状态中断
	}
	if((isr_status & ((u32)XUARTPS_IXR_TOUT)) != (u32)0)
	{
		//接收超时中断
	}
	if((isr_status & ((u32)XUARTPS_IXR_DMS)) != (u32)0)
	{
		/* Modem status interrupt */
	}

	isr_status = XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
					   XUARTPS_IMR_OFFSET);

	isr_status &= XUartPs_ReadReg(pxUart0InstPtr->Config.BaseAddress,
					   XUARTPS_ISR_OFFSET);
	//清除中断状态寄存器
	XUartPs_WriteReg(pxUart0InstPtr->Config.BaseAddress, XUARTPS_ISR_OFFSET,
			isr_status);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
