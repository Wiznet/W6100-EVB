# W6100 EVB
- W6100 chip development platform for net-enabled microcontroller applications
	- Ethernet (W6100 Hardwired TCP/IP chip) and 32-bit ARM® Cortex™-M3 based designs
	- Arduino Pin-compatible platform hardware.

[link-w6100-evb]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/w6100-evb.png

For more details, please refer to [W6100-EVB page](https://docs.wiznet.io/Product/iEthernet/W6100/w6100-evb) in WIZnet Docs page.

## Features
- WIZnet W6100 Hardwired TCP/IP chip
  - Hardwired TCP/IP embedded Ethernet controller
  - Parallel Host Interface (External BUS Interface)
  - SPI (Serial Peripheral Interface) Microcontroller Interface
  - Hardwired TCP/IP stack supports TCP, UDP, IPv6, IPv4, ICMPv6, ICMPv4, IGMP, MLDv1, ARP, PPPoE protocols
  - Easy to implement of the other network protocols
  
- STMicroelectronics STM32F103VCT6 MCU
  - 32-bit ARM® Cortex™-M3 microcontroller running at up to 72MHz
  - 256kB on-chip flash / 48kB on-chip SRAM / Various peripherals
- Pin-compatible with Arduino Shields designed for the UNO Rev3
- 2 x Push button switch(SW), 1 x RGB LED
- 1-Channel 10/100Mbps Ethernet Connector (RJ-45 with transformer) – (If you want to RJ-45, Contact us)
- Micro USB B connector and 5-pin(2.54mm) Cortex debug connector for SWD(Serial Wire Debug)

- W6100-EVB Arduino Compatible Pinout
![W6100-EVB Arduino Compatible Pinout](https://docs.wiznet.io/assets/images/w6100-evb_pinout_1-09e4d4e3d62eebce50b7fcbddf327a17.png "W6100-EVB Arduino Compatible Pinout")

- W6100-EVB External Pinout(left)
![W6100-EVB External Pinout(left)](https://docs.wiznet.io/assets/images/w6100-evb_pinout_2-948a788361e5a216464b9f97c5721514.png "W6100-EVB External Pinout(left)")

- W6100-EVB External Pinout(Right)
![W6100-EVB External Pinout(Right)](https://docs.wiznet.io/assets/images/w6100-evb_pinout_3-fab1afd733476ec35ba6031eff59659d.png "W6100-EVB External Pinout(Right)")

## Software
These are libraries source code and example projects based on STM32CubeIDE
If you want to base on TrueStudio or Ecplipse, change git branch master to truestudio_ver
- W6100-EVB TrueStudio or Ecplipse Git
 [W6100-EVB TrueStudio or Ecplipse Git](https://github.com/Wiznet/W6100-EVB/tree/truestudio_ver)

The tree of Directory is below.

```1
W6100-EVB
    ┣ W6100_EVB_FSMC_DMA or W6100_EVB_SPI_DMA
    ┣ Core
    ┃  ┣ Inc
    ┃  ┃  ┣ AddressAutoConfig.h
    ┃  ┃  ┣ board_init.h
    ┃  ┃  ┣ main.h
    ┃  ┃  ┣ msgq.h
    ┃  ┃  ┣ stm32f1xx_hal_conf.h
    ┃  ┃  ┣ stm32f1xx_it.h
    ┃  ┃  ┗ wizchip_init.h
    ┃  ┣ Src
    ┃  ┃  ┣ AddressAutoConfig.c
    ┃  ┃  ┣ board_init.c
    ┃  ┃  ┣ main.c
    ┃  ┃  ┣ msgq.c
    ┃  ┃  ┣ stm32f1xx_hal_msp.c
    ┃  ┃  ┣ stm32f1xx_it.c
    ┃  ┃  ┣ syscalls.c
    ┃  ┃  ┣ sysmem.c
    ┃  ┃  ┣ system_stm32f1xx.c
    ┃  ┃  ┗ wizchip_init.c
    ┃  ┗ Startup
    ┗ Drivers
       ┣ CMSIS
       ┣ STM32F1xx_HAL_Driver
       ┗ io6Library
          ┣ Application
          ┣ Ethernet
          ┣ Doxygen
          ┗ Internet

```

### io6Library GitHub Repository
- [io6Library](https://github.com/Wiznet/io6Library) : Latest WIZnet chip drivers and protocol libraries for developers

## Hardware material, Documents and Others
Refer to this link [W6100 Hardware data](https://github.com/Wiznet/Hardware-Files-of-WIZnet/tree/master/02_iEthernet/W6100/W6100-EVB_V110) 
- Documents
  - Getting Started: Hello world! / Downloading a new program
  - Make New W6100-EVB Projects
- Technical Reference
  - Datasheet
  - Schematic
  - Partlist
  - Demension
- See also



## Revision History
- 0.0.0.1       : Dec. 2022 (First release)
