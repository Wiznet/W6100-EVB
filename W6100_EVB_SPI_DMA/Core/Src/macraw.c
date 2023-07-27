#include "macraw.h"
#include "stdio.h"

int32_t loopback_macraw(uint8_t sn, uint8_t* buf)
{
  int32_t  ret;
  uint16_t size, sentsize;
  uint8_t  destip[4];
  uint16_t destport;
  uint8_t port = 5000;
  uint8_t count=0;
  uint8_t head[2];
  uint16_t pack_len = 0;
  
  switch(getSn_SR(sn))
  {
    case SOCK_MACRAW :
      if((pack_len = getSn_RX_RSR(sn)) > 0)
      {
        if(pack_len > DATA_BUF_SIZE)
        {
          pack_len = DATA_BUF_SIZE;
        }
        printf("pack_len = %d\r\n", pack_len);

#if 1
        wiz_recv_data(sn, head, 2);
        setSn_CR(sn, Sn_CR_RECV);

        // byte size of data packet (2byte)
        pack_len = head[0];
        pack_len = (pack_len << 8) + head[1];
        pack_len -= 2;
#if 0
        if (pack_len > len)
        {
            // Packet is bigger than buffer - drop the packet
            wiz_recv_ignore(sn, pack_len);
            setSn_CR(sn, Sn_CR_RECV);
            return 0;
        }
#endif
        wiz_recv_data(sn, buf, pack_len); // data copy
        setSn_CR(sn, Sn_CR_RECV);
        return pack_len;
#else
        
        ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport, 4);
        if(ret < 0)
        {
          printf("failed ret = %d\r\n", ret);
        }
        else
        {
          printf("recv ret = %d\r\n", ret);
          return ret;
        }
#endif
      }
    break;
      
    case SOCK_CLOSED:
      ret = socket(sn, Sn_MR_MACRAW, port, 0x00);
      printf("ret = %d\r\n", ret);
      return ret;
    break;
      
    default :
    break;
  }
  return 0;
}


