#include <stdio.h>
#include "HAL_Config.h"
#include "HALInit.h"
#include "wizchip_conf.h"
#include "inttypes.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "W6100RelFunctions.h"
#include "serialCommand.h"
#include "stm32f10x_rcc.h"
#include "loopback.h"
#include "AddressAutoConfig.h"

wiz_NetInfo gWIZNETINFO = { .mac = {
								0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
								},
							.ip = {
								192, 168, 100, 140
							},
							.sn = {
								255, 255, 255, 0
							},
							.gw = {
								192, 168, 100, 1
							},
							.dns = {
								192, 168, 100, 1
							},
							.lla={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.gua={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.sn6={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.gw6={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							}
};

wiz_NetInfo gWIZNETINFO_M = { .mac = {0x00,0x08,0xdc,0xFF,0xFF,0xFF},
							.ip = {192,168,177,25},
							.sn = {255, 255, 255, 0},
							.gw = {192, 168, 177, 1},
							.dns = {168, 126, 63, 1},
							//.dhcp = NETINFO_STATIC,
							.lla={0xfe,0x80,0x00,0x00,
									0x00,0x00, 0x00,0x00,
									0x02,0x08, 0xdc,0xff,
									0xfe,0x57, 0x57,0x25},   ///< Source Link Local Address
							.gua={0x20,0x01,0x02,0xb8,
									0x00,0x10,0x00,0x01,
									0x02,0x08,0xdc,0xff,
									0xfe,0x57,0x57,0x25},   ///< Source Global Unicast Address
							.sn6={0xff,0xff,0xff,0xff,
									0xff,0xff,0xff,0xff,
									0x00,0x00,0x00, 0x00,
									0x00,0x00,0x00,0x00 },   ///< IPv6 Prefix
							.gw6={0xfe, 0x80, 0x00,0x00,
									0x00,0x00,0x00,0x00,
									0x02,0x00, 0x87,0xff,
									0xfe,0x08, 0x4c,0x81}   ///< Gateway IPv6 Address
};

uint8_t WIZ_Dest_IP_virtual[4] = {192, 168, 0, 230};                  //DST_IP Address
uint8_t WIZ_Dest_IP_Google[4] = {216, 58, 200, 174};              //DST_IP Address

uint8_t mcastipv4_0[4] ={239,1,2,3};
uint8_t mcastipv4_1[4] ={239,1,2,4};
uint8_t mcastipv4_2[4] ={239,1,2,5};
uint8_t mcastipv4_3[4] ={239,1,2,6};

uint16_t WIZ_Dest_PORT = 15000;                                 //DST_IP port

//////////////////////////////////////////////////////////////////////
/*******************IPv6  ADDRESS**************************/
//////////////////////////////////////////////////////////////////////

uint8_t Zero_IP[16] = {0x00, };

#define ETH_MAX_BUF_SIZE	1024
uint8_t  remote_ip[4] = {192,168,177,200};                      //
uint16_t remote_port = 8080;
unsigned char ethBuf0[ETH_MAX_BUF_SIZE];
unsigned char ethBuf1[ETH_MAX_BUF_SIZE];
unsigned char ethBuf2[ETH_MAX_BUF_SIZE];
unsigned char ethBuf3[ETH_MAX_BUF_SIZE];
unsigned char ethBuf4[ETH_MAX_BUF_SIZE];
unsigned char ethBuf5[ETH_MAX_BUF_SIZE];
unsigned char ethBuf6[ETH_MAX_BUF_SIZE];
unsigned char ethBuf7[ETH_MAX_BUF_SIZE];

uint8_t bLoopback = 1;
uint8_t bRandomPacket = 0;
uint8_t bAnyPacket = 0;
uint16_t pack_size = 0;

void print_network_information(void);

int main(void)
{
	volatile int i;
	volatile int j,k;
 	uint16_t ver=0;
 	uint16_t curr_time = 0;
	uint8_t syslock = SYS_NET_LOCK;
	uint8_t svr_ipv4[4] = {192, 168, 177, 235};
	uint8_t svr_ipv6[16] = {0xfe, 0x80, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00,
							0xc1, 0x0b, 0x0a, 0xdf,
							0xea, 0xf4, 0xf4, 0x2d};

	RCC_ClocksTypeDef RCCA_TypeDef;
	RCCInitialize();
	gpioInitialize();
	usartInitialize();
	timerInitialize();
	printf("System start.\r\n");




#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_
	/* SPI method callback registration */
	#if defined SPI_DMA
		reg_wizchip_spi_cbfunc(spiReadByte, spiWriteByte,spiReadBurst,spiWriteBurst);
	#else
		reg_wizchip_spi_cbfunc(spiReadByte, spiWriteByte,0,0);
	#endif
	/* CS function register */
	reg_wizchip_cs_cbfunc(csEnable,csDisable);
#else
	/* Indirect bus method callback registration */
	#if defined BUS_DMA
			reg_wizchip_bus_cbfunc(busReadByte, busWriteByte,busReadBurst,busWriteBurst);
	#else
			reg_wizchip_bus_cbfunc(busReadByte, busWriteByte,0,0);
	#endif
#endif
#if _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_
	FSMCInitialize();
#else
	spiInitailize();
#endif

	resetAssert();
	delay(10);
	resetDeassert();
	delay(10);

	W6100Initialze();
	ctlwizchip(CW_SYS_UNLOCK, &syslock);
	
	// Set Mac and IPv4
	ctlnetwork(CN_SET_NETINFO, &gWIZNETINFO);
	print_network_information();

	// Auto Set IPv6
	if(1 != AddressAutoConfig_Init(&gWIZNETINFO))
	{
		// Manual Set IPv6
		gWIZNETINFO = gWIZNETINFO_M;
		ctlnetwork(CN_SET_NETINFO,&gWIZNETINFO);
	}

	// Get My Address
	printf("Register value after W6100 initialize!\r\n");
	print_network_information();


	while(1)
    {
	    loopback_udps(0,ethBuf0,50000,AS_IPV4);
	    loopback_udps(1,ethBuf1,50001,AS_IPV6);
		loopback_tcps(2,ethBuf2,50002,AS_IPV4);
		loopback_tcps(3,ethBuf3,50003,AS_IPV6);
		loopback_tcps(4,ethBuf4,50004,AS_IPDUAL);


    }
}

void delay(unsigned int count)
{
	int temp;
	temp = count + TIM2_gettimer();
	while(temp > TIM2_gettimer()){}
}

void print_network_information(void)
{


    uint8_t tmp_array[16];
    uint8_t i;
	wizchip_getnetinfo(&gWIZNETINFO);
	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("IP address : %d.%d.%d.%d\n\r",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("SM Mask	   : %d.%d.%d.%d\n\r",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	getGA6R(tmp_array);
    printf("GW6 : %04X:%04X", ((uint16_t)tmp_array[0] << 8) | ((uint16_t)tmp_array[1]),
    		((uint16_t)tmp_array[2] << 8) | ((uint16_t)tmp_array[3]));
    printf(":%04X:%04X", ((uint16_t)tmp_array[4] << 8) | ((uint16_t)tmp_array[5]),
    		((uint16_t)tmp_array[6] << 8) | ((uint16_t)tmp_array[7]));
    printf(":%04X:%04X", ((uint16_t)tmp_array[8] << 8) | ((uint16_t)tmp_array[9]),
    		((uint16_t)tmp_array[10] << 8) | ((uint16_t)tmp_array[11]));
    printf(":%04X:%04X\r\n", ((uint16_t)tmp_array[12] << 8) | ((uint16_t)tmp_array[13]),
    		((uint16_t)tmp_array[14] << 8) | ((uint16_t)tmp_array[15]));

	getLLAR(tmp_array);
	printf("LLA : %04X:%04X", ((uint16_t)tmp_array[0] << 8) | ((uint16_t)tmp_array[1]),
			((uint16_t)tmp_array[2] << 8) | ((uint16_t)tmp_array[3]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[4] << 8) | ((uint16_t)tmp_array[5]),
			((uint16_t)tmp_array[6] << 8) | ((uint16_t)tmp_array[7]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[8] << 8) | ((uint16_t)tmp_array[9]),
			((uint16_t)tmp_array[10] << 8) | ((uint16_t)tmp_array[11]));
	printf(":%04X:%04X\r\n", ((uint16_t)tmp_array[12] << 8) | ((uint16_t)tmp_array[13]),
			((uint16_t)tmp_array[14] << 8) | ((uint16_t)tmp_array[15]));
	getGUAR(tmp_array);
	printf("GUA : %04X:%04X", ((uint16_t)tmp_array[0] << 8) | ((uint16_t)tmp_array[1]),
			((uint16_t)tmp_array[2] << 8) | ((uint16_t)tmp_array[3]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[4] << 8) | ((uint16_t)tmp_array[5]),
			((uint16_t)tmp_array[6] << 8) | ((uint16_t)tmp_array[7]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[8] << 8) | ((uint16_t)tmp_array[9]),
			((uint16_t)tmp_array[10] << 8) | ((uint16_t)tmp_array[11]));
	printf(":%04X:%04X\r\n", ((uint16_t)tmp_array[12] << 8) | ((uint16_t)tmp_array[13]),
			((uint16_t)tmp_array[14] << 8) | ((uint16_t)tmp_array[15]));

	getSUB6R(tmp_array);
	printf("SUB6 : %04X:%04X", ((uint16_t)tmp_array[0] << 8) | ((uint16_t)tmp_array[1]),
			((uint16_t)tmp_array[2] << 8) | ((uint16_t)tmp_array[3]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[4] << 8) | ((uint16_t)tmp_array[5]),
			((uint16_t)tmp_array[6] << 8) | ((uint16_t)tmp_array[7]));
	printf(":%04X:%04X", ((uint16_t)tmp_array[8] << 8) | ((uint16_t)tmp_array[9]),
			((uint16_t)tmp_array[10] << 8) | ((uint16_t)tmp_array[11]));
	printf(":%04X:%04X\r\n", ((uint16_t)tmp_array[12] << 8) | ((uint16_t)tmp_array[13]),
			((uint16_t)tmp_array[14] << 8) | ((uint16_t)tmp_array[15]));


	printf("\r\nNETCFGLOCK : %x\r\n", getNETLCKR());
}
