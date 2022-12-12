#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "stdlib.h"

#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

//static uint16_t j=0;
static uint16_t any_port = 	50000;
static uint8_t curr_state[8] = {0,};
static uint8_t sock_state[8] = {0,};
uint8_t* msg_v4 = "IPv4 mode";
uint8_t* msg_v6 = "IPv6 mode";
uint8_t* msg_dual = "Dual IP mode";

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t loopback_mode)
{
    int32_t ret;
    datasize_t sentsize=0;
    int8_t status,inter;
    uint8_t tmp = 0;
    datasize_t received_size;
    uint8_t arg_tmp8;
    uint8_t* mode_msg;

    if(loopback_mode == AS_IPV4)
    {
       mode_msg = msg_v4;
    }else if(loopback_mode == AS_IPV6)
    {
       mode_msg = msg_v6;
    }else
    {
       mode_msg = msg_dual;
    }
    #ifdef _LOOPBACK_DEBUG_
        uint8_t dst_ip[16], ext_status;
        uint16_t dst_port;
    #endif
        getsockopt(sn, SO_STATUS, &status);
        switch(status)
        {
        case SOCK_ESTABLISHED :
            ctlsocket(sn,CS_GET_INTERRUPT,&inter);
            if(inter & Sn_IR_CON)
            {
            #ifdef _LOOPBACK_DEBUG_
                getsockopt(sn,SO_DESTIP,dst_ip);
                getsockopt(sn,SO_EXTSTATUS, &ext_status);
                if(ext_status & TCPSOCK_MODE){
                    //IPv6
                    printf("%d:Peer IP : %04X:%04X", sn, ((uint16_t)dst_ip[0] << 8) | ((uint16_t)dst_ip[1]),
                            ((uint16_t)dst_ip[2] << 8) | ((uint16_t)dst_ip[3]));
                    printf(":%04X:%04X", ((uint16_t)dst_ip[4] << 8) | ((uint16_t)dst_ip[5]),
                            ((uint16_t)dst_ip[6] << 8) | ((uint16_t)dst_ip[7]));
                    printf(":%04X:%04X", ((uint16_t)dst_ip[8] << 8) | ((uint16_t)dst_ip[9]),
                            ((uint16_t)dst_ip[10] << 8) | ((uint16_t)dst_ip[11]));
                    printf(":%04X:%04X, ", ((uint16_t)dst_ip[12] << 8) | ((uint16_t)dst_ip[13]),
                            ((uint16_t)dst_ip[14] << 8) | ((uint16_t)dst_ip[15]));
                }else
                {
                    //IPv4
                    //getSn_DIPR(sn,dst_ip);
                    printf("%d:Peer IP : %.3d.%.3d.%.3d.%.3d, ",
                            sn, dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3]);
                }
                getsockopt(sn,SO_DESTPORT,&dst_port);
                printf("Peer Port : %d\r\n", dst_port);
            #endif
                arg_tmp8 = Sn_IR_CON;
                ctlsocket(sn,CS_CLR_INTERRUPT,&arg_tmp8);
            }
            getsockopt(sn,SO_RECVBUF,&received_size);

            if(received_size > 0){
                if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE;
                ret = recv(sn, buf, received_size);

                if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                received_size = (uint16_t) ret;
                sentsize = 0;

                while(received_size != sentsize)
                {
                    ret = send(sn, buf+sentsize, received_size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
            }
            break;
        case SOCK_CLOSE_WAIT :
            #ifdef _LOOPBACK_DEBUG_
                printf("%d:CloseWait\r\n",sn);
            #endif
            getsockopt(sn, SO_RECVBUF, &received_size);
            if(received_size > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE;
                ret = recv(sn, buf, received_size);

                if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                received_size = (uint16_t) ret;
                sentsize = 0;

                while(received_size != sentsize)
                {
                    ret = send(sn, buf+sentsize, received_size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
            }

            if((ret = disconnect(sn)) != SOCK_OK) return ret;
                #ifdef _LOOPBACK_DEBUG_
                    printf("%d:Socket Closed\r\n", sn);
                #endif
            break;
        case SOCK_INIT :
            if( (ret = listen(sn)) != SOCK_OK) return ret;
                #ifdef _LOOPBACK_DEBUG_
                    printf("%d:Listen, TCP server loopback, port [%d] as %s\r\n", sn, port, mode_msg);
                #endif
                    printf("%d:Listen, TCP server loopback, port [%d] as %s\r\n", sn, port, mode_msg);
            break;
        case SOCK_CLOSED:
            #ifdef _LOOPBACK_DEBUG_
                printf("%d:TCP server loopback start\r\n",sn);
            #endif
                switch(loopback_mode)
                {
                case AS_IPV4:
                    tmp = socket(sn, Sn_MR_TCP4, port, SOCK_IO_NONBLOCK);
                    break;
                case AS_IPV6:
                    tmp = socket(sn, Sn_MR_TCP6, port, SOCK_IO_NONBLOCK);
                    break;
                case AS_IPDUAL:
                    tmp = socket(sn, Sn_MR_TCPD, port, SOCK_IO_NONBLOCK);
                    break;
                default:
                    break;
                }
                if(tmp != sn)    /* reinitialize the socket */
                {
                    #ifdef _LOOPBACK_DEBUG_
                        printf("%d : Fail to create socket.\r\n",sn);
                    #endif
                    return SOCKERR_SOCKNUM;
                }
            #ifdef _LOOPBACK_DEBUG_
                printf("%d:Socket opened[%d]\r\n",sn, getSn_SR(sn));
                sock_state[sn] = 1;
            #endif
            break;
        default:
            break;
        }
    return 1;
}


int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport, uint8_t loopback_mode)
{

    int32_t ret; // return value for SOCK_ERRORs
    datasize_t sentsize=0;
    uint8_t status,inter,addr_len;
    datasize_t received_size;
    uint8_t tmp = 0;
    uint8_t arg_tmp8;
    wiz_IPAddress destinfo;


    // Socket Status Transitions
    // Check the W6100 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
    getsockopt(sn,SO_STATUS,&status);
    switch(status)
    {
    case SOCK_ESTABLISHED :
        ctlsocket(sn,CS_GET_INTERRUPT,&inter);
        if(inter & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
        {
            #ifdef _LOOPBACK_DEBUG_
                printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
            #endif
            arg_tmp8 = Sn_IR_CON;
            ctlsocket(sn,CS_CLR_INTERRUPT,&arg_tmp8);// this interrupt should be write the bit cleared to '1'
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Data Transaction Parts; Handle the [data receive and send] process
        //////////////////////////////////////////////////////////////////////////////////////////////
        getsockopt(sn, SO_RECVBUF, &received_size);

        if(received_size > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
            ret = recv(sn, buf, received_size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
            received_size = (uint16_t) ret;
            sentsize = 0;

            // Data sentsize control
            while(received_size != sentsize)
            {
                ret = send(sn, buf+sentsize, received_size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if(ret < 0) // Send Error occurred (sent data length < 0)
                {
                    close(sn); // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        break;

    case SOCK_CLOSE_WAIT :
        #ifdef _LOOPBACK_DEBUG_
            printf("%d:CloseWait\r\n",sn);
        #endif
        getsockopt(sn, SO_RECVBUF, &received_size);

        if((received_size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
            ret = recv(sn, buf, received_size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
            received_size = (uint16_t) ret;
            sentsize = 0;

            // Data sentsize control
            while(received_size != sentsize)
            {
                ret = send(sn, buf+sentsize, received_size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if(ret < 0) // Send Error occurred (sent data length < 0)
                {
                    close(sn); // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        if((ret=disconnect(sn)) != SOCK_OK) return ret;
            #ifdef _LOOPBACK_DEBUG_
                printf("%d:Socket Closed\r\n", sn);
            #endif
        break;

    case SOCK_INIT :
        #ifdef _LOOPBACK_DEBUG_
            if(loopback_mode == AS_IPV4)
                printf("%d:Try to connect to the %d.%d.%d.%d, %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
            else if(loopback_mode == AS_IPV6)
            {
                printf("%d:Try to connect to the %04X:%04X", sn, ((uint16_t)destip[0] << 8) | ((uint16_t)destip[1]),
                    ((uint16_t)destip[2] << 8) | ((uint16_t)destip[3]));
                printf(":%04X:%04X", ((uint16_t)destip[4] << 8) | ((uint16_t)destip[5]),
                    ((uint16_t)destip[6] << 8) | ((uint16_t)destip[7]));
                printf(":%04X:%04X", ((uint16_t)destip[8] << 8) | ((uint16_t)destip[9]),
                    ((uint16_t)destip[10] << 8) | ((uint16_t)destip[11]));
                printf(":%04X:%04X,", ((uint16_t)destip[12] << 8) | ((uint16_t)destip[13]),
                    ((uint16_t)destip[14] << 8) | ((uint16_t)destip[15]));
                printf("%d\r\n", destport);
            }
        #endif

        if(loopback_mode == AS_IPV4)
          ret = connect(sn, destip, destport, 4); /* Try to connect to TCP server(Socket, DestIP, DestPort) */
        else if(loopback_mode == AS_IPV6)
          ret = connect(sn, destip, destport, 16); /* Try to connect to TCP server(Socket, DestIP, DestPort) */

        printf("SOCK Status: %d\r\n", ret);

        if( ret != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
        break;

    case SOCK_CLOSED:
        switch(loopback_mode)
        {
        case AS_IPV4:
            tmp = socket(sn, Sn_MR_TCP4, any_port++, SOCK_IO_NONBLOCK);
            break;
        case AS_IPV6:
            tmp = socket(sn, Sn_MR_TCP6, any_port++, SOCK_IO_NONBLOCK);
            break;
        case AS_IPDUAL:
            tmp = socket(sn, Sn_MR_TCPD, any_port++, SOCK_IO_NONBLOCK);
            break;
        default:
            break;
        }

        if(tmp != sn){    /* reinitialize the socket */
            #ifdef _LOOPBACK_DEBUG_
                printf("%d : Fail to create socket.\r\n",sn);
            #endif
            return SOCKERR_SOCKNUM;
        }
        printf("%d:Socket opened[%d]\r\n",sn, getSn_SR(sn));
        sock_state[sn] = 1;

        break;
    default:
        break;
    }
    return 1;
}

int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port, uint8_t loopback_mode)
{
    uint8_t status;
    static uint8_t destip[16] = {0,};
    static uint16_t destport;
    uint8_t pack_info;
    uint8_t addr_len;
    datasize_t ret;
    datasize_t received_size;
    uint16_t size, sentsize;
    uint8_t* mode_msg;

    if(loopback_mode == AS_IPV4)
    {
            mode_msg = msg_v4;
    }else if(loopback_mode == AS_IPV6)
    {
            mode_msg = msg_v6;
    }else
    {
            mode_msg = msg_dual;
    }

    getsockopt(sn, SO_STATUS,&status);
    switch(status)
    {
    case SOCK_UDP:
        getsockopt(sn, SO_RECVBUF, &received_size);
        if(received_size > DATA_BUF_SIZE) received_size = DATA_BUF_SIZE;
        if(received_size>0)
        {
            ret = recvfrom(sn, buf, received_size, (uint8_t*)&destip, (uint16_t*)&destport, &addr_len);

            if(ret <= 0)
             return ret;
            received_size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != received_size){
                ret = sendto(sn, buf+sentsize, received_size-sentsize, destip, destport, addr_len);

                if(ret < 0) return ret;

                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
             }
        }
        break;
    case SOCK_CLOSED:

        switch(loopback_mode)
        {
        case AS_IPV4:
           socket(sn,Sn_MR_UDP4, port, SOCK_IO_NONBLOCK);
           break;
        case AS_IPV6:
           socket(sn,Sn_MR_UDP6, port, SOCK_IO_NONBLOCK);
           break;
        case AS_IPDUAL:
            socket(sn,Sn_MR_UDPD, port, SOCK_IO_NONBLOCK);
            break;
        }
        printf("%d:Opened, UDP loopback, port [%d] as %s\r\n", sn, port, mode_msg);

    }

    return 0;
}

#endif
