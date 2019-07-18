/*
 * bsp_uart.h
 *
 *  Created on: 2019��6��14��
 *      Author: HuangYong
 */

#ifndef SRC_BSP_UART_H_
#define SRC_BSP_UART_H_

#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xstatus.h"
//#include "platform_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef _cplusplus
extern "C"{
#endif

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define UART_DEVICE_ID		XPAR_PS7_UART_0_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_0_INTR

extern XUartPs Uart0InstPtr;		/* Instance of the UART Device */
extern XScuGic Intc0InstPtr;	    /* Instance of the Interrupt Controller */
extern QueueHandle_t uartRecQueue;
extern XScuGic *pxInterruptController;
void bsp_uart_init(void);
void bsp_uart_send(uint8_t *pData, uint16_t Size) ;

#ifdef _cplusplus
}
#endif


#endif /* SRC_BSP_UART_H_ */
