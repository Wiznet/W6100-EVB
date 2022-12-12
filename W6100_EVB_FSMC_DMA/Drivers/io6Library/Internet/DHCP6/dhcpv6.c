//*****************************************************************************
//
//! \file dhcpv6.c
//! \brief DHCPv6 APIs implement file.
//! \details Processig DHCPv6 protocol as SOLICIT, ADVERTISE.
//! \version 0.0.1
//! \date 2016/06/08
//! \par  Revision history
//!       <2016/07/18> 1st Release
//! \author JustinKim
//! \copyright
//!
//! Copyright (c)  2016, WIZnet Co., LTD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include "dhcpv6.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* If you want to display debug & procssing message, Define _DHCP_DEBUG_ in dhcp.h */
#define _DHCP6_DEBUG_
#ifdef _DHCP6_DEBUG_
#include <stdio.h>
#endif

/* DHCP6 state machine. */
#define STATE_DHCP6_INIT 0      ///< Initialize
#define STATE_DHCP6_SOLICIT 1   ///< send DISCOVER and wait OFFER
#define STATE_DHCP6_REQUEST 2   ///< send REQEUST and wait ACK or NACK
#define STATE_DHCP6_LEASED 3    ///< ReceiveD ACK and IP leased
#define STATE_DHCP6_REREQUEST 4 ///< send REQUEST for maintaining leased IP
#define STATE_DHCP6_RELEASE 5   ///< No use
#define STATE_DHCP6_STOP 6      ///< Stop procssing DHCP

/* DHCP6 message type */
#define DHCP6_SOLICIT 1       ///< DISCOVER message in OPT of @ref RIP_MSG
#define DHCP6_ADVERTISE 2     ///< OFFER message in OPT of @ref RIP_MSG
#define DHCP6_REQUEST 3       ///< REQUEST message in OPT of @ref RIP_MSG
#define DHCP6_CONFIRM 4       ///< DECLINE message in OPT of @ref RIP_MSG
#define DHCP6_RENEW 5         ///< ACK message in OPT of @ref RIP_MSG
#define DHCP6_REBIND 6        ///< NACK message in OPT of @ref RIP_MSG
#define DHCP6_REPLY 7         ///< RELEASE message in OPT of @ref RIP_MSG. No use
#define DHCP6_RELEASE 8       ///< INFORM message in OPT of @ref RIP_MSG. No use
#define DHCP6_DECLINE 9       ///< INFORM message in OPT of @ref RIP_MSG. No use
#define DHCP6_RECONFIGURE 10  ///< INFORM message in OPT of @ref RIP_MSG. No use
#define DHCP6_INFO_REQUEST 11 ///< INFORM message in OPT of @ref RIP_MSG. No use

uint8_t DNS6_Address[16] = {
    0,
};
//todo

/* 
 * @brief DHCPv6 option (cf. RFC3315)
 */
enum
{
    OPT_CLIENTID = 1,
    OPT_SERVERID = 2,
    OPT_IANA = 3,
    OPT_IATA = 4,
    OPT_IAADDR = 5,
    OPT_REQUEST = 6,
    OPT_PREFERENCE = 7,
    OPT_ELAPSED_TIME = 8,
    OPT_RELAY_MSG = 9,
    OPT_AUTH = 11,
    OPT_UNICAST = 12,
    OPT_STATUS_CODE = 13,
    OPT_RAPID_COMMIT = 14,
    OPT_USER_CLASS = 15,
    OPT_VENDOR_CLASS = 16,
    OPT_VENDOR_OPTS = 17,
    OPT_INTERFACE_ID = 18,
    OPT_RECONF_MSG = 19,
    OPT_RECONF_ACCEPT = 20,
    SIP_Server_DNL = 21,
    SIP_Server_V6ADDR = 22,
    DNS_RecursiveNameServer = 23,
    Domain_Search_List = 24,
    OPT_IAPD = 25,
    OPT_IAPREFIX = 26,
    OPT_NIS_SERVERS = 27,
    OPT_NISP_SERVERS = 28,
    OPT_NIS_DOMAIN_NAME = 29,
    OPT_NISP_DOMAIN_NAME = 30,
    OPT_LIFETIME = 32,
    FQ_DOMAIN_NAME = 39
};

/*
 * @brief DHCP message format
 */
typedef struct
{
    uint8_t *OPT; ///< Option
} __attribute__((packed)) RIP_MSG;

uint8_t DHCP_SOCKET; // Socket number for DHCP

// Network information from DHCP Server
uint8_t DHCP_allocated_ip[16] = {
    0,
};                                    // IP address from DHCPv6
int8_t dhcp_state = STATE_DHCP6_INIT; // DHCP state
int8_t dhcp_retry_count = 0;

volatile uint32_t dhcp_tick_1s = 0; // unit 1 second
uint32_t dhcp_tick_next = DHCP_WAIT_TIME;

uint32_t DHCP_XID; // Any number

RIP_MSG pDHCPMSG; // Buffer pointer for DHCP processing

uint8_t HOST_NAME[] = DCHP_HOST_NAME;

uint8_t DHCP_CHADDR[6]; // DHCP Client MAC address.

/* send DISCOVER message to DHCP server */
void send_DHCP_SOLICIT(void);
uint8_t send_DHCP_REQUEST(void);

/* check the timeout in DHCP process */
uint8_t check_DHCP_timeout(void);

/* Intialize to timeout process.  */
void reset_DHCP_timeout(void);

uint16_t DUID_type_s;
uint16_t Hardware_type_s;
uint8_t Time_s[4];
uint32_t Enterprise_num_s;
uint8_t Server_MAC[6];
uint8_t recv_IP[16];
uint32_t PreLifeTime;
uint32_t ValidLifeTime;
uint16_t code;
uint8_t IAID[4];
uint8_t T1[4];
uint8_t T2[4];
uint16_t iana_len;
uint16_t iaaddr_len;
uint16_t statuscode_len;
uint16_t Lstatuscode_len;
uint16_t serverid_len;
uint16_t clientid_len;
uint8_t status_msg[] = "";

unsigned size;
unsigned num;
unsigned num2 = 0;
unsigned growby;

/**
 * @brief 
 * 
 * @param asize 
 * @param agrowby 
 */
void InitDhcpOption(unsigned asize, unsigned agrowby)
{
    size = asize;
    growby = agrowby;
    num = 0;
}

/**
 * @brief 
 * 
 * @param value 
 */
void AppendDhcpOption(uint8_t value)
{
    uint32_t need;

    need = num + 1;
    if (need > size)
    {
        size++;
    }
    pDHCPMSG.OPT[num] = value;
    num++;
}

/**
 * @brief 
 * 
 * @param sMark 
 */
void DumpDhcpOption(char *sMark)
{
    unsigned i;
    printf("%20s => size=%02d,num=%02d : ", sMark, size, num);
    for (i = num2; i < num; i++)
    {
        printf("%.2x ", pDHCPMSG.OPT[i]);
    }
    printf("\r\n");
    num2 = num;
}

/**
 * @brief 
 * 
 * @param option 
 */
void DHCP_Option_Select(uint8_t option)
{
    switch (option)
    case OPT_CLIENTID:
    case OPT_SERVERID:
    case OPT_IANA:
    case OPT_IATA:
    case OPT_IAADDR:
    case OPT_REQUEST:
    case OPT_PREFERENCE:
    case OPT_ELAPSED_TIME:
    case OPT_RELAY_MSG:
    case OPT_AUTH:
    case OPT_UNICAST:
    case OPT_STATUS_CODE:
    case OPT_RAPID_COMMIT:
    case OPT_USER_CLASS:
    case OPT_VENDOR_CLASS:
    case OPT_VENDOR_OPTS:
    case OPT_INTERFACE_ID:
    case OPT_RECONF_MSG:
    case OPT_RECONF_ACCEPT:
    case SIP_Server_DNL:
    case SIP_Server_V6ADDR:
    case DNS_RecursiveNameServer:
    case Domain_Search_List:
    case OPT_IAPD:
    case OPT_IAPREFIX:
    case OPT_NIS_SERVERS:
    case OPT_NISP_SERVERS:
    case OPT_NIS_DOMAIN_NAME:
    case OPT_NISP_DOMAIN_NAME:
    case FQ_DOMAIN_NAME:
        break;
}

/**
 * @brief 
 * 
 */
void send_DHCP_SOLICIT(void)
{
    uint16_t j;
    uint8_t ip[16];
    uint8_t rip_msg_size;

    size = 0;
    num = 0;
    growby = 0;

    InitDhcpOption(34, 1);
    DumpDhcpOption("option init");

    AppendDhcpOption(DHCP6_SOLICIT);
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x00FF0000) >> 16));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x00FF0000) >> 8));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x00FF0000) >> 0));
    DumpDhcpOption("Type&XID");

    // Elapsed time
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_ELAPSED_TIME);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x02);
    //    AppendDhcpOption(0x0c);AppendDhcpOption(0x1c);DumpDhcpOption("Option Elapsed Time");

    // Client Identifier
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_CLIENTID);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x0a); //length
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x03); //DUID_Type
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x01); //Hard_Type
    AppendDhcpOption(DHCP_CHADDR[0]);
    AppendDhcpOption(DHCP_CHADDR[1]); // MAC Addr
    AppendDhcpOption(DHCP_CHADDR[2]);
    AppendDhcpOption(DHCP_CHADDR[3]);
    AppendDhcpOption(DHCP_CHADDR[4]);
    AppendDhcpOption(DHCP_CHADDR[5]);
    DumpDhcpOption("Option Client ID");

    // Identity Association for Non-temporary Address
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_IANA);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x0c); // length
    AppendDhcpOption(0x03);
    AppendDhcpOption(0x00); // IAID
    AppendDhcpOption(0x08);
    AppendDhcpOption(0xdc);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x00); // T1
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x00); // T2
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x00);
    DumpDhcpOption("Option IANA");

    // Fully Qualified Domain Name
    //    AppendDhcpOption(0x00);AppendDhcpOption(39);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x06); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x04);
    //    AppendDhcpOption(0x44);AppendDhcpOption(0x45);
    //    AppendDhcpOption(0x44);AppendDhcpOption(0x59);DumpDhcpOption("Option FQ Domain Name");

    // Vendor Class
    //    AppendDhcpOption(0x00);AppendDhcpOption(16);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x0e); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x00);
    //    AppendDhcpOption(0x01);AppendDhcpOption(0x37);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x08);
    //    AppendDhcpOption(0x4d);AppendDhcpOption(0x53);
    //    AppendDhcpOption(0x46);AppendDhcpOption(0x54);
    //    AppendDhcpOption(0x20);AppendDhcpOption(0x35);
    //    AppendDhcpOption(0x2e);AppendDhcpOption(0x30);DumpDhcpOption("Option Vendor Class");

    // Option Request
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_REQUEST);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x08); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_VENDOR_OPTS);
    //    AppendDhcpOption(0x00);AppendDhcpOption(DNS_RecursiveNameServer);
    //    AppendDhcpOption(0x00);AppendDhcpOption(Domain_Search_List);
    //    AppendDhcpOption(0x00);AppendDhcpOption(FQ_DOMAIN_NAME);DumpDhcpOption("Option Request");

    // send broadcasting packet
    ip[0] = 0xff;
    ip[1] = 0x02;

    for (j = 2; j < 13; j++)
        ip[j] = 0x00;

    ip[13] = 0x01;
    ip[14] = 0x00;
    ip[15] = 0x02;

    rip_msg_size = size;

#ifdef _DHCP_DEBUG_
    printf("> Send DHCP_DISCOVER\r\n");
#endif

    sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG.OPT, rip_msg_size, ip, DHCP_SERVER_PORT, 16);

#ifdef _DHCP_DEBUG_
    printf("> %d, %d\r\n", ret, rip_msg_size);
#endif

    //return ret
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t send_DHCP_REQUEST(void)
{
    //uint16_t i;
    uint16_t j;
    uint8_t ip[16];
    uint8_t rip_msg_size;
    uint8_t ret = 1;

    size = 0;
    num = 0;
    growby = 0;

    if (iana_len == 0)
    {
        return 9;
    }
    printf("req : %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x \r\n", recv_IP[0], recv_IP[1], recv_IP[2], recv_IP[3], recv_IP[4], recv_IP[5], recv_IP[6], recv_IP[7], recv_IP[8], recv_IP[9], recv_IP[10], recv_IP[11], recv_IP[12], recv_IP[13], recv_IP[14], recv_IP[15]);

    InitDhcpOption(60, 1);
    DumpDhcpOption("option init");

    AppendDhcpOption(DHCP6_REQUEST);
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x00FF0000) >> 16));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x0000FF00) >> 8));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x000000FF) >> 0));
    DumpDhcpOption("Type&XID");

    // Elapsed time
    //AppendDhcpOption(0x00);AppendDhcpOption(OPT_ELAPSED_TIME);
    //AppendDhcpOption(0x00);AppendDhcpOption(0x02);
    //AppendDhcpOption(0x0c);AppendDhcpOption(0x1c);DumpDhcpOption("Option Elapsed Time");

    // Identity Association for Non-temporary Address
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_IANA);
    AppendDhcpOption((uint8_t)(iana_len >> 8));
    AppendDhcpOption((uint8_t)iana_len); // length
    AppendDhcpOption(IAID[0]);
    AppendDhcpOption(IAID[1]); // IAID
    AppendDhcpOption(IAID[2]);
    AppendDhcpOption(IAID[3]);
    AppendDhcpOption(T1[0]);
    AppendDhcpOption(T1[1]); // T1
    AppendDhcpOption(T1[2]);
    AppendDhcpOption(T1[3]);
    AppendDhcpOption(T2[0]);
    AppendDhcpOption(T2[1]); // T2
    AppendDhcpOption(T2[2]);
    AppendDhcpOption(T2[3]);
    DumpDhcpOption("Option IANA");

    // IA Address
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_IAADDR);
    AppendDhcpOption((uint8_t)(iaaddr_len >> 8));
    AppendDhcpOption((uint8_t)iaaddr_len); // length
    AppendDhcpOption(recv_IP[0]);
    AppendDhcpOption(recv_IP[1]); // IP
    AppendDhcpOption(recv_IP[2]);
    AppendDhcpOption(recv_IP[3]);
    AppendDhcpOption(recv_IP[4]);
    AppendDhcpOption(recv_IP[5]);
    AppendDhcpOption(recv_IP[6]);
    AppendDhcpOption(recv_IP[7]);
    AppendDhcpOption(recv_IP[8]);
    AppendDhcpOption(recv_IP[9]);
    AppendDhcpOption(recv_IP[10]);
    AppendDhcpOption(recv_IP[11]);
    AppendDhcpOption(recv_IP[12]);
    AppendDhcpOption(recv_IP[13]);
    AppendDhcpOption(recv_IP[14]);
    AppendDhcpOption(recv_IP[15]);
    AppendDhcpOption((uint8_t)(PreLifeTime >> 24));
    AppendDhcpOption((uint8_t)(PreLifeTime >> 16));
    AppendDhcpOption((uint8_t)(PreLifeTime >> 8));
    AppendDhcpOption((uint8_t)PreLifeTime);
    AppendDhcpOption((uint8_t)(ValidLifeTime >> 24));
    AppendDhcpOption((uint8_t)(ValidLifeTime >> 16));
    AppendDhcpOption((uint8_t)(ValidLifeTime >> 8));
    AppendDhcpOption((uint8_t)ValidLifeTime);
    DumpDhcpOption("Option IA_addr");

    // Status code
#if 0
    // 20190318
    AppendDhcpOption(0x00);AppendDhcpOption(OPT_STATUS_CODE);DumpDhcpOption("Option status_code type");
    AppendDhcpOption((uint8_t)(Lstatuscode_len>>8));AppendDhcpOption((uint8_t)Lstatuscode_len); DumpDhcpOption("Option status_code length");// length
    AppendDhcpOption((uint8_t)(code>>8));AppendDhcpOption((uint8_t)code); DumpDhcpOption("Option status_code code");// code
#endif
    //    for(i=0; i<(statuscode_len-2); i++)
    //        AppendDhcpOption(status_msg[i]);
    //    DumpDhcpOption("Option status_code msg");

#if 0
    // 20190318
    AppendDhcpOption(0x41);AppendDhcpOption(0x73);
    AppendDhcpOption(0x73);AppendDhcpOption(0x69);
    AppendDhcpOption(0x67);AppendDhcpOption(0x6e);
    AppendDhcpOption(0x65);AppendDhcpOption(0x64);
    AppendDhcpOption(0x20);AppendDhcpOption(0x61);
    AppendDhcpOption(0x6e);AppendDhcpOption(0x20);
    AppendDhcpOption(0x61);AppendDhcpOption(0x64);
    AppendDhcpOption(0x64);AppendDhcpOption(0x72);
    AppendDhcpOption(0x65);AppendDhcpOption(0x73);
    AppendDhcpOption(0x73);AppendDhcpOption(0x2e);
#endif

    // Client Identifier
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_CLIENTID);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x0a); //length
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x03); //DUID_Type
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x01); //Hard_Type
    AppendDhcpOption(DHCP_CHADDR[0]);
    AppendDhcpOption(DHCP_CHADDR[1]); // MAC Addr
    AppendDhcpOption(DHCP_CHADDR[2]);
    AppendDhcpOption(DHCP_CHADDR[3]);
    AppendDhcpOption(DHCP_CHADDR[4]);
    AppendDhcpOption(DHCP_CHADDR[5]);
    DumpDhcpOption("Option Client ID");
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_CLIENTID);
    //    AppendDhcpOption((uint8_t)(clientid_len>>8));AppendDhcpOption((uint8_t)clientid_len); //length
    //    AppendDhcpOption((uint8_t)(DUID_type>>8));AppendDhcpOption((uint8_t)DUID_type); //DUID_Type
    //    AppendDhcpOption((uint8_t)(Hardware_type>>8));AppendDhcpOption((uint8_t)Hardware_type); //Hard_Type
    //    AppendDhcpOption(DHCP_CHADDR[0]);AppendDhcpOption(DHCP_CHADDR[1]); // MAC Addr
    //    AppendDhcpOption(DHCP_CHADDR[2]);AppendDhcpOption(DHCP_CHADDR[3]);
    //    AppendDhcpOption(DHCP_CHADDR[4]);AppendDhcpOption(DHCP_CHADDR[5]);DumpDhcpOption("Option Client ID");

    //Server Identifier
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_SERVERID);
    AppendDhcpOption((uint8_t)(serverid_len >> 8));
    AppendDhcpOption((uint8_t)serverid_len); //length
    AppendDhcpOption((uint8_t)(DUID_type_s >> 8));
    AppendDhcpOption((uint8_t)DUID_type_s); //DUID_Type
    AppendDhcpOption((uint8_t)(Hardware_type_s >> 8));
    AppendDhcpOption((uint8_t)Hardware_type_s); //Hard_Type
#if 0
    // 20190318
    AppendDhcpOption(Time_s[0]);AppendDhcpOption(Time_s[1]); // Time
    AppendDhcpOption(Time_s[2]);AppendDhcpOption(Time_s[3]);
#endif
    AppendDhcpOption(Server_MAC[0]);
    AppendDhcpOption(Server_MAC[1]); // MAC Addr
    AppendDhcpOption(Server_MAC[2]);
    AppendDhcpOption(Server_MAC[3]);
    AppendDhcpOption(Server_MAC[4]);
    AppendDhcpOption(Server_MAC[5]);
    DumpDhcpOption("Option Server ID");

    // Fully Qualified Domain Name
    //    AppendDhcpOption(0x00);AppendDhcpOption(39);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x06); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x04);
    //    AppendDhcpOption(0x44);AppendDhcpOption(0x45);
    //    AppendDhcpOption(0x44);AppendDhcpOption(0x59);DumpDhcpOption("Option FQ Domain Name");

    // Vendor Class
    //    AppendDhcpOption(0x00);AppendDhcpOption(16);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x0e); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x00);
    //    AppendDhcpOption(0x01);AppendDhcpOption(0x37);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x08);
    //    AppendDhcpOption(0x4d);AppendDhcpOption(0x53);
    //    AppendDhcpOption(0x46);AppendDhcpOption(0x54);
    //    AppendDhcpOption(0x20);AppendDhcpOption(0x35);
    //    AppendDhcpOption(0x2e);AppendDhcpOption(0x30);DumpDhcpOption("Option Vendor Class");

    // Option Request
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_REQUEST);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x08); // length
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_VENDOR_OPTS);
    //    AppendDhcpOption(0x00);AppendDhcpOption(DNS_RecursiveNameServer);
    //    AppendDhcpOption(0x00);AppendDhcpOption(Domain_Search_List);
    //    AppendDhcpOption(0x00);AppendDhcpOption(FQ_DOMAIN_NAME);DumpDhcpOption("Option Request");

    // send broadcasting packet
    ip[0] = 0xff;
    ip[1] = 0x02;

    for (j = 2; j < 13; j++)
        ip[j] = 0x00;

    ip[13] = 0x01;
    ip[14] = 0x00;
    ip[15] = 0x02;

    rip_msg_size = size;

#ifdef _DHCP_DEBUG_
    printf("> Send DHCP_REQUEST\r\n");
#endif

    sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG.OPT, rip_msg_size, ip, DHCP_SERVER_PORT, 16);
#ifdef _DHCP_DEBUG_
    printf("> %d, %d\r\n", ret, rip_msg_size);
#endif

    return ret;
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t send_DHCP_INFOREQ(void)
{
    //uint16_t i;
    uint16_t j;
    uint8_t ip[16];
    uint8_t rip_msg_size;
    uint8_t ret = 0;

    size = 0;
    num = 0;
    growby = 0;

    //printf("req : %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x \r\n",recv_IP[0],recv_IP[1],recv_IP[2],recv_IP[3],recv_IP[4],recv_IP[5],recv_IP[6],recv_IP[7],recv_IP[8],recv_IP[9],recv_IP[10],recv_IP[11],recv_IP[12],recv_IP[13],recv_IP[14],recv_IP[15]);
    InitDhcpOption(30, 1);
    DumpDhcpOption("option init");

    AppendDhcpOption(DHCP6_INFO_REQUEST);
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x00FF0000) >> 16));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x0000FF00) >> 8));
    AppendDhcpOption((uint8_t)((DHCP_XID & 0x000000FF) >> 0));
    DumpDhcpOption("Type&XID");

    //    // Elapsed time
    //    AppendDhcpOption(0x00);AppendDhcpOption(OPT_ELAPSED_TIME);
    //    AppendDhcpOption(0x00);AppendDhcpOption(0x02);
    //    AppendDhcpOption(0x0c);AppendDhcpOption(0x1c);DumpDhcpOption("Option Elapsed Time");

    // Client Identifier
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_CLIENTID);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x0a); //length
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x03); //DUID_Type
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x01); //Hard_Type
    AppendDhcpOption(DHCP_CHADDR[0]);
    AppendDhcpOption(DHCP_CHADDR[1]); // MAC Addr
    AppendDhcpOption(DHCP_CHADDR[2]);
    AppendDhcpOption(DHCP_CHADDR[3]);
    AppendDhcpOption(DHCP_CHADDR[4]);
    AppendDhcpOption(DHCP_CHADDR[5]);
    DumpDhcpOption("Option Client ID");

    // Option Request
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_REQUEST);
    AppendDhcpOption(0x00);
    AppendDhcpOption(0x06); // length
    //AppendDhcpOption(0x00);AppendDhcpOption(OPT_VENDOR_OPTS);
    AppendDhcpOption(0x00);
    AppendDhcpOption(DNS_RecursiveNameServer);
    AppendDhcpOption(0x00);
    AppendDhcpOption(Domain_Search_List);
    AppendDhcpOption(0x00);
    AppendDhcpOption(OPT_LIFETIME);
    DumpDhcpOption("Option Request");

    // send broadcasting packet
    ip[0] = 0xff;
    ip[1] = 0x02;

    for (j = 2; j < 13; j++)
        ip[j] = 0x00;

    ip[13] = 0x01;
    ip[14] = 0x00;
    ip[15] = 0x02;

    rip_msg_size = size;

#ifdef _DHCP_DEBUG_
    printf("> Send DHCP_REQUEST\r\n");
#endif

    sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG.OPT, rip_msg_size, ip, DHCP_SERVER_PORT, 16);

#ifdef _DHCP_DEBUG_
    printf("> %d, %d\r\n", ret, rip_msg_size);
#endif

    return ret;
}

/**
 * @brief 
 * 
 * @return int8_t 
 */
int8_t parseDHCPMSG(void)
{
    uint8_t svr_addr[16];
    uint16_t svr_port;
    uint8_t addlen;
    uint16_t len;
    uint8_t *p;
    uint8_t *e;
    uint8_t type, i;
    uint16_t opt_len;
    uint32_t end_point;

    if ((len = getSn_RX_RSR(DHCP_SOCKET)) > 0)
    {
        len = recvfrom(DHCP_SOCKET, (uint8_t *)pDHCPMSG.OPT, len, svr_addr, (uint16_t *)&svr_port, &addlen);
#ifdef _DHCP6_DEBUG_
        printf("DHCP message : %.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x(%d) %d received. \r\n", svr_addr[0], svr_addr[1], svr_addr[2], svr_addr[3], svr_addr[4], svr_addr[5], svr_addr[6], svr_addr[7], svr_addr[8], svr_addr[9], svr_addr[10], svr_addr[11], svr_addr[12], svr_addr[13], svr_addr[14], svr_addr[15], svr_port, len);
#endif
    }
    else
    {
        return 0;
    }

    type = 0;
    p = (uint8_t *)(pDHCPMSG.OPT);
    e = p + len;
#ifdef _DHCP6_DEBUG_
    printf("in server port %x\r\n", *p);
#endif
    i = 0;
    /*
        while(p<e){
        	printf("%.2x ",*p);
        	i8f(i%16 == 1){
        		printf("\r\n");
        	}
        	p++;i++;
        }
        i=0;*/

    switch (*p)
    {
    case DHCP6_ADVERTISE:
    case DHCP6_REPLY:
    {
#ifdef _DHCP6_DEBUG_
        printf("in ADVER or REPLY(7) type : %x \r\n", *p);
#endif
        type = *p++; // type
        p++;         // xid[0]
        p++;         // xid[1]
        p++;         // xid[2]
        while (p < e)
        {
            p++;
            switch (*p)
            {
            case OPT_CLIENTID:
            {
#ifdef _DHCP6_DEBUG_
                printf("in clientid \r\n");
#endif
                p++;

                opt_len = (*p++ << 8);
                clientid_len = opt_len + (*p++);
#ifdef _DHCP6_DEBUG_
                printf("opt_len : %.4x\r\n", clientid_len);
#endif
                end_point = (uint32_t)p + clientid_len;

                while ((uint32_t)p != end_point)
                {
                    p++;
                }
                break;
            }

            case OPT_IANA:
            {
#ifdef _DHCP6_DEBUG_
                printf("in iana \r\n");
#endif
                p++;
                opt_len = (*p++ << 8);
                iana_len = opt_len + (*p++);
                end_point = (uint32_t)p + iana_len;

                //IAID
                IAID[0] = *p++;
                IAID[1] = *p++;
                IAID[2] = *p++;
                IAID[3] = *p++;
                //T1
                T1[0] = *p++;
                T1[1] = *p++;
                T1[2] = *p++;
                T1[3] = *p++;
                //T2
                T2[0] = *p++;
                T2[1] = *p++;
                T2[2] = *p++;
                T2[3] = *p++;
                //IA_NA-options
                while ((uint32_t)p < end_point)
                {
                    p++;
                    switch (*p)
                    {
                    case OPT_IAADDR:
                    {
#ifdef _DHCP6_DEBUG_
                        printf("in IA addr \r\n");
#endif
                        p++;
                        opt_len = (*p++ << 8);
                        iaaddr_len = opt_len + (*p++);
                        recv_IP[0] = *p++;
                        recv_IP[1] = *p++;
                        recv_IP[2] = *p++;
                        recv_IP[3] = *p++;
                        recv_IP[4] = *p++;
                        recv_IP[5] = *p++;
                        recv_IP[6] = *p++;
                        recv_IP[7] = *p++;
                        recv_IP[8] = *p++;
                        recv_IP[9] = *p++;
                        recv_IP[10] = *p++;
                        recv_IP[11] = *p++;
                        recv_IP[12] = *p++;
                        recv_IP[13] = *p++;
                        recv_IP[14] = *p++;
                        recv_IP[15] = *p++;

                        PreLifeTime = (*p++ << 24);
                        PreLifeTime += (*p++ << 16);
                        PreLifeTime += (*p++ << 8);
                        PreLifeTime += (*p++);
                        ValidLifeTime = (*p++ << 24);
                        ValidLifeTime += (*p++ << 16);
                        ValidLifeTime += (*p++ << 8);
                        ValidLifeTime += (*p++);

                        printf("IANA : %.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x \r\n", recv_IP[0], recv_IP[1], recv_IP[2], recv_IP[3], recv_IP[4], recv_IP[5], recv_IP[6], recv_IP[7], recv_IP[8], recv_IP[9], recv_IP[10], recv_IP[11], recv_IP[12], recv_IP[13], recv_IP[14], recv_IP[15]);
                        break;
                    }

                    case OPT_STATUS_CODE:
                    {
#ifdef _DHCP6_DEBUG_
                        printf("in status code \r\n");
#endif
                        p++;
                        opt_len = (*p++ << 8);
                        statuscode_len = opt_len + (*p++);
                        Lstatuscode_len = statuscode_len;
#ifdef _DHCP6_DEBUG_
                        printf("status_len : %x\r\n", statuscode_len);
                        printf("Lstatus_len : %x\r\n", Lstatuscode_len);
#endif
                        code = (*p++ << 8);
                        code = code + (*p++);
                        //printf("status_code : %x\r\n", code);
                        //for(i=0; i<(statuscode_len-2); i++)
                        //    status_msg[i] = *p++;
                        //printf("status_msg : %s \r\n", status_msg);

                        p += statuscode_len - 2;
                        break;
                    }

                    default:
                    {
#ifdef _DHCP6_DEBUG_
                        printf("in default \r\n");
#endif
                        p++;
                        opt_len = (*p++ << 8);
                        opt_len = opt_len + (*p++);
                        p += opt_len;
                        break;
                    }
                    }
                }
                break;
            }

            case OPT_IATA:
            {
#ifdef _DHCP6_DEBUG_
                printf("in iata \r\n");
#endif
                p++;
                opt_len = (*p++ << 8);
                opt_len = opt_len + (*p++);
                //IA_TA-options
                p += opt_len;
                break;
            }

            case OPT_SERVERID:
            {
#ifdef _DHCP6_DEBUG_
                printf("in serverid \r\n");
#endif

                p++;
                opt_len = (*p++ << 8);
                serverid_len = opt_len + (*p++);
#ifdef _DHCP6_DEBUG_
                printf("opt_len : %.4x\r\n", serverid_len);
#endif
                end_point = (uint32_t)p + serverid_len;

                DUID_type_s = (*p++ << 8);
                DUID_type_s = DUID_type_s + (*p++);
#ifdef _DHCP6_DEBUG_
                printf("DUID_type : %.4x\r\n", DUID_type_s);
#endif
                if (DUID_type_s == 0x02)
                {
                    Enterprise_num_s = (*p++ << 24);
                    Enterprise_num_s = Enterprise_num_s + (*p++ << 16);
                    Enterprise_num_s = Enterprise_num_s + (*p++ << 8);
                    Enterprise_num_s = Enterprise_num_s + (*p++);
                }
                else
                {
                    Hardware_type_s = (*p++ << 8);
                    Hardware_type_s = Hardware_type_s + (*p++);
#ifdef _DHCP6_DEBUG_
                    printf("Hardware_type : %.4x\r\n", Hardware_type_s);
#endif
                }

                if (DUID_type_s == 0x01)
                {
                    Time_s[0] = *p++;
                    Time_s[1] = *p++;
                    Time_s[2] = *p++;
                    Time_s[3] = *p++;
#ifdef _DHCP6_DEBUG_
                    printf("Time : ");
                    for (i = 0; i < 4; i++)
                        printf("%.2x", Time_s[i]);
                    printf("\r\n");
#endif
                }

                Server_MAC[0] = *p++;
                Server_MAC[1] = *p++;
                Server_MAC[2] = *p++;
                Server_MAC[3] = *p++;
                Server_MAC[4] = *p++;
                Server_MAC[5] = *p++;

#ifdef _DHCP6_DEBUG_
                printf("Server_MAC : ");
                for (i = 0; i < 6; i++)
                    printf("%.2x", Server_MAC[i]);
                printf("\r\n");
#endif
                while ((uint32_t)p != end_point)
                {
                    p++;
                }
                break;
            }

            case DNS_RecursiveNameServer:
            {
#ifdef _DHCP6_DEBUG_
                printf("in DNS Recursive Name Server \r\n");
#endif
                p++;
                opt_len = (*p++ << 8);
                opt_len = opt_len + (*p++);
                end_point = (uint32_t)p + opt_len;

                DNS6_Address[0] = *p++;
                DNS6_Address[1] = *p++;
                DNS6_Address[2] = *p++;
                DNS6_Address[3] = *p++;
                DNS6_Address[4] = *p++;
                DNS6_Address[5] = *p++;
                DNS6_Address[6] = *p++;
                DNS6_Address[7] = *p++;
                DNS6_Address[8] = *p++;
                DNS6_Address[9] = *p++;
                DNS6_Address[10] = *p++;
                DNS6_Address[11] = *p++;
                DNS6_Address[12] = *p++;
                DNS6_Address[13] = *p++;
                DNS6_Address[14] = *p++;
                DNS6_Address[15] = *p++;

                while ((uint32_t)p < end_point)
                {
                    p++;
                }

                break;
            }

            case Domain_Search_List:
            {
#ifdef _DHCP6_DEBUG_
                printf("in Domain Search List \r\n");
#endif
                p++;
                opt_len = (*p++ << 8);
                opt_len = opt_len + (*p++);
                end_point = (uint32_t)p + opt_len;

                while ((uint32_t)p < end_point)
                {
                    p++;
                }

                break;
            }

            default:
            {
#ifdef _DHCP6_DEBUG_
                printf("in default \r\n");
#endif
                p++;
                opt_len = (*p++ << 8);
                opt_len = opt_len + (*p++);
                p += opt_len;
                break;
            }
            }
        }
    }
    }
    return type;
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t DHCP_run2(void)
{
    uint8_t type;
    uint8_t ret;

    if (dhcp_state == STATE_DHCP6_STOP)
    {
        return DHCP_STOPPED; // Check DHCP6 STOP State
    }

    if (getSn_SR(DHCP_SOCKET) != SOCK_UDP)
    {                                                // Check DHCP SOCKET == UDP
        WIZCHIP_WRITE(_Sn_TTLR_(DHCP_SOCKET), 0x01); // hop limit 1
        socket(DHCP_SOCKET, (Sn_MR_UDP6), DHCP_CLIENT_PORT, 0x00);
    }
    ret = DHCP_RUNNING;
    type = parseDHCPMSG();
    switch (dhcp_state)
    {
    case STATE_DHCP6_INIT:
        send_DHCP_INFOREQ();
        dhcp_state = STATE_DHCP6_RELEASE;
        break;
    case STATE_DHCP6_RELEASE:
        return DHCP_IP_LEASED;
    default:
        break;
    }

    return ret;
}

/**
 * @brief 
 * 
 * @param netinfo 
 * @return uint8_t 
 */
uint8_t DHCP_run(wiz_NetInfo *netinfo)
{
    uint8_t type;
    uint8_t ret;
    uint8_t i;

    if (dhcp_state == STATE_DHCP6_STOP)
        return DHCP_STOPPED; // Check DHCP6 STOP State

    if (getSn_SR(DHCP_SOCKET) != SOCK_UDP)
    {                                                // Check DHCP SOCKET == UDP
        WIZCHIP_WRITE(_Sn_TTLR_(DHCP_SOCKET), 0x01); // hop limit 1
        socket(DHCP_SOCKET, (Sn_MR_UDP6), DHCP_CLIENT_PORT, 0x00);
    }

    ret = DHCP_RUNNING;
    type = parseDHCPMSG();
    printf("type:%d, dhcp_state :%d\r\n", type, dhcp_state);
    switch (dhcp_state)
    {
    case STATE_DHCP6_INIT:
        DHCP_allocated_ip[0] = 0;
        DHCP_allocated_ip[1] = 0;
        DHCP_allocated_ip[2] = 0;
        DHCP_allocated_ip[3] = 0;
        DHCP_allocated_ip[4] = 0;
        DHCP_allocated_ip[5] = 0;
        DHCP_allocated_ip[6] = 0;
        DHCP_allocated_ip[7] = 0;
        DHCP_allocated_ip[8] = 0;
        DHCP_allocated_ip[9] = 0;
        DHCP_allocated_ip[10] = 0;
        DHCP_allocated_ip[11] = 0;
        DHCP_allocated_ip[12] = 0;
        DHCP_allocated_ip[13] = 0;
        DHCP_allocated_ip[14] = 0;
        DHCP_allocated_ip[15] = 0;

        send_DHCP_SOLICIT();
        dhcp_state = STATE_DHCP6_SOLICIT;
        break;
    case STATE_DHCP6_SOLICIT:
        if (type == DHCP6_ADVERTISE)
        {
#ifdef _DHCP6_DEBUG_
            printf("> Receive DHCP_ADVERTISE\r\n");
#endif

            DHCP_allocated_ip[0] = recv_IP[0];
            DHCP_allocated_ip[1] = recv_IP[1];
            DHCP_allocated_ip[2] = recv_IP[2];
            DHCP_allocated_ip[3] = recv_IP[3];
            DHCP_allocated_ip[4] = recv_IP[4];
            DHCP_allocated_ip[5] = recv_IP[5];
            DHCP_allocated_ip[6] = recv_IP[6];
            DHCP_allocated_ip[7] = recv_IP[7];
            DHCP_allocated_ip[8] = recv_IP[8];
            DHCP_allocated_ip[9] = recv_IP[9];
            DHCP_allocated_ip[10] = recv_IP[10];
            DHCP_allocated_ip[11] = recv_IP[11];
            DHCP_allocated_ip[12] = recv_IP[12];
            DHCP_allocated_ip[13] = recv_IP[13];
            DHCP_allocated_ip[14] = recv_IP[14];
            DHCP_allocated_ip[15] = recv_IP[15];

            ret = send_DHCP_REQUEST();
            if (ret == 9)
            {
                return 0;
            }
            dhcp_state = STATE_DHCP6_REQUEST;
        } //else ret = check_DHCP_timeout();
        break;
    case STATE_DHCP6_REQUEST:
        // 20190318
        //NETUNLOCK();

        memcpy(netinfo->gua, recv_IP, 16);

        //NETLOCK();
        return DHCP_IP_LEASED;
    default:
        break;
    }
    return ret;
}

/**
 * @brief 
 * 
 */
void DHCP_stop(void)
{
    close(DHCP_SOCKET);
    dhcp_state = STATE_DHCP6_STOP;
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t check_DHCP_timeout(void)
{
    uint8_t ret = DHCP_RUNNING;

    if (dhcp_retry_count < MAX_DHCP_RETRY)
    {
        if (dhcp_tick_next < dhcp_tick_1s)
        {
            switch (dhcp_state)
            {
            case STATE_DHCP6_SOLICIT:
            {
#ifdef _DHCP6_DEBUG_
                printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
#endif
                send_DHCP_SOLICIT();
                break;
            }
            default:
            {
                break;
            }
            }

            dhcp_tick_1s = 0;
            dhcp_tick_next = dhcp_tick_1s + DHCP_WAIT_TIME;
            dhcp_retry_count++;
        }
    }
    else
    { // timeout occurred

        switch (dhcp_state)
        {
        case STATE_DHCP6_SOLICIT:
            dhcp_state = STATE_DHCP6_INIT;
            ret = DHCP_FAILED;
            break;
        default:
            break;
        }
        reset_DHCP_timeout();
    }
    return ret;
}

/**
 * @brief 
 * 
 * @param s 
 * @param buf 
 */
void DHCP_init(uint8_t s, uint8_t *buf)
{
    uint8_t zeroip[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    getSHAR(DHCP_CHADDR);
    if ((DHCP_CHADDR[0] | DHCP_CHADDR[1] | DHCP_CHADDR[2] | DHCP_CHADDR[3] | DHCP_CHADDR[4] | DHCP_CHADDR[5]) == 0x00)
    {
#ifdef _DHCP6_DEBUG_
        printf("DHCP_init-set MAC\r\n");
#endif
        // assing temporary mac address, you should be set SHAR before call this function.
        DHCP_CHADDR[0] = 0x00;
        DHCP_CHADDR[1] = 0x08;
        DHCP_CHADDR[2] = 0xdc;
        DHCP_CHADDR[3] = 0x00;
        DHCP_CHADDR[4] = 0x00;
        DHCP_CHADDR[5] = 0x00;
        setSHAR(DHCP_CHADDR);
    }

    DHCP_SOCKET = s; // SOCK_DHCP

    memset(buf, 0, sizeof(buf));
    pDHCPMSG.OPT = buf;
    DHCP_XID = 0x515789;

    reset_DHCP_timeout();
    dhcp_state = STATE_DHCP6_INIT;
}

/**
 * @brief 
 * 
 */
void reset_DHCP_timeout(void)
{
    dhcp_tick_1s = 0;
    dhcp_tick_next = DHCP_WAIT_TIME;
    dhcp_retry_count = 0;
}

/**
 * @brief 
 * 
 */
void DHCP_time_handler(void)
{
    dhcp_tick_1s++;
}
