//*****************************************************************************
//
//! \file dhcp6.h
//! \brief DHCPv6 APIs Header file.
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
#ifndef _DHCP6_H_
#define _DHCP6_H_
#include <stdint.h>
#include "W6100.h"
#include "socket.h"
/*
 * @brief 
 * @details If you want to display debug & procssing message, Define _DHCP_DEBUG_ 
 * @note    If defined, it dependens on <stdio.h>
 */
//#define _DHCP_DEBUG_

/* Retry to processing DHCP */
#define MAX_DHCP_RETRY 2  ///< Maxium retry count
#define DHCP_WAIT_TIME 10 ///< Wait Time 10s

/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT 547 ///< DHCP server port number
#define DHCP_CLIENT_PORT 546 ///< DHCP client port number

#define DCHP_HOST_NAME "WIZnet\0"

/* 
 * @brief return value of @ref DHCP_run()
 */
enum
{
   DHCP_FAILED = 0, ///< Procssing Fail
   DHCP_RUNNING,    ///< Procssing DHCP proctocol
   DHCP_IP_ASSIGN,  ///< First Occupy IP from DHPC server      (if cbfunc == null, act as default default_ip_assign)
   DHCP_IP_CHANGED, ///< Change IP address by new ip from DHCP (if cbfunc == null, act as default default_ip_update)
   DHCP_IP_LEASED,  ///< Stand by
   DHCP_STOPPED     ///< Stop procssing DHCP protocol
};

/**
 * @brief 
 * 
 * @param asize 
 * @param agrowby 
 */
void InitDhcpOption(unsigned asize, unsigned agrowby);
/**
 * @brief 
 * 
 * @param value 
 */
void AppendDhcpOption(uint8_t value);
/**
 * @brief 
 * 
 * @param sMark 
 */
void DumpDhcpOption(char *sMark);
/**
 * @brief 
 * 
 * @param Option 
 */
void DHCP_Option_Select(uint8_t Option);
/*
 * @brief DHCP client initialization (outside of the main loop)
 * @param s   - socket number
 * @param buf - buffer for procssing DHCP message
 */
void DHCP_init(uint8_t s, uint8_t *buf);

/*
 * @brief DHCP 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler 
 */
void DHCP_time_handler(void);

/**
 * @brief 
 * 
 * @param netinfo 
 * @return uint8_t 
 */
uint8_t DHCP_run(wiz_NetInfo *netinfo);

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t DHCP_run2(void);

/*
 * @brief Stop DHCP procssing
 * @note If you want to restart. call DHCP_init() and DHCP_run()
 */
void DHCP_stop(void);

#endif /* _DHCP_H_ */
