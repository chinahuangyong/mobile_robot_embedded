/*
 * bsp_timer.h
 *
 *  Created on: 2019��6��11��
 *      Author: HuangYong
 */

#ifndef SRC_BSP_TIMER_H_
#define SRC_BSP_TIMER_H_

#include "xparameters.h"
#include "xtmrctr.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

extern XTmrCtr TimerInstancePtr1;	//ϵͳʱ���������
extern XTmrCtr TimerInstancePtr2;	//ϵͳͬ����ʱ������ʱ���1s

void bsp_timer_init(void);
void bsp_timer_start(void);
uint64_t bsp_timer_get(void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BSP_TIMER_H_ */
