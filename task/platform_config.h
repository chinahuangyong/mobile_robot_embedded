#ifndef __PLATFORM_CONFIG_H_
#define __PLATFORM_CONFIG_H_

#include "xscugic.h"

#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
#define PLATFORM_ZYNQ

extern XScuGic *pxInterruptController;

#endif
