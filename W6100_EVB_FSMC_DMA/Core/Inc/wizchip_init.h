#ifndef __WIZCHIP_INIT_H__
#define __WIZCHIP_INIT_H__

//#define WIZCHIP_REGISTER_DUMP

#ifdef USE_STDPERIPH_DRIVER
	#warning USE_STDPERIPH_DRIVER

#include "stm32f10x_conf.h"
#include "serialCommand.h"
#include "mcu_init.h"


#elif defined USE_HAL_DRIVER
	#warning USE_HAL_DRIVER
#include "main.h"
#include "wizchip_conf.h"

/* RESET */
#define W6100_RESET_PIN		    GPIO_PIN_8
#define W6100_RESET_PORT		GPIOD

/* SPI2 */
#if (_WIZCHIP_IO_MODE_ &_WIZCHIP_IO_MODE_SPI_)
#define W6100_CS_PIN			GPIO_PIN_7
#define W6100_CS_PORT			GPIOD

extern SPI_HandleTypeDef hspi2;
#define W6100_SPI  				hspi2

#define W6100_SPI_SIZE          1
#define W6100_SPI_TIMEOUT       10
#else


#endif

#else
	#error Error!! STD_DRIVER not defined

#endif



#if (_WIZCHIP_IO_MODE_==_WIZCHIP_IO_MODE_BUS_INDIR_)
	#ifdef DMA
   	   #define BUS_DMA
	#endif
#elif(_WIZCHIP_IO_MODE_== _WIZCHIP_IO_MODE_SPI_VDM_)||(_WIZCHIP_IO_MODE_== _WIZCHIP_IO_MODE_SPI_FDM_)
	#ifdef DMA
		#define SPI_DMA
	#endif
#endif

void W6100Initialze(void);
void W6100Reset(void);

uint8_t W6100SpiReadByte(void);
void W6100SpiWriteByte(uint8_t byte);

uint8_t W6100SpiDummyReadBurst(uint8_t* pBuf, uint16_t len);
void W6100SpiDummyWriteBurst(uint8_t* pBuf, uint16_t len);

#if defined SPI_DMA
uint8_t W6100SpiReadBurst(uint8_t* pBuf, uint16_t len);
void W6100SpiWriteBurst(uint8_t* pBuf, uint16_t len);
#endif

void W6100BusWriteByte(uint32_t addr, iodata_t data);
iodata_t W6100BusReadByte(uint32_t addr);

#if defined BUS_DMA

void W6100BusDmaWriteByte(uint32_t addr, iodata_t data);
iodata_t W6100BusDmaReadByte(uint32_t addr);

void W6100BusWriteBurst(uint32_t addr, uint8_t* pBuf ,uint32_t len,uint8_t addr_inc);
void W6100BusReadBurst(uint32_t addr,uint8_t* pBuf, uint32_t len,uint8_t addr_inc);
#endif

void W6100Reset(void);
void W6100CsEnable(void);
void W6100CsDisable(void);
void W6100ResetAssert(void);
void W6100ResetDeassert(void);

#ifdef WIZCHIP_REGISTER_DUMP
void W6100Register_read(void);
void W6100SocketRegister_read(uint8_t sn);
#endif

#endif
