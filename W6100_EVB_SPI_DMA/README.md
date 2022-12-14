# How to Test Loopback Example


## Step 1: Prepare software

The following serial terminal programs are required for Loopback example test, download and install from below links.

- [**ST Flash loader demonstrator**][link-st-flash]
- [**Tera Term**][link-tera_term]
- [**Script Communicator**][link-ScriptCommunicator]


## Step 2: Prepare hardware

1. Connect ethernet cable to W6100-EVB ethernet port.

2. Connect W6100-EVB to desktop or laptop using 5 pin micro USB cable.


## Step 3: Setup Loopback Example

To test the Loopback example, minor settings shall be done in code.

1. If you want to test with the Loopback example using SPI, Set Mode switch [0 0]

![][link-mode-sw-spi]

And if you want to using DMA, defined comment in main.h in 'W6100_EVB_SPI_DMA/Core/Inc/' directory.

```cpp
//#define DMA 
```

2. Setup network configuration such as IP in 'main.c' which is the Loopback example in 'W6100_EVB_SPI_DMA/Core/Src/' directory.
 If you use Auto Address Configuration, do not remove this comment

```cpp
/* Network */
wiz_NetInfo gWIZNETINFO = { .mac = {
								0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
							},
							.ip = {
								192, 168, 111, 107
							},
							.sn = {
								255, 255, 255, 0
							},
							.gw = {
								192, 168, 11, 1
							},
							.dns = {
								8, 8, 8, 8
							},
							.lla={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.gua={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.sn6={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							},
							.gw6={
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0
							}
};

```

```cpp
  /* Address Auto Configuration */
  	if(1 != AddressAutoConfig_Init(&gWIZNETINFO))
  	{
  		// Manual Set IPv6
  		gWIZNETINFO = gWIZNETINFO_M;
  		ctlnetwork(CN_SET_NETINFO, &gWIZNETINFO);
  	}

```

And if you use static network, configure this comment. and remove AddressAutoConfig_Init

```cpp
wiz_NetInfo gWIZNETINFO_M = { .mac = {0x00,0x08,0xdc,0xFF,0xFF,0xFF},
							.ip = {192,168,11,107},
							.sn = {255, 255, 255, 0},
							.gw = {192, 168, 11, 1},
							.dns = {8, 8, 8, 8},
							//.dhcp = NETINFO_STATIC,
							.lla={
									0x00,0x00, 0x00,0x00,
									0x00,0x00, 0x00,0x00,
								  },   ///< Source Link Local Address

							.gua={0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00},   ///< Source Global Unicast Address
							.sn6={0xff,0xff,0xff,0xff,
									0xff,0xff,0xff,0xff,
									0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00 },   ///< IPv6 Prefix
							.gw6={0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00}   ///< Gateway IPv6 Address
};

```
and add this.

```cpp

  	// Manual Set IPv6
  	gWIZNETINFO = gWIZNETINFO_M;
  	ctlnetwork(CN_SET_NETINFO, &gWIZNETINFO);
  	
```


3. Setup loopback Port configuration in 'main.c'.

```cpp
/* Port */
  while (1)
  {
	   	loopback_udps(0,ethBuf0,50000,AS_IPV4);
		loopback_tcps(1,ethBuf3,50003,AS_IPV4);
		loopback_tcps(2,ethBuf4,50004,AS_IPV6);
		loopback_tcps(3,ethBuf5,50005,AS_IPDUAL);
  }
```


## Step 4: Build

1. After completing the Loopback example configuration, mouse right click to Project name and selsect 'Build Project'

![][link-build-project]

2. When the build is completed, 'W6100_EVB_SPI_DMA.hex' is generated in 'W6100_EVB_SPI_DMA\Debug' or W6100_EVB_SPI_DMA\Release' directory.


## Step 5: Upload and Run
# Flash programming via UART
STMicroelectronics Flash loader demonstrator is a program to perform in-system programming (ISP) of the MCU flash via its UART.

1. Press the 'Boot0' push button switch on board until turn on the board after reset or power supply.

2. Set the settings on main window of Flash loader demonstrator program. The figure below shows the default configuration for W6100-EVB

![][link-flash_down]

3. refer to below pictures If you do not go to the next page in flash loader demonstrator, users try again this action. 'Press the 'Boot0' push button switch on board until turn on the board after reset or power supply.'

![][link-flash_down2]

4. After finish to flash programming and board reset, The MCU do running the program When the running to loopback program, as below picture serial debug message print out.

![][link-terminal-start]

5. Connect to the open loopback server using ScriptCommunicator TCP, UDP IPv4 or IPv6 client. When connecting to the loopback server, you need to enter is the IP that was configured in Step 3, default is UDP:50000, TCP IPv4:50003, TCP IPv6:50004, TCP IPv4+IPv6:55000

You have got IP information in start message in terminal
![][link-ip-infomation]

 - Run ScriptCommunicator and set sockets for IPv4 connection. And send a message to the W6100EVB. Then the W6100EVB reply with message you've sent.

![][link-connect-to-ipv4]

 - Run ScriptCommunicator and set sockets for IPv6 connection. And send a message to the W6100EVB. Then the W6100EVB reply with message you've sent.

![][link-connect-to-ipv6]

 - Test packet capture

![][link-capture]



<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
[link-st-flash]: https://www.st.com/en/development-tools/flasher-stm32.html
[link-ScriptCommunicator]: https://sourceforge.net/projects/scriptcommunicator/
[link-mode-sw-spi]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/mode-sw-spi.png
[link-build-project]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/build-project.png
[link-flash_down]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/flash_down.png
[link-flash_down2]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/flash_down2.png
[link-terminal-start]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/terminal-start.png
[link-ip-infomation]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/ip-infomation.png
[link-connect-to-ipv4]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/connect-to-ipv4.png
[link-connect-to-ipv6]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/connect-to-ipv6.png
[link-capture]: https://github.com/Wiznet/W6100-EVB/blob/master/static/images/capture.png
