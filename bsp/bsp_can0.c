/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    bsp_can0.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2019-7-10
  * @brief
  *          该通信为CAN0的驱动程序，包含CAN0的初始化函数，数据发送函数，中断处理函数
  * @history
  *          2019-7-10 HuangYong 创建文件，完成初始化函数，数据发送函数，中断处理函数；
*********************************************************************************/
#include "bsp_can0.h"

#include "platform_config.h"

//宏定义
#define CAN0_DEVICE_ID			XPAR_XCANPS_0_DEVICE_ID
#define CAN0_INTR_VEC_ID		XPAR_XCANPS_0_INTR
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS ((XCANPS_MAX_FRAME_SIZE) / (sizeof(u32)))

//CAN0的时钟源为24M，设置波特率为500K
#define BTR_SYNCJUMPWIDTH		3
#define BTR_SECOND_TIMESEGMENT	6
#define BTR_FIRST_TIMESEGMENT	15
#define BRPR_BAUD_PRESCALAR		1

//CAN0句柄
XCanPs Can0InstPtr;
//CAN0句柄指针
XCanPs *pxCan0InstPtr = &Can0InstPtr;

static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

//中断处理函数
static void can0_isr_handler(void);

//回调函数，目前没有测试，不确定是否可用
void can0_isr_callback(void) __attribute__ ((weak));

/* @beief     CAN0初始化函数
 * @return    0 - 成功 <0 表示返回失败
 * */
int8_t bsp_can0_init(void)
{
	int Status;
	XCanPs_Config *ConfigPtr;

	//在查找表中查找CAN相关定义
	ConfigPtr = XCanPs_LookupConfig(CAN0_DEVICE_ID);
	if (ConfigPtr == NULL)
	{

	}
	XCanPs_CfgInitialize(&Can0InstPtr, ConfigPtr, ConfigPtr->BaseAddr);

	//CAN0模块的自查
	Status = XCanPs_SelfTest(&Can0InstPtr);
	if (Status != XST_SUCCESS)
	{
	}

	//进入配置模式，并等待配置模式进入完成
	XCanPs_EnterMode(&Can0InstPtr, XCANPS_MODE_CONFIG);
	while(XCanPs_GetMode(&Can0InstPtr) != XCANPS_MODE_CONFIG);

	//配置分配系统
	XCanPs_SetBaudRatePrescaler(&Can0InstPtr, BRPR_BAUD_PRESCALAR);
	//配置CAN0时序相关参数
	XCanPs_SetBitTiming(&Can0InstPtr, BTR_SYNCJUMPWIDTH, BTR_SECOND_TIMESEGMENT, BTR_FIRST_TIMESEGMENT);

	//进入正常模式，CAN正常工作
	XCanPs_EnterMode(&Can0InstPtr, XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(&Can0InstPtr) != XCANPS_MODE_NORMAL);

	//配置CAN的接收中断和对应的中断处理函数
//	XCanPs_SetHandler(&Can0InstPtr, XCANPS_HANDLER_RECV, can0_isr_handler, NULL);
	//将中断处理函数连接到中断向量
	Status = XScuGic_Connect(pxInterruptController, CAN0_INTR_VEC_ID,
					  (Xil_ExceptionHandler) can0_isr_handler,
					  (void *)&Can0InstPtr);
	if (Status != XST_SUCCESS)
	{
	}
	//使用CAN中断源-使能接收FIFO非空中断
	XCanPs_IntrEnable(&Can0InstPtr, XCANPS_IXR_RXNEMP_MASK);
	//使能CAN全局中断
	XScuGic_Enable(pxInterruptController, CAN0_INTR_VEC_ID);

	return 0;
}

/* @beief     CAN0初始化函数
 * @param[in] can_msg - 需要发送的CAN数据
 * @return    0 - 成功 <0 表示返回失败
 * */
int8_t bsp_can0_send(can_msg_t can_msg)
{
	int Status;

	/* 创建一个ID */
	if(can_msg.ide)	//判断是否为拓展帧
	{
		TxFrame[0] = (u32)XCanPs_CreateIdValue(0, 0, 1, (u32)((can_msg.ext_id)&0x7FFFF), can_msg.rtr);
	}
	else
	{
		TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)can_msg.std_id, can_msg.rtr, 0, 0, 0);
	}
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)can_msg.dlc);

	//TODO:填充数据到数据段，此处移植在不同下时需考虑大小端的问题，目前考虑小端模式
	memcpy(&TxFrame[2], &can_msg.data[0], 4);
	memcpy(&TxFrame[3], &can_msg.data[4], 4);

	Status=XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
			XCANPS_SR_OFFSET);
	while((Status& XCANPS_SR_TXFLL_MASK) != 0);

	XCanPs_WriteReg(pxCan0InstPtr->CanConfig.BaseAddr,
			XCANPS_TXFIFO_ID_OFFSET, TxFrame[0]);
	XCanPs_WriteReg(pxCan0InstPtr->CanConfig.BaseAddr,
			XCANPS_TXFIFO_DLC_OFFSET, TxFrame[1]);
	XCanPs_WriteReg(pxCan0InstPtr->CanConfig.BaseAddr,
			XCANPS_TXFIFO_DW1_OFFSET, Xil_EndianSwap32(TxFrame[2]));
	XCanPs_WriteReg(pxCan0InstPtr->CanConfig.BaseAddr,
			XCANPS_TXFIFO_DW2_OFFSET, Xil_EndianSwap32(TxFrame[3]));
	Status= XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
							XCANPS_ISR_OFFSET);
	Status = XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr, XCANPS_SR_OFFSET) ;
	//TODO: 此处应该增加通过状态判断数据是否发送成功，目前预留，后续增加
	return 0;
}

/* @beief     CAN0接收中断处理函数
 * @return    无
 * */
static void can0_isr_handler(void)
{
//	CanRxMsgTypeDef *RxMessage ;
	int Status;
	u32 IntrValue;

	Status = XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr, XCANPS_ISR_OFFSET) ;
	if ((Status& XCANPS_IXR_RXNEMP_MASK) != (u32)0)
	{
		/*
		 * Read IDR, DLC, Data Word 1 and Data Word 2 from the CAN device.
		 */
		RxFrame[0] = XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
						XCANPS_RXFIFO_ID_OFFSET);
		RxFrame[1] = XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
						XCANPS_RXFIFO_DLC_OFFSET);
		RxFrame[2] = Xil_EndianSwap32(XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
						XCANPS_RXFIFO_DW1_OFFSET));
		RxFrame[3] = Xil_EndianSwap32(XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
						XCANPS_RXFIFO_DW2_OFFSET));

		/*
		 * Clear RXNEMP bit in ISR. This allows future XCanPs_IsRxEmpty() call
		 * returns correct RX FIFO occupancy/empty condition.
		 */
		IntrValue= XCanPs_ReadReg(pxCan0InstPtr->CanConfig.BaseAddr,
						XCANPS_ISR_OFFSET);
		IntrValue &=  XCANPS_IXR_RXNEMP_MASK;
		XCanPs_WriteReg(pxCan0InstPtr->CanConfig.BaseAddr, XCANPS_ICR_OFFSET,
						IntrValue);

		/* 使用weak属性设置用户回调函数 */
		can0_isr_callback();

//	  RxMessage -> IDE =(uint8_t)((XCANPS_IDR_IDE_MASK & RxFrame [0])>> 16);
//	 if(RxMessage-> IDE == TEST_MESSAGE_ID)
//	  {
//		RxMessage-> StdId =(uint32_t)((XCANPS_IDR_ID1_MASK & RxFrame [0])>> 21);
//	  }
//	 else
//	  {
//		RxMessage-> ExtId =(uint32_t)(((XCANPS_IDR_ID2_MASK & RxFrame [0])>> 1)|
//						((XCANPS_IDR_ID1_MASK & RxFrame [0])>> 3));
//	  }
//
//	  RxMessage-> RTR =(uint8_t)(XCANPS_IDR_RTR_MASK&RxFrame [0]);
//
//	  RxMessage-> DLC =(uint8_t)((XCANPS_DLCR_DLC_MASK&RxFrame [1])>> 28);
//
//
//
//	  RxMessage-> Data [0] =(uint8_t)(XCANPS_DW1R_DB3_MASK & RxFrame [2]);
//	  RxMessage-> Data [1] =(uint8_t)((XCANPS_DW1R_DB2_MASK & RxFrame [2])>> 8);
//	  RxMessage-> Data [2] =(uint8_t)((XCANPS_DW1R_DB1_MASK & RxFrame [2])>> 16);
//	  RxMessage-> Data [3] =(uint8_t)((XCANPS_DW1R_DB0_MASK & RxFrame [2])>> 24);
//
//	  RxMessage-> Data [4] =(uint8_t)(XCANPS_DW2R_DB7_MASK & RxFrame [3]);
//	  RxMessage-> Data [5] =(uint8_t)((XCANPS_DW2R_DB6_MASK & RxFrame [3])>> 8);
//	  RxMessage-> Data [6] =(uint8_t)((XCANPS_DW2R_DB5_MASK & RxFrame [3])>> 16);
//	  RxMessage-> Data [7] =(uint8_t)((XCANPS_DW2R_DB4_MASK & RxFrame [3])>> 24);
	}
}




