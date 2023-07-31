//! \file 		ping.c
//! \brief 		ping.c script
//! \version	1.1.1
//! \date 		2017/04/04
//! \par		Revision history
//!				<2017/04/04> Notice
//!				Change function for control IPRAW Ping Request or Reply mode
//!				1. Added request_flag parameter: ping_auto(), ping_reply()
//!				2. request_flag 0: Send Request ping from outside MCU to MCU
//!                request_flag 1: Send Request ping from MCU to outside MCU
//!				Update function: ping_reply
//!				1. More check data: CheckSum, SeqNum
//!				2. Added function: When Send Request ping from outside to MCU, send data using sendto()
//!				3. Consider request_flag when send request ping from outside to MCU: PingReply.Type
//!				I modified each of the functions to see more data in the serial window.
//! \date		2018/05/25
//!	\par		Modified: Checksum, SeqNum
//!	\author		Matthew

#include "ping.h"
#include "stdio.h"


PINGMSGR PingRequest;	 // Variable for Ping Request
PINGMSGR PingReply;	     // Variable for Ping Reply
static uint16_t RandomID = 0x1234; 
static uint16_t RandomSeqNum = 0x4321;
uint8_t ping_reply_received = 0; 
uint8_t req=0;
uint8_t rep=0;

#if 1
// 20230726 taylor
extern wiz_NetInfo gWIZNETINFO;
#endif

/*****************************************************************************************
	Function name: wait_1us
	Input		:	cnt; Delay duration = cnt * 1u seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*1u second.
*****************************************************************************************/
void wait_1us(unsigned int cnt)
{
	unsigned int i;

	for(i = 0; i<cnt; i++) {


		}
}


/*****************************************************************************************
	Function name: wait_1ms
	Input		:	cnt; Delay duration = cnt * 1m seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*1m second. This function use wait_1us but the wait_1us
		has some error (not accurate). So if you want exact time delay, please use the Timer.
*****************************************************************************************/
void wait_1ms(unsigned int cnt)
{
	unsigned int i;
	for (i = 0; i < cnt; i++) wait_1us(1000);
}

/*****************************************************************************************
	Function name: wait_10ms
	Input		:	cnt; Delay duration = cnt * 10m seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*10m second. This function use wait_1ms but the wait_1ms
		has some error (not accurate more than wait_1us). So if you want exact time delay,
		please use the Timer.
*****************************************************************************************/
void wait_10ms(unsigned int cnt)
{
	unsigned int i;
	for (i = 0; i < cnt; i++) wait_1ms(10);
}



uint8_t ping_auto(uint8_t s, uint8_t *addr, uint8_t request_flag)
{
	uint8_t i;
	int32_t len = 0;
	uint8_t cnt=0;
	uint16_t time_i, time_j;
	uint8_t wait_ping_reply = 0;

	for(i = 0; i<4;i++)
	{

		switch(getSn_SR(s))
		{
			case SOCK_CLOSED:
				close(s);
        #if 1
        // 20230726 taylor
        setSn_PNR(s, 1);
        #else
				IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP);              // set ICMP Protocol
        #endif

				if(socket(s,Sn_MR_IPRAW,3000,0)!=0)
				{
				// open the SOCKET with IPRAW mode, if fail then Error
					printf( "\r\n socket %d fail \r\n",   (0)) ;
#ifdef PING_DEBUG
					return SOCKET_ERROR;
#endif
				}
		/* Check socket register */
				while(getSn_SR(s)!=SOCK_IPRAW);
        #if 1
        // 20230726 taylor
        wait_10ms(1);
        #else
					wait_10ms(10000); // wait 1000ms
					wait_10ms(10000); // wait 1000ms
          #endif
					break;
			case SOCK_IPRAW:
				/* Modes for performing REQUEST in the MCU */
				if(request_flag)
				{
					for(req = 0; req < 5; req++)
					{
						ping_request(s,addr);
						wait_ping_reply = 1;
						printf("send!");
						printf("\r\n wait_ping_reply = %d\r\n", wait_ping_reply);

						while(1)
						{
#if 1
// 20230727 taylor
              len = getSn_RX_RSR(s);
              if(len > 0)
              {
              #if 0
                printf("%s(%d)\r\n len = %d\r\n", __FILE__, __LINE__, len);
              #endif
                ping_reply(s, addr, len, request_flag);
                #if 1
                // 20230731 taylor
                rep++;
								if(ping_reply_received)
								{
									printf("received!");
									wait_ping_reply = 0;
									printf("\r\n wait_ping_reply = %d\r\n", wait_ping_reply);
									break;
								}
                #endif
              }
#else
							if((len = getSn_RX_RSR(s)) > 0)
							{
								ping_reply(s, addr, len, request_flag);
								rep++;
								if(ping_reply_received)
								{
									printf("received!");
									wait_ping_reply = 0;
									printf("\r\n wait_ping_reply = %d\r\n", wait_ping_reply);
									break;
								}
							}
							else if(cnt > 100)
							{
								printf("Request Time Out. \r\n");
								cnt = 0;
								break;
							}
							else
							{

								for(time_i = 0 ; time_i < 5000; time_i++)
								{
									for(time_j = 0 ; time_j < 1000; time_j++)
									{

									}
								}
								cnt++;
							}
#endif
						}
						printf("len = %d \r\n", len);
						printf("req = %d", req);
						if(req == 4)
						{
							printf("req = %d", req);
							return req;
						}
					}
				}
				/* Modes for performing REQUEST in outside the MCU */
				else
				{
					while(1)
					{
						if((len = getSn_RX_RSR(s))> 0 )
						{
							printf("1");
							ping_reply(s, addr, len, request_flag);
							rep++;
						}

					}
				}

				break;

			default:
				break;
		}

	}

}

uint8_t ping_request(uint8_t s, uint8_t *addr){
  uint16_t i;
  int send_result;
	//Initailize flag for ping reply
	ping_reply_received = 0;
	/* make header of the ping-request  */
	PingRequest.Type = PING_REQUEST;                // Ping-Request
	PingRequest.Code = CODE_ZERO;	                // Always '0'
	PingRequest.ID = htons(RandomID++);	      		// set ping-request's ID to random integer value
	PingRequest.SeqNum =htons(RandomSeqNum++);		// set ping-request's sequence number to random integer value
	//size = 32;                                 	// set Data size

	/* Fill in Data[]  as size of BIF_LEN (Default = 32)*/
  	for(i = 0 ; i < BUF_LEN; i++)
  	{
		PingRequest.Data[i] = (i) % 8;		  		//'0'~'8' number into ping-request's data
	}


  	printf("\r\n");
  	for(i = 0 ; i < BUF_LEN; i++)
  	{
  		printf("%x ",PingRequest.Data[i]);
  	}

  	printf("\r\n=========================\r\n");
	/* Do checksum of Ping Request */
	PingRequest.CheckSum = 0;		               	// value of checksum before calculating checksum of ping-request packet
	PingRequest.CheckSum = htons(checksum((uint8_t*)&PingRequest,sizeof(PingRequest)));  // Calculate checksum

  #if 1
  // 20230726 taylor
  send_result = sendto(s,(uint8_t *)&PingRequest,sizeof(PingRequest),addr,3000, 4);
  #else
	send_result = sendto(s,(uint8_t *)&PingRequest,sizeof(PingRequest),addr,3000);
  #endif

	if(send_result == 0)
	{
		/* Send Ping-Request to the specified peer.*/
	  	 printf( "\r\n Fail to send ping-reply packet  \r\n") ;
	}
	else
	{
	     /* sendto ping_request to destination */
	 	  printf( "Send Ping Request  to Destination (") ;
          printf( "%d.%d.%d.%d )",   (addr[0]),  (addr[1]),  (addr[2]),  (addr[3])) ;
		  printf( " ID:%x  SeqNum:%x CheckSum:%x\r\n",   htons(PingRequest.ID),  htons(PingRequest.SeqNum),  PingRequest.CheckSum) ;
		  printf("==========================================\r\n");
	}
	return 0;
} // ping request

uint8_t ping_reply(uint8_t s, uint8_t *addr,  uint16_t rlen, uint8_t request_flag)
{

	 uint16_t len;
	 uint16_t i;

	 uint8_t data_buf[128];
	 uint8_t comp_data_buf[128];
	 uint16_t destport = 3000;

	 uint8_t error_SeqNum;
	 uint8_t error_CheckSum;

	 uint16_t tmp_checksum;
	 uint16_t comp_request_checksum;
	 uint16_t comp_reply_checksum;

	 int32_t  send_rep;
	 PINGMSGR PingReply;

   #if 1
   // 20230727 taylor
   uint8_t addrlen;
   #endif

  #if 1
  #if 1
  // 20230727 taylor
  len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport, &addrlen);
  #else
  // 20230726 taylor
  len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport, 4);
  #endif
  #else
	 len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport);
  #endif

  #if 1
  // 20230727 taylor
  printf("%s(%d)\r\n len = %d\r\n", __FILE__, __LINE__, len);
  #endif

	if(data_buf[0] == PING_REPLY)
	{
		printf("PING_REPLY\r\n");
		PingReply.Type 		 = data_buf[0];
		PingReply.Code 		 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];
		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}

		/* check Checksum of Ping Reply */
		tmp_checksum = ~checksum(&data_buf,len);

		/* Operation for comparing CheckSum */
		comp_reply_checksum = (data_buf[3]<<8) + data_buf[2];
		comp_reply_checksum = comp_reply_checksum >> 8;
		comp_request_checksum = PingRequest.CheckSum >> 8;


		PingReply.CheckSum = 0;
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));

		if(PingRequest.SeqNum == PingReply.SeqNum)
		{
			if(tmp_checksum != 0xffff)
				printf("tmp_checksum = %x\r\n",tmp_checksum);
			else
			{
				/*  Compare Checksum of Ping Reply and Ping Request */
				if(comp_request_checksum == comp_reply_checksum)
				{
					/*  Output the Destination IP and the size of the Ping Reply Message  */
					printf("Reply from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes  CheckSum %x\r\n",
							(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  htons(PingReply.ID),  htons(PingReply.SeqNum),  (rlen+6), PingReply.CheckSum );
					printf("\r\n");

					/*  SET ping_reply_receiver to '1' and go out the while_loop (waiting for ping reply)  */
					ping_reply_received =1;
				}
				else
				{
					error_CheckSum++;
					printf("\r\n Error CheckSum\r\n");
				}
			}
		}
		else
		{
			error_SeqNum++;
			printf("\r\nError Sequence: %d\r\n", error_SeqNum);
		}
	}

	else if(data_buf[0] == PING_REQUEST)
	{
		printf("PING_REQUEST\r\n");
		/*for comp checksum*/
		PingReply.Type = data_buf[0];
		PingReply.Code 	 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];

		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}


		tmp_checksum = PingReply.CheckSum;

		/* Calculate Checksum of Ping Reply */
		PingReply.CheckSum = 0;
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));

		printf("PingReply.CheckSum = %x \r\n", htons(PingReply.CheckSum));

		if(tmp_checksum != PingReply.CheckSum){
			printf( " \r\n Request CheckSum is incorrect %x should be %x \r\n",   (tmp_checksum),  htons(PingReply.CheckSum)) ;
		}
		else
		{
			printf( "Request Checksum is correct  \r\n") ;
		}
		/* end of comp checksum*/


		PingReply.Type = 0;
		PingReply.Code 	 = data_buf[1];

		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];

		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}

		/* check Checksum of Ping Reply */
		tmp_checksum = PingReply.CheckSum;

		PingReply.CheckSum = 0;
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));

		/* sendto() is used to send data from the outside */
    #if 1
    // 20230726 taylor
    send_rep = sendto(s,(uint8_t *)&PingReply,sizeof(PingReply), addr, 3000, 4);
    #else
	   	send_rep = sendto(s,(uint8_t *)&PingReply,sizeof(PingReply), addr, 3000);
    #endif

		if(send_rep<0)
		{
		  	 printf( "\r\n Fail to send ping-reply packet, error num = %d \r\n", send_rep);
		}
		else
		{
			/*  Output the Destination IP and the size of the Ping Reply Message*/
			printf("\r\n Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
					(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  (PingReply.ID),  (PingReply.SeqNum),  (rlen+6) );
		}

		/*  SET ping_reply_receiver to '1' and go out the while_loop (waiting for ping reply)*/
		ping_reply_received =1;
	}

	return 0;
}// ping_reply

#if 1
// 20230726 taylor
uint8_t ping6_auto(uint8_t s, uint8_t *addr, uint8_t request_flag)
{
	uint8_t i;
	int32_t len = 0;
	uint8_t cnt=0;
	uint16_t time_i, time_j;
	uint8_t wait_ping_reply = 0;

	for(i = 0; i<4;i++)
	{

		switch(getSn_SR(s))
		{
			case SOCK_CLOSED:
				close(s);
        #if 1
        // 20230726 taylor
        setSn_PNR(s, 58);
        #else
				IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP);              // set ICMP Protocol
        #endif

				if(socket(s,Sn_MR_IPRAW6,3000,0)!=0)
				{
				// open the SOCKET with IPRAW mode, if fail then Error
					printf( "\r\n socket %d fail \r\n",   (0)) ;
#ifdef PING_DEBUG
					return SOCKET_ERROR;
#endif
				}
		/* Check socket register */
				while(getSn_SR(s)!=SOCK_IPRAW6);
        #if 1
        // 20230731 taylor
        printf("getSn_SR(s) == SOCK_IPRAW6\r\n");
        #endif
        #if 0
        #if 0
        // 20230726 taylor
        wait_10ms(1);
        #else
					wait_10ms(10000); // wait 1000ms
					wait_10ms(10000); // wait 1000ms
          #endif
        #endif
					break;
			case SOCK_IPRAW6:
				/* Modes for performing REQUEST in the MCU */
				if(request_flag)
				{
					for(req = 0; req < 5; req++)
					{
						ping6_request(s,addr);
						wait_ping_reply = 1;
						printf("send!");
						printf("\r\n wait_ping_reply = %d\r\n", wait_ping_reply);

						while(1)
						{
							if((len = getSn_RX_RSR(s)) > 0)
							{
							  #if 0
                printf("%s : %d\r\n", __FILE__, __LINE__);
                #endif
								ping6_reply(s, addr, len, request_flag);
								rep++;
								if(ping_reply_received)
								{
									printf("received!");
									wait_ping_reply = 0;
									printf("\r\n wait_ping_reply = %d\r\n", wait_ping_reply);
									break;
								}
							}
							else if(cnt > 100)
							{
								printf("Request Time Out. \r\n");
								cnt = 0;
								break;
							}
							else
							{
							#if 0
#if 0
                for(time_i = 0 ; time_i < 500; time_i++)
                {
                  for(time_j = 0 ; time_j < 100; time_j++);
                }

#else
								for(time_i = 0 ; time_i < 5000; time_i++)
								{
									for(time_j = 0 ; time_j < 1000; time_j++)
									{

									}
								}
#endif
#endif
								cnt++;
							}

						}
						printf("len = %d \r\n", len);
						printf("req = %d", req);
						if(req == 4)
						{
							printf("req = %d", req);
							return req;
						}
					}
				}
				/* Modes for performing REQUEST in outside the MCU */
				else
				{
					while(1)
					{
						if((len = getSn_RX_RSR(s))> 0 )
						{
							printf("1");
							ping6_reply(s, addr, len, request_flag);
							rep++;
						}

					}
				}

				break;

			default:
				break;
		}

	}

}

uint8_t ping6_request(uint8_t s, uint8_t *addr){
  uint16_t i;
  int send_result;
	//Initailize flag for ping reply
	ping_reply_received = 0;
	/* make header of the ping-request  */
	PingRequest.Type = PING6_REQUEST;                // Ping-Request
	PingRequest.Code = CODE_ZERO;	                // Always '0'
	PingRequest.ID = htons(RandomID++);	      		// set ping-request's ID to random integer value
	PingRequest.SeqNum =htons(RandomSeqNum++);		// set ping-request's sequence number to random integer value
	//size = 32;                                 	// set Data size

	/* Fill in Data[]  as size of BIF_LEN (Default = 32)*/
  	for(i = 0 ; i < BUF_LEN; i++)
  	{
		PingRequest.Data[i] = (i) % 8;		  		//'0'~'8' number into ping-request's data
	}


  	printf("\r\n");
  	for(i = 0 ; i < BUF_LEN; i++)
  	{
  		printf("%x ",PingRequest.Data[i]);
  	}

  	printf("\r\n=========================\r\n");
	/* Do checksum of Ping Request */
	PingRequest.CheckSum = 0;		               	// value of checksum before calculating checksum of ping-request packet

  CSUM Pseudo;
  
  // Source Address
  memcpy(Pseudo.Src, gWIZNETINFO.gua, 16);
  memcpy(Pseudo.Dst, addr, 16);
  Pseudo.Next[0] = 0x00;
  Pseudo.Next[1] = 0x3A;
  Pseudo.Len_icmp6[0] = (sizeof(PingRequest)&0xff00)>>8;
  Pseudo.Len_icmp6[1] = sizeof(PingRequest)&0xff;

  #if 1
  memcpy(&(Pseudo.Icmpv6), &PingRequest, sizeof(PingRequest));
  #else
  Pseudo.Icmpv6.Type = PingRequest.Type;
  Pseudo.Icmpv6.Code = PingRequest.Code;
  Pseudo.Icmpv6.CheckSum = 0;
  Pseudo.Icmpv6.ID = PingRequest.ID;
  Pseudo.Icmpv6.SeqNum = PingRequest.SeqNum;
  Pseudo.Icmpv6.Data
  #endif

  
  
  PingRequest.CheckSum = htons(checksum6((uint8_t*)&Pseudo,sizeof(Pseudo)));  // Calculate checksum

  #if 1
  // 20230726 taylor
  send_result = sendto(s,(uint8_t *)&PingRequest,sizeof(PingRequest),addr,3000, 16);
  #else
	send_result = sendto(s,(uint8_t *)&PingRequest,sizeof(PingRequest),addr,3000);
  #endif

	if(send_result == 0)
	{
		/* Send Ping-Request to the specified peer.*/
	  	 printf( "\r\n Fail to send ping-reply packet  \r\n") ;
	}
	else
	{
	     /* sendto ping_request to destination */
	 	  printf( "Send Ping Request  to Destination \r\n") ;
      printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[0]),  (addr[1]),  (addr[2]),  (addr[3])) ;
      printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[4]),  (addr[5]),  (addr[6]),  (addr[7])) ;
      printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[8]),  (addr[9]),  (addr[10]),  (addr[11])) ;
      printf( "0x%x:0x%x:0x%x:0x%x\r\n",   (addr[12]),  (addr[13]),  (addr[14]),  (addr[15])) ;
		  printf( " ID:%x  SeqNum:%x CheckSum:%x\r\n",   htons(PingRequest.ID),  htons(PingRequest.SeqNum),  PingRequest.CheckSum) ;
		  printf("==========================================\r\n");
	}
	return 0;
} // ping request

uint8_t ping6_reply(uint8_t s, uint8_t *addr,  uint16_t rlen, uint8_t request_flag)
{

	 uint16_t len;
	 uint16_t i;

	 uint8_t data_buf[128];
	 uint8_t comp_data_buf[128];
	 uint16_t destport = 3000;

	 uint8_t error_SeqNum;
	 uint8_t error_CheckSum;

	 uint16_t tmp_checksum;
	 uint16_t comp_request_checksum;
	 uint16_t comp_reply_checksum;

	 int32_t  send_rep;
	 PINGMSGR PingReply;
   CSUM Pseudo;
   #if 1
   // 20230727 taylor
   uint8_t addrlen;
   #endif

  #if 1
  #if 1
  // 20230727 taylor
  len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport, &addrlen);
  #else
  // 20230726 taylor
  len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport, 16);
  #endif
  #else
	 len = recvfrom(s, (uint8_t *)data_buf,rlen,addr,(uint16_t*)&destport);
  #endif

#if 1
// 20230731 taylor
  if(data_buf[0] == PING6_REPLY)
#else
	if(data_buf[0] == PING_REPLY)
#endif
	{
		printf("PING6_REPLY\r\n");
    #if 1
    #if 0
    printf( "Receive Ping Reply from Source\r\n") ;
    printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[0]),  (addr[1]),  (addr[2]),  (addr[3]));
    printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[4]),  (addr[5]),  (addr[6]),  (addr[7]));
    printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[8]),  (addr[9]),  (addr[10]),  (addr[11]));
    printf( "0x%x:0x%x:0x%x:0x%x\r\n",   (addr[12]),  (addr[13]),  (addr[14]),  (addr[15]));
    printf("Port 0x%x(%d)\r\n", destport, destport);
    #endif

    PingReply.Type 		 = data_buf[0];
		PingReply.Code 		 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];
		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}

    CSUM Pseudo;

    // Source Address
    memcpy(Pseudo.Src, addr, 16);
    memcpy(Pseudo.Dst, gWIZNETINFO.gua, 16);
    Pseudo.Next[0] = 0x00;
    Pseudo.Next[1] = 0x3A;
    Pseudo.Len_icmp6[0] = (sizeof(PingReply)&0xff00)>>8;
    Pseudo.Len_icmp6[1] = sizeof(PingReply)&0xff;
    memcpy(&(Pseudo.Icmpv6), &PingReply, sizeof(PingReply));
    Pseudo.Icmpv6.CheckSum = 0;
    #endif
    
    #if 0
    // 20230731 taylor
    CSUM Pseudo;
  
    // Source Address
    memcpy(Pseudo.Src, gWIZNETINFO.gua, 16);
    memcpy(Pseudo.Dst, addr, 16);
    Pseudo.Next[0] = 0x00;
    Pseudo.Next[1] = 0x3A;
    Pseudo.Len_icmp6[0] = (sizeof(PingRequest)&0xff00)>>8;
    Pseudo.Len_icmp6[1] = sizeof(PingRequest)&0xff;

    #if 1
    memcpy(&(Pseudo.Icmpv6), &PingRequest, sizeof(PingRequest));
    #else
    Pseudo.Icmpv6.Type = PingRequest.Type;
    Pseudo.Icmpv6.Code = PingRequest.Code;
    Pseudo.Icmpv6.CheckSum = 0;
    Pseudo.Icmpv6.ID = PingRequest.ID;
    Pseudo.Icmpv6.SeqNum = PingRequest.SeqNum;
    Pseudo.Icmpv6.Data
    #endif

    
    
    PingRequest.CheckSum = htons(checksum6((uint8_t*)&Pseudo,sizeof(Pseudo)));  // Calculate checksum
    #endif

    #if 1
    /* check Checksum of Ping Reply */
		tmp_checksum = checksum6(&Pseudo,sizeof(Pseudo));
    #if 0
    printf("tmp_checksum = %x\r\n",tmp_checksum);
    for(i=0; i<sizeof(Pseudo); i++)
    {
      printf("Pseudo[%d] = 0x%x\r\n", i, *(((uint8_t*)&Pseudo)+i));
    }
    #endif
    #else
		PingReply.Type 		 = data_buf[0];
		PingReply.Code 		 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];
		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}

		/* check Checksum of Ping Reply */
		tmp_checksum = ~checksum6(&data_buf,len);
    #endif

		/* Operation for comparing CheckSum */
		comp_reply_checksum = (data_buf[2]<<8) + data_buf[3];
    #if 0
    // 20230731 taylor
    printf("comp_reply_checksum = 0x%x\r\n", comp_reply_checksum);
    #endif
    #if 0
    // 20230731 taylor
		comp_reply_checksum = comp_reply_checksum >> 8;
    #if 1
    // 20230731 taylor
    printf("comp_reply_checksum = 0x%x\r\n", comp_reply_checksum);
    #endif
		comp_request_checksum = PingRequest.CheckSum >> 8;
    #if 1
    // 20230731 taylor
    printf("PingRequest.CheckSum = 0x%x\r\n", PingRequest.CheckSum);
    printf("comp_request_checksum = 0x%x\r\n", comp_request_checksum);
    #endif
    #endif

    #if 0
    // 20230731 taylor
		PingReply.CheckSum = 0;
    #if 1
    // 20230731 taylor
    PingReply.CheckSum = htons(checksum6((uint8_t*)&PingReply,sizeof(PingReply)));
    #else
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));
    #endif
    #endif

    #if 0
    // 20230731 taylor
    printf("PingRequest.SeqNum = 0x%x\r\n", PingRequest.SeqNum);
    printf("PingReply.SeqNum = 0x%x\r\n", PingReply.SeqNum);
    #endif
		if(PingRequest.SeqNum == PingReply.SeqNum)
		{
		#if 1
      #if 0
      printf("tmp_checksum = 0x%x\r\n", tmp_checksum);
      printf("comp_reply_checksum = 0x%x\r\n", comp_reply_checksum);
      #endif
      /*  Compare Checksum of Ping Reply and Ping Request */
      if(tmp_checksum == comp_reply_checksum)
      {
        /*  Output the Destination IP and the size of the Ping Reply Message  */
        #if 1
        // 20230731 taylor
        printf("Reply from ");
        printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[0]),  (addr[1]),  (addr[2]),  (addr[3]));
        printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[4]),  (addr[5]),  (addr[6]),  (addr[7]));
        printf( "0x%x:0x%x:0x%x:0x%x:",   (addr[8]),  (addr[9]),  (addr[10]),  (addr[11]));
        printf( "0x%x:0x%x:0x%x:0x%x ",   (addr[12]),  (addr[13]),  (addr[14]),  (addr[15]));
        printf("ID:0x%x SeqNum:0x%x  :data size %d bytes  CheckSum 0x%x\r\n", htons(PingReply.ID), htons(PingReply.SeqNum), (rlen+6), PingReply.CheckSum);
        #else
        printf("Reply from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes  CheckSum %x\r\n",
        (addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  htons(PingReply.ID),  htons(PingReply.SeqNum),  (rlen+6), PingReply.CheckSum );
        printf("\r\n");
        #endif

        /*  SET ping_reply_receiver to '1' and go out the while_loop (waiting for ping reply)  */
        ping_reply_received =1;
      }
      else
      {
        error_CheckSum++;
        printf("\r\n Error CheckSum\r\n");
      }
    #else
			if(tmp_checksum != 0xffff)
				printf("tmp_checksum = %x\r\n",tmp_checksum);
			else
			{
				/*  Compare Checksum of Ping Reply and Ping Request */
				if(comp_request_checksum == comp_reply_checksum)
				{
					/*  Output the Destination IP and the size of the Ping Reply Message  */
					printf("Reply from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes  CheckSum %x\r\n",
							(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  htons(PingReply.ID),  htons(PingReply.SeqNum),  (rlen+6), PingReply.CheckSum );
					printf("\r\n");

					/*  SET ping_reply_receiver to '1' and go out the while_loop (waiting for ping reply)  */
					ping_reply_received =1;
				}
				else
				{
					error_CheckSum++;
					printf("\r\n Error CheckSum\r\n");
				}
			}
    #endif
		}
		else
		{
			error_SeqNum++;
			printf("\r\nError Sequence: %d\r\n", error_SeqNum);
		}
	}

#if 1
  // 20230731 taylor
  else if(data_buf[0] == PING6_REQUEST)
#else
	else if(data_buf[0] == PING_REQUEST)
#endif
	{
		printf("PING6_REQUEST\r\n");
		/*for comp checksum*/
		PingReply.Type = data_buf[0];
		PingReply.Code 	 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];

		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}


		tmp_checksum = PingReply.CheckSum;

		/* Calculate Checksum of Ping Reply */
		PingReply.CheckSum = 0;
    #if 1
    // 20230731 taylor
    PingReply.CheckSum = htons(checksum6((uint8_t*)&PingReply,sizeof(PingReply)));
    #else
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));
    #endif

		printf("PingReply.CheckSum = %x \r\n", htons(PingReply.CheckSum));

		if(tmp_checksum != PingReply.CheckSum){
			printf( " \r\n Request CheckSum is incorrect %x should be %x \r\n",   (tmp_checksum),  htons(PingReply.CheckSum)) ;
		}
		else
		{
			printf( "Request Checksum is correct  \r\n") ;
		}
		/* end of comp checksum*/


		PingReply.Type = 0;
		PingReply.Code 	 = data_buf[1];

		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];

		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}

		/* check Checksum of Ping Reply */
		tmp_checksum = PingReply.CheckSum;

		PingReply.CheckSum = 0;
    #if 1
    // 20230731 taylor
    PingReply.CheckSum = htons(checksum6((uint8_t*)&PingReply,sizeof(PingReply)));
    #else
		PingReply.CheckSum = htons(checksum((uint8_t*)&PingReply,sizeof(PingReply)));
    #endif

		/* sendto() is used to send data from the outside */
    #if 1
    // 20230726 taylor
    send_rep = sendto(s,(uint8_t *)&PingReply,sizeof(PingReply), addr, 3000, 16);
    #else
	   	send_rep = sendto(s,(uint8_t *)&PingReply,sizeof(PingReply), addr, 3000);
    #endif

		if(send_rep<0)
		{
		  	 printf( "\r\n Fail to send ping-reply packet, error num = %d \r\n", send_rep);
		}
		else
		{
			/*  Output the Destination IP and the size of the Ping Reply Message*/
      printf("\r\n Request from\r\n");
      printf("0x%x:0x%x:0x%x:0x%x:",   (addr[0]),  (addr[1]),  (addr[2]),  (addr[3])) ;
      printf("0x%x:0x%x:0x%x:0x%x:",   (addr[4]),  (addr[5]),  (addr[6]),  (addr[7])) ;
      printf("0x%x:0x%x:0x%x:0x%x:",   (addr[8]),  (addr[9]),  (addr[10]),  (addr[11])) ;
      printf("0x%x:0x%x:0x%x:0x%x\r\n",   (addr[12]),  (addr[13]),  (addr[14]),  (addr[15])) ;
      
			printf("ID:%x SeqNum:%x  :data size %d bytes\r\n",
					(PingReply.ID),  (PingReply.SeqNum),  (rlen+6) );
		}

		/*  SET ping_reply_receiver to '1' and go out the while_loop (waiting for ping reply)*/
		ping_reply_received =1;
	}

	return 0;
}// ping_reply

#endif

uint16_t checksum(uint8_t * data_buf, uint16_t len)

{
  uint16_t sum, tsum, i, j;
  uint32_t lsum;

  j = len >> 1;
  lsum = 0;
  tsum = 0;
  for (i = 0; i < j; i++)
    {
      tsum = data_buf[i * 2];
      tsum = tsum << 8;
      tsum += data_buf[i * 2 + 1];
      lsum += tsum;
    }
   if (len % 2)
    {
      tsum = data_buf[i * 2];
      lsum += (tsum << 8);
    }
    sum = (uint16_t)lsum;
    sum = ~(sum + (lsum >> 16));
  return sum;

}

uint16_t checksum6(uint8_t * data_buf, uint16_t len)

{
  uint32_t sum = 0;
  uint16_t data;

  #if 0
  printf("len = %d\n", len);
  #if 1
  // 20230731 taylor
  uint32_t j;
  for(j=0; j<len; j++)
  {
    printf("data_buf[%d] = 0x%x\r\n", j, data_buf[j]);
  }
  printf("\r\n\r\n");
  #endif
  #endif

  for (int i = 0; i < len; i+=2)
  {
    data = data_buf[i]<<8 | data_buf[i+1];

    #if 0
    printf("before sum = %x\n", sum);
    printf("data[%d] = %x\n", i, data);
    #endif
    sum += data;
    #if 0
    printf("after sum = %x\n\n", sum);
    #endif
  }
  
  while (sum >> 16)
  {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  #if 0
  printf("sum = %x\n", sum);
  printf("~sum = %x\n", ~sum);
  #endif
  
  return ~sum;
}

uint16_t htons( uint16_t hostshort)
{

#if 1
  //#ifdef LITTLE_ENDIAN
	uint16_t netshort=0;
	netshort = (hostshort & 0xFF) << 8;

	netshort |= ((hostshort >> 8)& 0xFF);
	return netshort;

#else
	return hostshort;
#endif

}


