#include "board_init.h"
#include "wizchip_init.h"

void BoardInitialze(void)
{

#ifdef USE_STDPERIPH_DRIVER
	RCCInitialize();
	gpioInitialize();
	usartInitialize();
	timerInitialize();

	printf("System start.\r\n");

#if _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_
	FSMCInitialize();
#else
	spiInitailize();
#endif
#endif

	W6100Initialze();

}

