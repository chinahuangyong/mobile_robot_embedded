/*
 * bsp_camera_time.h
 *
 *  Created on: 2019Äê6ÔÂ13ÈÕ
 *      Author: HuangYong
 */

#ifndef SRC_BSP_CAMERA_TIME_H_
#define SRC_BSP_CAMERA_TIME_H_

#include "xparameters.h"
#include "xiicps.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID

/* The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x31
#define IIC_SCLK_RATE		200000

extern XIicPs Iic;

void bsp_camera_time_init(void);
void bsp_timer_sync(void);


#ifdef _cplusplus
}
#endif

#endif /* SRC_BSP_CAMERA_TIME_H_ */
