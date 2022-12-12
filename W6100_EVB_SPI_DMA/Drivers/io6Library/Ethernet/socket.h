//* ****************************************************************************
//! \file socket.h
//! \brief SOCKET APIs Header File.
//! \version 1.0.0
//! \date 2019/01/01
//! \par  Revision history
//!       <2019/01/01> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2019, WIZnet Co., LTD.
//!
//! Permission is hereby granted, free of charge, to any person obtaining a copy
//! of this software and associated documentation files (the "Software"), to deal
//! in the Software without restriction, including without limitation the rights 
//! to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//! copies of the Software, and to permit persons to whom the Software is 
//! furnished to do so, subject to the following conditions: 
//!
//! The above copyright notice and this permission notice shall be included in
//! all copies or substantial portions of the Software. 
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//! OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//! SOFTWARE. 
//!
//*****************************************************************************

/**
 * @defgroup WIZnet_socket_APIs 1. WIZnet socket APIs
 * @brief WIZnet socket APIs are based on Berkeley socket APIs,  thus it has much similar name and interface.
 *        But there is a little bit of difference.
 * @details
 * <b> Comparison between WIZnet and Berkeley SOCKET APIs </b>
 * <table>
 *    <tr> <td><b>API       </b></td> <td><b>WIZnet</b></td> <td><b>Berkeley</b></td> </tr>
 *    <tr> <td><b>socket()  </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>bind()    </b></td> <td>X            </td> <td>O              </td> </tr>
 *    <tr> <td><b>listen()  </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>connect() </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>accept()  </b></td> <td>X            </td> <td>O              </td> </tr>
 *    <tr> <td><b>recv()    </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>send()    </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>recvfrom()</b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>sendto()  </b></td> <td>O            </td> <td>O              </td> </tr>
 *    <tr> <td><b>closesocket()</b></td> <td>O<br>close() & disconnect()</td> <td>O</td> </tr>
 * </table>
 * There are <b>bind()</b> and <b>accept()</b> functions in <b>Berkeley SOCKET API</b> but, not in <b>WIZnet SOCKET API</b>. \n
 * Because socket() of WIZnet is not only creating a SOCKET but also binding a local port number, \n
 * and listen() of WIZnet is not only listening to connection request from client but also accepting the connection request. \n
 * When you program "TCP SERVER" with Berkeley SOCKET API, you can use only one listen port. \n
 * When the listen SOCKET accepts a connection request from a client, it keeps listening. \n
 * After accepting the connection request, a new SOCKET is created and the new SOCKET is used in communication with the client. \n
 * Following figure shows network flow diagram by Berkeley SOCKET API. \n
 *  <table width=0 >
 *  <tr> <td>@image html Berkeley_SOCKET.jpg "<Berkeley SOCKET API>"</td> </tr>
 *  </table>
 * But, When you program "TCP SERVER" with WIZnet SOCKET API, you can use as many as 8 listen SOCKET with same port number. \n
 * Because there's no accept() in WIZnet SOCKET APIs, when the listen SOCKET accepts a connection request from a client, \n
 * it is changed in order to communicate with the client.\n
 * And the changed SOCKET is not listening any more and is dedicated for communicating with the client. \n
 * If there're many listen SOCKET with same listen port number and a client requests a connection, \n
 * the SOCKET which has the smallest SOCKET number accepts the request and is changed as communication SOCKET. \n
 * Following figure shows network flow diagram by WIZnet SOCKET API.
 *  <table width=0 >
 *  <tr> <td>@image html WIZnet_SOCKET.jpg "<WIZnet SOCKET API>"</td> </tr>
 *  </table>
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "stdint.h"
#include "wizchip_conf.h"

#define SOCKET               uint8_t  ///< SOCKET type define for legacy driver

#define SOCK_OK              1        ///< Result is OK about socket process.
#define SOCK_BUSY            0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL           (-1000)    ///< Result is fatal error about socket process.

#define SOCK_ERROR           0        
#define SOCKERR_SOCKNUM      (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT      (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT     (SOCK_ERROR - 3)     ///< Socket is not initialized or SIPR is Zero IP address when Sn_MR_TCP
#define SOCKERR_SOCKCLOSED   (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE     (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG     (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS   (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG          (SOCK_ERROR - 10)    ///< Invalid argument.
#define SOCKERR_PORTZERO     (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID    (SOCK_ERROR - 12)    ///< Invalid source or destination IP address
#define SOCKERR_TIMEOUT      (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN      (SOCK_ERROR - 14)    ///< Invalid data length
#define SOCKERR_BUFFER       (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.
//#define SOCKERR_IPvMISMATCH   (SOCK_ERROR - 16)    ///< Socket IP version invalid
//#define SOCKERR_IPLENINVALID  (SOCK_ERROR - 17)    ///< Socket IP version invalid

#define SOCKFATAL_PACKLEN    (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.

/*
 * - @ref Sn_MR_MULTI : Support UDP Multicasting
 * - @ref Sn_MR_MF    : Support MAC Filter Enable
 * - @ref Sn_MR_BRDB  : Broadcast Block
 * - @ref Sn_MR_FPSH  : Force PSH flag
 * - @ref Sn_MR_ND    : No Delay ACK flag
 * - @ref Sn_MR_MC    : IGMP ver2, ver1
 * - @ref Sn_MR_SMB   : Solicited Multicast Block
 * - @ref Sn_MR_MMB   : IPv4 Multicast block
 * - @ref Sn_MR_UNIB  : Unicast Block
 * - @ref Sn_MR_MMB6  : IPv6 UDP Multicast Block </b>

 * - @ref Sn_MR2_DHAM : @ref Sn_MR2_DHAM_AUTO, @ref Sn_MR2_DHAM_MANUAL
 * - @ref Sn_MR_FARP   
*/

/*
 * SOCKET FLAG
 */
/**
 * @brief In UDP mode such as @ref Sn_MR_UDP4 and @ref Sn_MR_UDP6, @ref Sn_MR_UDP6, Enable multicast mode. When @ref Sn_MR_UDP6, Enable only IPv6 Multicating.
 */
#define SF_MULTI_ENABLE      (Sn_MR_MULTI)    
#define SF_ETHER_OWN         (Sn_MR_MF)       ///< In MACRAW mode such as @ref Sn_MR_MACRAW, Receive only the packet as broadcast, multicast and own packet

/**
 * @brief In UDP mode such as @ref Sn_MR_UDP4, @ref Sn_MR_UDP6 and @ref Sn_MR_UDPD, or In MACRAW mode sucha as @ref Sn_MR_MACRAW, Block a broadcast packet. 
 */
#define SF_BROAD_BLOCK       (Sn_MR_BRDB)       
#define SF_TCP_FPSH          (Sn_MR_FPSH)       ///< In TCP mode such as @ref Sn_MR_TCP4, @ref Sn_MR_TCP6 and @ref Sn_MR_TCPD, Use to forced push flag.

#define SF_TCP_NODELAY       (Sn_MR_ND)       ///< In TCP mode such as @ref Sn_MR_TCP4, @ref Sn_MR_TCP6 and @ref Sn_MR_TCPD, Use to nodelayed ack.
#define SF_IGMP_VER2         (Sn_MR_MC)       ///< In UDP mode such as @ref Sn_MR_UDP4 with @ref SF_MULTI_ENABLE, Select IGMP version 2.   
#define SF_SOLICIT_BLOCK     (Sn_MR_SMB)      ///< In UDP mode such as @ref Sn_MR_UDP6 and @ref Sn_MR_UDPD, Block a solicited mutlicast packet.
#define SF_ETHER_MULTI4B     (Sn_MR_MMB4)     ///< In MACRAW mode such as @ref Sn_MR_MACRAW with @ref SF_MULTI_ENABLE, Block a IPv4 multicast packet. 

#define SF_UNI_BLOCK         (Sn_MR_UNIB)     ///< In UDP mdoe such as @ref Sn_MR_UDP4, @ref Sn_MR_UDP6 and @ref Sn_MR_UDPD with @ref SF_MULTI_ENABLE, Block a unicast packet. 
#define SF_ETHER_MULIT6B     (Sn_MR_MMB6)     ///< In MACRAW mode such as @ref Sn_MR_MACRAW with @ref SF_MULTI_ENABLE, Block a IPv6 multicast packet. 

/**
 * @brief Force to APR.
 * @details In datagram mode such as @ref Sn_MR_IPRAW4, @ref Sn_MR_IPRAW6, @ref Sn_MR_UDP4, @ref Sn_MR_UDP6, and @ref Sn_MR_UDPD,
 *          Force to request ARP before a packet is sent to a destination.\n
 *          In TCP mode such as @ref Sn_MR_TCP4, @ref Sn_MR_TCP6, and @ref Sn_MR_TCPD and <b>TCP SERVER</b> operation mode, 
 *          Force to request ARP before SYN/ACK packet is sent to a <b>TCP CLIENT</b>. \n
 *          When @ref SF_DHA_MANUAL is set, the ARP is process but the destination hardware address is fixed by user.
 */
#define SF_FORCE_ARP         (Sn_MR2_FARP)

/**
 * @brief The destination hardware address of packet to be transmitted is set by user through @ref _Sn_DHAR_. It is invalid in MACRAW mode such as @ref Sn_MR_MACRAW.
 */
#define SF_DHA_MANUAL        (Sn_MR2_DHAM)

#define SF_IO_NONBLOCK       (0x01 << 3)     ///< Socket nonblock io mode. It used parameter in @ref socket().

/*
 * UDP, IPRAW, MACRAW Packet Infomation
 */
#define PACK_IPv6            (1<<7)                ///< It indicates the destination IP address of the received packet is IPv6 or IPv4.
#define PACK_IPV6_ALLNODE    (PACK_IPv6 | (1<<6))  ///< It indicates the destination IP address of the received packet is allnode multicast(broadcast) address or not.
#define PACK_IPV6_MULTI      (PACK_IPv6 | (1<<5))  ///< It indicates the destination IP address of the received packet is multicast address or not.
#define PACK_IPV6_LLA        (PACK_IPv6 | (1<<4))  ///< It indicates the destination IP address of the received packet is lla or gua.
#define PACK_COMPLETED       (1<<3)                ///< It indicates the read data is last in the received packet.
#define PACK_REMAINED        (1<<2)                ///< It indicates to remain data in the received packet
#define PACK_FIRST           (1<<1)                ///< It indicates the read data is first in the received packet.
#define PACK_NONE            (0x00)                ///< It indicates no information of a packet

#define SRCV6_PREFER_AUTO    (PSR_AUTO)            ///< Soruce IPv6 address is preferred to auto-selection. Refer to @ref _Sn_PSR_
#define SRCV6_PREFER_LLA     (PSR_LLA)             ///< Soruce IPv6 address is preferred to link local address. Refer to @ref _Sn_PSR_
#define SRCV6_PREFER_GUA     (PSR_GUA)             ///< Soruce IPv6 address is preferred to global unique address. Refer to @ref _Sn_PSR_

#define TCPSOCK_MODE         (Sn_ESR_TCPM)         ///< It indicates the IP version when SOCKETn is opened as TCP6 or TCPD mode.(0 - IPv4 , 1 - IPv6)
#define TCPSOCK_OP           (Sn_ESR_TCPOP)        ///< It indicates the operation mode when SOCKETn is connected.(0 - <b>TCP CLIENT</b> , 1 - <b>TCP SERVER</b>)
#define TCPSOCK_SIP          (Sn_ESR_IP6T)         ///< It indicates the source ip address type when SOCKET is connected. (0 - Link Local, 1 - Global Unique) 

/////////////////////////////
// SOCKET CONTROL & OPTION //
/////////////////////////////
#define SOCK_IO_BLOCK         0  ///< Socket Block IO Mode in @ref setsockopt().
#define SOCK_IO_NONBLOCK      1  ///< Socket Non-block IO Mode in @ref setsockopt().


/**
 * @ingroup WIZnet_socket_APIs
 * @brief Open a socket.
 * @details Initializes the SOCKET have the <i>sn</i> number open it.
 *
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param protocol Protocol type to operate such as TCP, UDP IPRAW, and MACRAW. \n
 *  - TCP    : @ref Sn_MR_TCP(= @ref Sn_MR_TCP4), @ref Sn_MR_TCP6, and @ref Sn_MR_TCPD
 *  - UDP    : @ref Sn_MR_UDP(= @ref Sn_MR_UDP4), @ref Sn_MR_UDP6, and @ref Sn_MR_UDPD
 *  - IPRAW  : @ref Sn_MR_IPRAW(= @ref Sn_MR_IPRAW4) and @ref Sn_MR_IPRAW6
 *  - MACRAW : @ref Sn_MR_MACRAW       
 * @param port Source port number to be binded.
 * @param flag SOCKET flags are as following. It is used together with OR operation.
 *  - @ref SF_MULTI_ENABLE
 *  - @ref SF_ETHER_OWN    
 *  - @ref SF_BROAD_BLOCK 
 *  - @ref SF_TCP_FPSH 
 *  - @ref SF_TCP_NODELAY 
 *  - @ref SF_IGMP_VER2 
 *  - @ref SF_SOLICIT_BLOCK
 *  - @ref SF_ETHER_MULTI4B
 *  - @ref SF_UNI_BLOCK
 *  - @ref SF_ETHER_MULIT6B
 *  - @ref SF_DHA_MANUAL
 *  - @ref SF_FORCE_ARP
 *  - @ref SF_IO_NONBLOCK
 * @return Success : The SOCKET number sn passed as parameter\n
 *         Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid SOCKETn, <i>sn</i>\n
 *                     @ref SOCKERR_SOCKMODE    - Invalid SOCKETn mode\n
 *                     @ref SOCKERR_SOCKFLAG    - Invaild SOCKETn flag.
 * @sa _Sn_MR_, _Sn_MR2_ 
 */
int8_t socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Close a SOCKET.
 * @details It closes the SOCKET  with <b>'sn'</b> passed as parameter.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @return Success : @ref SOCK_OK \n
 *         Fail    : @ref SOCKERR_SOCKNUM - Invalid SOCKET number
 */
int8_t close(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Listen to a connection request from a <b>TCP CLIENT</b>.
 * @details It is listening to a connection request from a client.
 *          If connection request is accepted successfully, the connection is established. \n
 *          SOCKET <i>sn</i> is used as passive(<b>TCP SERVER</b>) mode.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @return Success : @ref SOCK_OK \n
 *         Fail    :\n @ref SOCKERR_SOCKINIT   - Socket is not initialized \n
 *                     @ref SOCKERR_SOCKCLOSED - Socket closed unexpectedly.
 */
int8_t listen(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to connect to a <b>TCP SERVER</b>.
 * @details It sends a connection-reqeust message to the server with destination IP address and port number passed as parameter.\n
 *          SOCKET <i>sn</i> is used as active(<b>TCP CLIENT</b>) mode.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param addr Pointer variable of destination IPv6 or IPv4 address. 
 * @param port Destination port number.
 * @param addrlen the length of <i>addr</i>. \n
 *                If addr is IPv6 address it should be 16,else if addr is IPv4 address it should be 4. Otherwize, return @ref SOCKERR_IPINVALID.
 * @return Success : @ref SOCK_OK \n
 *         Fail    :\n @ref SOCKERR_SOCKNUM   - Invalid socket number\n
 *                     @ref SOCKERR_SOCKMODE  - Invalid socket mode\n
 *                     @ref SOCKERR_SOCKINIT  - Socket is not initialized\n
 *                     @ref SOCKERR_IPINVALID - Wrong server IP address\n
 *                     @ref SOCKERR_PORTZERO  - Server port zero\n
 *                     @ref SOCKERR_TIMEOUT   - Timeout occurred during request connection\n
 *                     @ref SOCK_BUSY         - In non-block io mode, it returns immediately\n
 * @note It is valid only in TCP client mode. \n
 *       In block io mode, it does not return until connection is completed. \n
 *       In Non-block io mode(@ref SF_IO_NONBLOCK), it returns @ref SOCK_BUSY immediately.
 */
int8_t connect(uint8_t sn, uint8_t * addr, uint16_t port, uint8_t addrlen);


/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to disconnect to the connected peer.
 * @details It sends disconnect-request message to the connected peer which is <b>TCP SERVER</b> or <b>TCP CLIENT</b>.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @return Success :   @ref SOCK_OK \n
 *         Fail    :\n @ref SOCKERR_SOCKNUM  - Invalid SOCKET number \n
 *                     @ref SOCKERR_SOCKMODE - Invalid operation in the SOCKET \n
 *                     @ref SOCKERR_TIMEOUT  - Timeout occurred \n
 *                     @ref SOCK_BUSY        - In non-block io mode, it returns immediately.
 * @note It is valid only in TCP mode such as @ref Sn_MR_TCP4, @ref Sn_MR_TCP6, and @ref Sn_MR_TCPD. \n
 *       In block io mode, it does not return until disconnection is completed. \n
 *       In Non-block io mode(@ref SF_IO_NONBLOCK), it returns @ref SOCK_BUSY immediately. \n
 */
int8_t disconnect(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Send data to the connected peer.
 * @details It sends data to the connected peer by using TCP mode SOCKET <i>sn</i>.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer of data buffer to be sent.
 * @param len The byte length of data in <i>buf</i>.
 * @return   Success : The real sent data size. It may be equal to <i>len</i> or small. \n
 *          Fail    : \n @ref SOCKERR_SOCKSTATUS - Invalid SOCKET status for SOCKET operation \n
 *                          @ref SOCKERR_TIMEOUT    - Timeout occurred \n
 *                          @ref SOCKERR_SOCKMODE     - Invalid operation in the SOCKET \n
 *                          @ref SOCKERR_SOCKNUM    - Invalid SOCKET number \n
 *                          @ref SOCK_BUSY          - SOCKET is busy.
 * @note It is valid only in TCP mode such as @ref Sn_MR_TCP4, Sn_MR_TCP6, and Sn_MR_TCPD. \n
 *       It can send data as many as SOCKET TX buffer size if data is greater than SOCKET TX buffer size. \n
 *       In block io mode, It doesn't return until data sending is completed when SOCKET transmittable buffer size is greater than data. \n
 *       In non-block io mode(@ref SF_IO_NONBLOCK), It return @ref SOCK_BUSY immediately when SOCKET transmittable buffer size is not enough or the previous sent data is not completed. \n
 */
datasize_t send(uint8_t sn, uint8_t * buf, datasize_t len);


/**
 * @ingroup WIZnet_socket_APIs
 * @brief Receive data from the connected peer.
 * @details It can read data received from the connected peer by using TCP mode SOCKET <i>sn</i>.\n
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer buffer to read the received data.
 * @param len The max data length of data in buf.
 * @return Success : The real received data size. It may be equal to <i>len</i> or small. \n
 *         Fail    :\n
 *                  @ref SOCKERR_SOCKSTATUS - Invalid SOCKET status for SOCKET operation \n
 *                  @ref SOCKERR_SOCKMODE   - Invalid operation in the SOCKET \n
 *                  @ref SOCKERR_SOCKNUM    - Invalid SOCKET number \n
 *                   @ref SOCK_BUSY         - SOCKET is busy.
 * @note It is valid only in <b>TCP SERVER</b> or <b>TCP CLIENT</b> mode. \n
 *       It can read data as many as SOCKET RX buffer size if data is greater than SOCKET RX buffer size. \n
 *       In block io mode, it doesn't return until data reception is completed. that is, it waits until any data is received in SOCKET RX buffer. \n
 *       In non-block io mode(@ref SF_IO_NONBLOCK), it return @ref SOCK_BUSY immediately when SOCKET RX buffer is empty. \n
 *
 */
datasize_t recv(uint8_t sn, uint8_t * buf, datasize_t len);


/**
 * @ingroup WIZnet_socket_APIs
 * @brief Send datagram to the peer specifed by destination IP address and port number passed as parameter.
 * @details It sends datagram data by using UDP,IPRAW, or MACRAW mode SOCKET.
 * @param sn SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer of data buffer to be sent.
 * @param len The byte length of data in buf.
 * @param addr Pointer variable of destination IPv6 or IPv4 address. 
 * @param port Destination port number.
 * @param addrlen the length of <i>addr</i>. \n
 *                If addr is IPv6 address it should be 16,else if addr is IPv4 address it should be 4. Otherwize, return @ref SOCKERR_IPINVALID.
 * @return Success : The real sent data size. It may be equal to <i>len</i> or small.\n
 *         Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid SOCKET number \n
 *                     @ref SOCKERR_SOCKMODE    - Invalid operation in the SOCKET \n
 *                     @ref SOCKERR_SOCKSTATUS  - Invalid SOCKET status for SOCKET operation \n
 *                     @ref SOCKERR_IPINVALID   - Invalid IP address\n
 *                     @ref SOCKERR_PORTZERO    - Destination port number is zero\n
 *                     @ref SOCKERR_DATALEN     - Invalid data length \n
 *                     @ref SOCKERR_SOCKCLOSED  - SOCKET unexpectedly closed \n
 *                     @ref SOCKERR_TIMEOUT     - Timeout occurred \n
 *                     @ref SOCK_BUSY           - SOCKET is busy.
 * @note It is valid only in @ref Sn_MR_UDP4, @ref Sn_MR_UDP6, @ref Sn_MR_UDPD, @ref Sn_MR_IPRAW4, @ref Sn_MR_IPRAW6, and @ref Sn_MR_MACRAW. \n
 *       In UDP mode, It can send data as many as SOCKET RX buffer size if data is greater than SOCKET TX buffer size. \n
 *       In IPRAW and MACRAW mode, It should send data as many as MTU(maxium transmission unit) if data is greater than MTU. That is, <i>len</i> can't exceed to MTU.
 *       In block io mode, It doesn't return until data send is completed. 
 *       In non-block io mode(@ref SF_IO_NONBLOCK), It return @ref SOCK_BUSY immediately when SOCKET transimttable buffer size is not enough.
 */
datasize_t sendto(uint8_t sn, uint8_t * buf, datasize_t len, uint8_t * addr, uint16_t port, uint8_t addrlen);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Receive datagram from a peer 
 * @details It can read a data received from a peer by using UDP, IPRAW, or MACRAW mode SOCKET.
 * @param sn   SOCKET number. It should be <b>0 ~ @ref _WIZCHIP_SOCK_NUM_</b>.
 * @param buf  Pointer buffer to be saved the received data.
 * @param len  The max read data length. \n
 *             When the received packet size <= <i>len</i>, it can read data as many as the packet size. \n
 *             When others, it can read data as many as len and remain to the rest data of the packet.
 * @param addr Pointer variable of destination IP address.\n
 *             It is valid only when @ref recvfrom() is first called for receiving the datagram packet.
 *             You can check it valid or not through @ref PACK_FIRST. You can get it through @ref getsockopt(sn, @ref SO_PACKINFO, &packinfo).\n
 *             In UDP4, IPRAW mode SOCKET, it should be allocated over 4bytes. \n
 *             In UDP6, UDPD mode SOCKET, it should be allocated over 16bytes.
 * @param port Pointer variable of destination port number. \n
 *             It is valid only when @ref recvfrom() is first called for receiving the datagram packet, same as <i>port</i> case.
 * @param addrlen The byte length of destination IP address. \n
 *                It is valid only when @ref recvfrom() is first called for receiving the datagram packet, same as <i>port</i> case.\n
 *                When the destination has a IPv4 address, it is set to 4. \n
 *                when the destination has a IPv6 address, it is set to 16. 
 * @return   Success : The real received data size. It may be equal to <i>len</i> or small.\n
 *          Fail    : @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                    @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                    @ref SOCKERR_ARG        - Invalid parameter such as <i>addr</i>, <i>port</i>
 *                    @ref SOCK_BUSY          - SOCKET is busy.
 * @note It is valid only in @ref Sn_MR_UDP4, @ref Sn_MR_UDP6, @ref Sn_MR_UDPD, @ref Sn_MR_IPRAW4, @ref Sn_MR_IPRAW6, and @ref Sn_MR_MACRAW. \n
 *       When SOCKET is opened with @ref Sn_MR_MACRAW or When it reads the the remained data of the previous datagram packet,
 *       the parameters such as <i>addr</i>, <i>port</i>, <i>addrlen</i> is ignored. \n
 *       Also, It can read data as many as the received datagram packet size if <i>len</i> is greater than the datagram packet size. \n
 *       In block io mode, it doesn't return until data reception is completed. that is, it waits until any datagram packet is received in SOCKET RX buffer. \n
 *       In non-block io mode(@ref SF_IO_NONBLOCK), it return @ref SOCK_BUSY immediately when SOCKET RX buffer is empty. \n
 */
datasize_t recvfrom(uint8_t sn, uint8_t * buf, datasize_t len, uint8_t * addr, uint16_t *port, uint8_t *addrlen);



/**
 * @defgroup DATA_TYPE DATA TYPE
 */

/**
 * @ingroup DATA_TYPE
 * @brief The kind of SOCKET Interrupt.
 * @sa _Sn_IR_, _Sn_IMR_, setSn_IR(), getSn_IR(), setSn_IMR(), getSn_IMR()
 */
typedef enum
{
   SIK_CONNECTED     = (1 << 0),    ///< connected
   SIK_DISCONNECTED  = (1 << 1),    ///< disconnected
   SIK_RECEIVED      = (1 << 2),    ///< data received
   SIK_TIMEOUT       = (1 << 3),    ///< timeout occurred
   SIK_SENT          = (1 << 4),    ///< send ok
   SIK_ALL           = 0x1F         ///< all interrupt
}sockint_kind;

/**
 * @ingroup DATA_TYPE
 * @brief The type of @ref ctlsocket().
 */
typedef enum
{
   CS_SET_IOMODE,          ///< set SOCKET IO mode with @ref SOCK_IO_BLOCK or @ref SOCK_IO_NONBLOCK
   CS_GET_IOMODE,          ///< get SOCKET IO mode
   CS_GET_MAXTXBUF,        ///< get the size of SOCKET TX buffer allocated in TX memory
   CS_GET_MAXRXBUF,        ///< get the size of SOCKET RX buffer allocated in RX memory
   CS_CLR_INTERRUPT,       ///< clear the interrupt of SOCKET with @ref sockint_kind.
   CS_GET_INTERRUPT,       ///< get the SOCKET interrupt. refer to @ref sockint_kind.
   CS_SET_INTMASK,         ///< set the interrupt mask of SOCKET with @ref sockint_kind.
   CS_GET_INTMASK,         ///< get the masked interrupt of SOCKET. refer to @ref sockint_kind.
   CS_SET_PREFER,          ///< set the preferred source IPv6 address of transmission packet.\n Refer to @ref SRCV6_PREFER_AUTO, @ref SRCV6_PREFER_LLA and @ref SRCV6_PREFER_GUA.
   CS_GET_PREFER,          ///< get the preferred source IPv6 address of transmission packet.\n Refer to @ref SRCV6_PREFER_AUTO, @ref SRCV6_PREFER_LLA and @ref SRCV6_PREFER_GUA.
}ctlsock_type;


/**
 * @ingroup DATA_TYPE
 * @brief The type of socket option in @ref setsockopt() or @ref getsockopt()
 */ 
typedef enum
{
   SO_FLAG,             ///< Valid only in @ref getsockopt(), For set flag of socket refer to <i>flag</i> in @ref socket(). \n
   SO_TTL,              ///< Set/Get TTL. ( @ref setSn_TTLR(), @ref getSn_TTLR() ) \n
   SO_TOS,              ///< Set/Get TOS. ( @ref setSn_TOSR(), @ref getSn_TOSR() )
   SO_MSS,              ///< Set/Get MSS. ( @ref setSn_MSSR(), @ref getSn_MSSR() )
   SO_DESTIP,           ///< Set/Get the destination IP address with argument @ref wiz_IPAddress. To get it, SOCKETn should be TCP mode.
   SO_DESTPORT,         ///< Set/Get the destination Port number. To get it, SOCKETn should be TCP mode.
   SO_KEEPALIVESEND,    ///< Valid only in @ref setsockopt(). Manually send keep-alive packet in TCP mode.
   SO_KEEPALIVEAUTO,    ///< Set/Get keep-alive auto transmission timer in TCP mode 
   SO_SENDBUF,          ///< Valid only in @ref getsockopt(). Get the free data size of SOCKETn TX buffer. @ref getSn_TX_FSR()
   SO_RECVBUF,          ///< Valid only in @ref getsockopt(). Get the received data size in SOCKETn RX buffer. @ref getSn_RX_RSR()
   SO_STATUS,           ///< Valid only in @ref getsockopt(). Get the SOCKETn status. @ref getSn_SR()
   SO_EXTSTATUS,        ///< Valid only in @ref getsockopt(). Get the extended TCP SOCKETn status. @ref getSn_ESR()
   SO_REMAINSIZE,       ///< Valid only in @ref getsockopt(). Get the remained packet size in non-TCP mode.
   SO_MODE,
   SO_PACKINFO          ///< Valid only in @ref getsockopt(). Get the packet information as @ref PACK_FIRST, @ref PACK_REMAINED, and etc.
}sockopt_type;

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Control SOCKETn.
 * @details Control IO mode, Interrupt & Mask of SOCKETn and get the SOCKETn buffer information.
 *          Refer to @ref ctlsock_type.
 * @param sn socket number
 * @param cstype type of control SOCKETn. refer to @ref ctlsock_type.
 * @param arg Data type and value is determined according to @ref ctlsock_type. \n
 *          <table>
 *             <tr> <td> @b cstype                                </td> <td> <i>@b arg</i> type</td>
 *                  <td>@b value</td> </tr>
 *             <tr> <td> @ref CS_SET_IOMODE \n @ref CS_GET_IOMODE </td> <td> uint8_t           </td>
 *                  <td>@ref SOCK_IO_BLOCK, @ref SOCK_IO_NONBLOCK </td> </tr>
 *             <tr> <td> @ref CS_GET_MAXTXBUF \n @ref CS_GET_MAXRXBUF   </td> <td> datasize_t  </td>
 *                  <td> 0,1,2,4,8,16 KB </td> </tr>
 *             <tr> <td> @ref CS_CLR_INTERRUPT \n @ref CS_GET_INTERRUPT \n @ref CS_SET_INTMASK \n @ref CS_GET_INTMASK </td> 
 *                  <td> @ref sockint_kind </td> <td> @ref SIK_CONNECTED, etc.  </td> </tr> 
 *             <tr> <td> @ref CS_SET_PREFER \n @ref CS_GET_PREFER       </td> <td> uint8_t </td>
 *                  <td> @ref SRCV6_PREFER_AUTO, @ref SRCV6_PREFER_LLA, @ref SRCV6_PREFER_GUA  </td>< /tr>
 *          </table>
 * @return Success @ref SOCK_OK \n
 *         Fail   : \n
 *         - @ref SOCKERR_ARG         - Invalid argument\n
 *         - @ref SOCKERR_SOCKNUM     - Invalid Socket number \n
 */
int8_t ctlsocket(uint8_t sn, ctlsock_type cstype, void* arg);

/** 
 * @ingroup WIZnet_socket_APIs
 *  @brief Set SOCKETn options
 *  @details Set SOCKETn option like as TTL, MSS, TOS, and so on. Refer to @ref sockopt_type.
 *               
 *  @param sn SOCKET number
 *  @param sotype SOCKET option type. refer to @ref sockopt_type
 *  @param arg Data type and value is determined according to <I>sotype</I>. \n
 *           <table>
 *              <tr> <td> @b sotype             </td> <td> <i>@b >arg</i> type</td><td>@b value   </td> </tr> 
 *              <tr> <td> @ref SO_TTL           </td> <td> uint8_t            </td><td> 0 ~ 255   </td> </tr>
 *              <tr> <td> @ref SO_TOS           </td> <td> uint8_t            </td><td> 0 ~ 255   </td> </tr>
 *              <tr> <td> @ref SO_MSS           </td> <td> uint16_t           </td><td> 0 ~ 65535 </td> </tr>
 *              <tr> <td> @ref SO_DESTIP        </td> <td> @ref wiz_IPAddress </td><td>           </td> </tr> 
 *              <tr> <td> @ref SO_DESTPORT      </td> <td> uint16_t           </td><td> 1 ~ 65535 </td> </tr>
 *              <tr> <td> @ref SO_KEEPALIVESEND </td> <td> null               </td><td> null      </td> </tr> 
 *              <tr> <td> @ref SO_KEEPALIVEAUTO </td> <td> uint8_t            </td><td> 0 ~ 255   </td> </tr> 
 *           </table>
 * @return 
 *   - Success : @ref SOCK_OK \n
 *   - Fail
 *     - @ref SOCKERR_SOCKNUM     - Invalid SOCKET number \n
 *     - @ref SOCKERR_SOCKMODE    - Invalid SOCKET mode \n
 *     - @ref SOCKERR_SOCKOPT     - Invalid SOCKET option or its value \n
 *     - @ref SOCKERR_TIMEOUT     - Timeout occurred when sending keep-alive packet
 */
int8_t setsockopt(uint8_t sn, sockopt_type sotype, void* arg);

/** 
 * @ingroup WIZnet_socket_APIs
 *  @brief get SOCKETn options
 *  @details Get SOCKETn option like as FLAG, TTL, MSS, and so on. Refer to @ref sockopt_type
 *  @param sn SOCKET number
 *  @param sotype SOCKET option type. refer to @ref sockopt_type
 *  @param arg Data type and value is determined according to <I>sotype</I>.
 *           <table>
 *              <tr> <td> @b sotype             </td> <td> <i>@b arg</i> type </td><td> @b value                   </td></tr>
 *              <tr> <td> @ref SO_FLAG          </td> <td> uint8_t            </td><td> @ref SF_ETHER_OWN, etc...  </td> </tr>
 *              <tr> <td> @ref SO_TOS           </td> <td> uint8_t            </td><td> 0 ~ 255                    </td> </tr>
 *              <tr> <td> @ref SO_MSS           </td> <td> uint16_t           </td><td> 0 ~ 65535                  </td> </tr>
 *              <tr> <td> @ref SO_DESTIP        </td> <td> @ref wiz_IPAddress </td><td>                            </td></tr> 
 *              <tr> <td> @ref SO_DESTPORT      </td> <td> uint16_t           </td><td> 1 ~ 65535                  </td></tr>
 *              <tr> <td> @ref SO_KEEPALIVEAUTO </td> <td> uint8_t            </td><td> 0 ~ 255                    </td></tr> 
 *              <tr> <td> @ref SO_SENDBUF       </td> <td> @ref datasize_t    </td><td> 0 ~                        </td></tr>
 *              <tr> <td> @ref SO_RECVBUF       </td> <td> @ref datasize_t    </td><td> 0 ~                        </td></tr>
 *              <tr> <td> @ref SO_STATUS        </td> <td> uint8_t            </td><td> @ref SOCK_ESTABLISHED, etc. </td></tr>  
 *              <tr> <td> @ref SO_EXTSTATUS     </td> <td> uint8_t            </td><td> @ref TCPSOCK_MODE, @ref TCPSOCK_OP, @ref TCPSOCK_SIP </td></tr>   
 *              <tr> <td> @ref SO_REMAINSIZE    </td> <td> @ref datasize_t    </td><td> 0~                         </td></tr>
 *              <tr> <td> @ref SO_PACKINFO      </td> <td> uint8_t            </td><td> @ref PACK_FIRST, etc.      </td></tr>
 *           </table>
 * @return 
 *   - Success : @ref SOCK_OK \n
 *   - Fail
 *     - @ref SOCKERR_SOCKNUM   - Invalid Socket number \n
 *     - @ref SOCKERR_SOCKOPT   - Invalid socket option or its value \n
 *     - @ref SOCKERR_SOCKMODE  - Invalid socket mode \n
 * @note
 *   The option as @ref PACK_REMAINED of @ref SO_PACKINFO is valid only in NON-TCP mode. 
 */
int8_t getsockopt(uint8_t sn, sockopt_type sotype, void* arg);

/**
 * @ingroup WIZnet_socket_APIs
 *  @brief Peeks a sub-message in SOCKETn RX buffer
 *  @details It peeks the incoming message of SOCKETn RX buffer. \n
 *           It can find the specified sub-message in the incoming message and
 *           return the length of incoming message before the sub-message. \n
 *           It is useful when you need to read each messages from multiple message in SOCKET RX buffer.
 *  @param sn SOCKET number
 *  @param submsg sub-message pointer to find
 *  @param subsize the length of <i>submsg</i>
 * @return
 *   - Success : the length of incoming message length before the <i>submsg</i> \n
 *   - Fail : -1
 * @note
 *   It is just return the length of incoming message before the found sub-message. It does not receive the message.\n
 *   So, after calling peeksockmsg, @ref _Sn_RX_RD_ is not changed.
 */
int16_t peeksockmsg(uint8_t sn, uint8_t* submsg, uint16_t subsize);

#endif   // _SOCKET_H_
