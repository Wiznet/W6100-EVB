#ifndef _ADDRESSAUTOCONFIG_
#define _ADDRESSAUTOCONFIG_

#include "stdio.h"
#include "stdint.h"
#include "w6100.h"
#include "socket.h"
#include "dhcpv6.h"

#define AutoConfig_debug debug_on
#define debug_on 1
#define debug_off 0

#define PROTOCOL_NUM_ICMPv6 58

#define ROUTER_ADVERTISEMENT 134

#define SUCCESS 0
#define ERROR_DAD_FAIL -1
#define ERROR_SLCMD    -2
#define ERROR_TIMEOUT  -3

#define SLAAC_RDNSS  0
#define SLAAC_DHCP6  1
#define SFAAC_DHCP6  3

#define RAO_SLLA	 1
#define RAO_TLLA	 2
#define RAO_PI		 3
#define RAO_RH		 4
#define RAO_MTU		 5
#define RAO_RDNS	25

void Generate_EUI64(uint8_t *mac_addr, uint8_t *Link_Local_Addr);
uint8_t Duplicate_Address_Detection(wiz_NetInfo* netinfo);
uint8_t Address_Auto_Config_RA(uint8_t sn, uint8_t *icmpbuf, uint16_t buf_size, wiz_NetInfo* netinfo);
uint8_t Address_Auto_Config_SLDHCP(uint8_t sn, uint8_t* test_buf);
uint8_t Address_Auto_Config_SFDHCP(uint8_t sn, uint8_t* test_buf);

#endif // _ADDRESSAUTOCONFIG_
