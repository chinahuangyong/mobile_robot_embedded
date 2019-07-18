/*******************************************************************************
    All Copyright(c) 1992-2017 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    task_net.c
  * @author  HuangYong
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-05-06
  * @brief   ���������������������������������������������������������
  * @history
       2019.06.20  ����������������
********************************************************************************/
#include "task_net.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "netif/etharp.h"

#include "bsp_laser.h"
#include "bsp_timer.h"

#include <stdint.h>

void lwip_init();

#define XEMCIF_THREAD_STACKSIZE 1024

#define TASK_NET_PRIO (configMAX_PRIORITIES - 2)
#define TASK_NET_STK_SIZE 1024
TaskHandle_t handle_net;

static struct netif server_netif;
struct netif *echo_netif;

void task_net(void* argument);

QueueHandle_t xTimeStamp = NULL;

void task_net_init(void)
{
//	/* initialize lwIP before calling sys_thread_new */
//	lwip_init();
//
//	struct netif *netif;
//	struct ip_addr ipaddr, netmask, gw;
//	/* the mac address of the board. this should be unique per board */
//	unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
//
//	netif = &server_netif;
//
//	/* initliaze IP addresses to be used */
//	IP4_ADDR(&ipaddr, 192, 168, 2, 11);
//	IP4_ADDR(&netmask, 255, 255, 255, 0);
//	IP4_ADDR(&gw, 192, 168, 2, 1);
//
//	/* print out IP settings of the board */
//	xil_printf("\r\n\r\n");
//	xil_printf("-----lwIP Socket Mode Echo server Demo Application ------\r\n");
//
//	/* Add network interface to the netif_list, and set it as default */
//	if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR))
//	{
//		xil_printf("Error adding N/W interface\r\n");
//		return;
//	}
//	netif_set_default(netif);
//
//	/* specify that the network if is up */
//	netif_set_up(netif);
//
//	/* start packet receive thread - required for lwIP operation */
//	sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
//					XEMCIF_THREAD_STACKSIZE,
//					DEFAULT_THREAD_PRIO);
//
//	xTaskCreate((TaskFunction_t)task_net,
//	              (const char *)"task_net",
//	              (uint16_t)TASK_NET_STK_SIZE,
//	              (void *)NULL,
//	              (UBaseType_t)TASK_NET_PRIO,
//	              (TaskHandle_t *)&handle_net);
//
//	xTimeStamp = xQueueCreate(1000, sizeof(time_u_t));
}

char data[50000];
uint16_t real_lens = 0;
uint64_t time_stamp2 = 0;


void task_net(void* argument)
{
	int ret;

	time_u_t time_u;

	bsp_laser_device_init();
	bsp_laser_scan_init();

	for(;;)
	{
		memset(data, 0, 50000);
		ret = bsp_laser_get_datagram(data, 50000, &real_lens);
		if(ret < 0)
		{
			real_lens = 0;
		}
		else
		{
			time_stamp2 = bsp_timer_get();
			time_u.time_l = (uint32_t)time_stamp2;
			time_u.time_h = (uint32_t)(time_stamp2>>32);
			time_u.len = real_lens;
			xQueueSend(xTimeStamp, &time_u, 1);
		}
	}
}
