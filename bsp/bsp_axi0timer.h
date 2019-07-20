/*
 * bsp_timer.h
 *
 *  Created on: 2019��6��11��
 *      Author: HuangYong
 */

#ifndef _BSP_AXI0TIMER_H_
#define _BSP_AXI0TIMER_H_

#include "xparameters.h"
#include "xtmrctr.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

extern XTmrCtr TimerInstancePtr1;	//

void bsp_axi0timer_init(void);
void bsp_axi0timer_start(void);
uint64_t bsp_axi0timer_get(void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BSP_TIMER_H_ */
