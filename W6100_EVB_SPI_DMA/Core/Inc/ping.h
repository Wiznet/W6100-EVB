//! \file 		ping.h
//! \brief 		ping.c Header file.
//! \version	1.1.1
//! \data 		2017/04/04
//! \par		Revision history
//!				<2017/04/04> Notice
//!				Change function for control IPRAW Ping Request or Reply mode
//!				1. Added request_flag parameter: ping_auto(), ping_reply()
//!				2. request_flag 0: Send Request ping from outside MCU to MCU
//!                request_flag 1: Send Request ping from MCU to outside MCU
//!	\author		Matthew

#include "wizchip_conf.h"
#include "socket.h"
#include "w6100.h"


#define BUF_LEN 32
#define PING_REQUEST 8
#if 1
// 20230726 taylor
#define PING6_REQUEST 128
#endif

#define PING_REPLY 0
#define CODE_ZERO 0

#define SOCKET_ERROR 1
#define TIMEOUT_ERROR 2
#define SUCCESS 3
#define REPLY_ERROR 4
#define PING_DEBUG


typedef struct pingmsg
{
  uint8_t  Type; 		// 0 - Ping Reply, 8 - Ping Request
  uint8_t  Code;		// Always 0
  int16_t  CheckSum;	// Check sum
  int16_t  ID;	            // Identification
  int16_t  SeqNum; 	// Sequence Number
  int8_t	 Data[BUF_LEN];// Ping Data  : 1452 = IP RAW MTU - sizeof(Type+Code+CheckSum+ID+SeqNum)
} PINGMSGR;

#if 1
// 20230726 taylor
typedef struct csum
{
  uint8_t Src[16];
  uint8_t Dst[16];
  uint8_t Next[2];
  uint8_t Len_icmp6[2];
  PINGMSGR Icmpv6;
} CSUM;

#endif


uint8_t ping_auto(uint8_t s, uint8_t *addr, uint8_t request_flag);
uint8_t ping_count(uint8_t s, uint16_t pCount, uint8_t *addr);
uint8_t ping_request(uint8_t s, uint8_t *addr);
uint8_t ping_reply(uint8_t s,  uint8_t *addr, uint16_t rlen, uint8_t request_flag);
#if 1
// 20230726 taylor
uint8_t ping6_auto(uint8_t s, uint8_t *addr, uint8_t request_flag);
uint8_t ping6_count(uint8_t s, uint16_t pCount, uint8_t *addr);
uint8_t ping6_request(uint8_t s, uint8_t *addr);
uint8_t ping6_reply(uint8_t s,  uint8_t *addr, uint16_t rlen, uint8_t request_flag);
#endif
uint16_t checksum(uint8_t * data_buf, uint16_t len);
#if 1
// 20230726 taylor
uint16_t checksum6(uint8_t * data_buf, uint16_t len);
#endif
uint16_t htons( uint16_t  hostshort);	/* htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).*/
