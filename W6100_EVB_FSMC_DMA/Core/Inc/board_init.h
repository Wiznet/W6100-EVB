/*
 * board_init.h
 *
 *  Created on: Apr 19, 2019
 *      Author: taylor
 */

#ifndef __BOARD_INIT_H__
#define __BOARD_INIT_H__

#ifdef USE_STDPERIPH_DRIVER
	#warning USE_STDPERIPH_DRIVER
#include "mcu_init.h"
#endif

#include "wizchip_init.h"

void BoardInitialze(void);

#endif /* BOARD_INIT_H__ */

