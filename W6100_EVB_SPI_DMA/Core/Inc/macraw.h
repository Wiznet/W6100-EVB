#ifndef MACRAW_H
#define MACRAW_H

#include "wizchip_conf.h"
#include "socket.h"
#include "w6100.h"

#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE   2048
#endif

int32_t loopback_macraw(uint8_t sn, uint8_t* buf);


#endif
