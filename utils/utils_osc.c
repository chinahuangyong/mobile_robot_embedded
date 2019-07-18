/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    util_osc.c
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-11-10
  * @brief
  *          该通信接口程序为和上位机通信程序，上位机软件为Serial_Digital ScopeV2.0
  * @history
  *          2019-7-10 HuangYong 创建文件，完成crc校验程序和数据发送程序；
*********************************************************************************/
#include "utils_osc.h"

#include "xil_printf.h"

/* @beief     osc的crc校验函数
 * @param[in] buff - 需要校验的数据
 * @param[in] buff_num - 需要校验的数据长度
 * @return    返回计算的校验值
 * */
static uint16_t utils_osc_check_crc(uint8_t* buff, uint8_t buff_num)
{
	uint16_t crc_temp;
	uint8_t i,j;
	crc_temp = 0xffff;

	for (i=0; i<buff_num; i++){
		crc_temp ^= buff[i];
		for (j=0;j<8;j++)
		{
			if (crc_temp & 0x01)
				crc_temp = (crc_temp >>1 ) ^ 0xA001;
			else
				crc_temp = crc_temp >> 1;
		}
	}
	return(crc_temp);
}

/* @beief     osc的数据包发送函数
 * @param[in] ch1 - 通道1数据
 * @param[in] ch2 - 通道2数据
 * @param[in] ch3 - 通道3数据
 * @param[in] ch4 - 通道4数据
 * @return    无
 * */
void utils_osc_send(int16_t  ch1, int16_t ch2, int16_t ch3, int16_t ch4)
{
	uint8_t data_buff[10] = {0};
	uint8_t i;
	uint16_t crc16 = 0;

	data_buff[0] = (ch1 & 0x00FF);
	data_buff[1] = (ch1 & 0xFF00) >> 8;
	data_buff[2] = (ch2 & 0x00FF);
	data_buff[3] = (ch2 & 0xFF00) >> 8;
	data_buff[4] = (ch3 & 0x00FF);
	data_buff[5] = (ch3 & 0xFF00) >> 8;
	data_buff[6] = (ch4 & 0x00FF);
	data_buff[7] = (ch4 & 0xFF00) >> 8;

	crc16 = utils_osc_check_crc(data_buff,8);
	data_buff[8] = crc16%256;
	data_buff[9] = crc16/256;

	for(i=0; i<10; i++)
	{
		xil_printf("%c", data_buff[i]);
	}
}
