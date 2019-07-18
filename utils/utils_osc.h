/*******************************************************************************
    All Copyright(c) 2018-2019 Receive By Guangzhou Shiyuan Electronics CO.Ltd.
  * @file    util_osc.h
  * @author  HuangYong(huangyong6295@cvte.com)
  * @version V0.0.1
  * @Company CVTE
  * @date    2017-11-10
  * @brief
  *          该通信接口程序为和上位机通信程序，上位机软件为Serial_Digital ScopeV2.0
  * @history
  *          2019-7-10 HuangYong 创建文件，完成crc校验程序和数据发送程序；
*********************************************************************************/
#ifndef __UTILS_OSC_H_
#define __UTILS_OSC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* @beief     osc的数据包发送函数
 * @param[in] ch1 - 通道1数据
 * @param[in] ch2 - 通道2数据
 * @param[in] ch3 - 通道3数据
 * @param[in] ch4 - 通道4数据
 * @return    无
 * */
void utils_osc_send(int16_t  ch1, int16_t ch2, int16_t ch3, int16_t ch4);

#ifdef __cpluslus
}
#endif

#endif /* SRC_UTILS_UTILS_OSC_H_ */
