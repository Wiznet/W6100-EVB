/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "loopback.h"
#include "board_init.h"
#include "AddressAutoConfig.h"
#if 1
// 20230726 taylor
#include "ping.h"
#endif
#if 1
// 20230727 taylor
#include "macraw.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 20230725 taylor
#if 1
#define TEST_MACRAW
#endif
// 20230727 taylor
#if 0
#define TEST_IPRAW
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
wiz_NetInfo gWIZNETINFO = { .mac = {
								0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
							},
							#if 1
              // 20230726 taylor
							.ip = {
								192, 168, 50, 162
							},
              #else
							.ip = {
								192, 168, 111, 107
							},
							#endif
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

wiz_NetInfo gWIZNETINFO_M = { .mac = {0x00,0x08,0xdc,0xFF,0xFF,0xFF},
#if 1
// 20230726 taylor
              .ip = {192,168,50,162},
#else
							.ip = {192,168,11,107},
#endif
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

uint8_t WIZ_Dest_IP_virtual[4] = {192, 168, 0, 230};               //DST_IP Address
uint8_t WIZ_Dest_IP_Google[4]  = {216, 58, 200, 174};              //DST_IP Address

uint8_t mcastipv4_0[4] ={239,1,2,3};
uint8_t mcastipv4_1[4] ={239,1,2,4};
uint8_t mcastipv4_2[4] ={239,1,2,5};
uint8_t mcastipv4_3[4] ={239,1,2,6};

uint16_t WIZ_Dest_PORT = 15000;                                 //DST_IP port

#define ETH_MAX_BUF_SIZE	1024

uint8_t  remote_ip[4] = {192,168,177,200};                      //
uint16_t remote_port = 8080;

unsigned char ethBuf0[ETH_MAX_BUF_SIZE];
unsigned char ethBuf1[ETH_MAX_BUF_SIZE];
unsigned char ethBuf2[ETH_MAX_BUF_SIZE];
unsigned char ethBuf3[ETH_MAX_BUF_SIZE];
unsigned char ethBuf4[ETH_MAX_BUF_SIZE];
unsigned char ethBuf5[ETH_MAX_BUF_SIZE];
unsigned char ethBuf6[ETH_MAX_BUF_SIZE];
unsigned char ethBuf7[ETH_MAX_BUF_SIZE];

uint8_t bLoopback = 1;
uint8_t bRandomPacket = 0;
uint8_t bAnyPacket = 0;
uint16_t pack_size = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void print_network_information(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	volatile int i;
	volatile int j,k;
 	uint16_t ver=0;
 	uint16_t curr_time = 0;
	uint8_t syslock = SYS_NET_LOCK;
  #if 0
  // 20230726 taylor
  uint8_t svr_ipv4[4] = {192, 168, 50, 162};
  #else
	uint8_t svr_ipv4[4] = {192, 168, 177, 235};
  #endif
	uint8_t svr_ipv6[16] = {0xfe, 0x80, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00,
							0xc1, 0x0b, 0x0a, 0xdf,
							0xea, 0xf4, 0xf4, 0x2d};

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("\r\n system start \r\n");
#if 1
  printf("Compiled @ %s %s\r\n", __DATE__, __TIME__);
#endif

  W6100Initialze();

  ctlwizchip(CW_SYS_UNLOCK,& syslock);
  ctlnetwork(CN_SET_NETINFO,&gWIZNETINFO);

  printf("Register value after W6100 initialize!\r\n");

  #if 0
  // 20230726 taylor
  #else
  /* Address Auto Configuration */
  	if(1 != AddressAutoConfig_Init(&gWIZNETINFO))
  	{
  		// Manual Set IPv6
  		gWIZNETINFO = gWIZNETINFO_M;
  		ctlnetwork(CN_SET_NETINFO, &gWIZNETINFO);
  	}
  #endif


  print_network_information();

  #ifdef TEST_MACRAW

  printf("Start TEST_MACRAW\r\n");

  while(1)
  {
    uint16_t rxlen;
    uint16_t len;
    
    rxlen = loopback_macraw(0, ethBuf0);
    if(rxlen > 0)
    {
      printf("rxlen = %d\r\n", rxlen);

      uint32_t i;

      printf("dst MAC : ");
      for(i=0; i<6; i++)
      {
        printf("%x", ethBuf0[i]);
        
        if(i!=5)
        {
          printf(":");
        }
      }
      printf("\r\n");

      printf("src MAC : ");
      for(i=6; i<12; i++)
      {
        printf("%x", ethBuf0[i]);
        
        if(i!=11)
        {
          printf(":");
        }
      }
      printf("\r\n");

      printf("Type : %x %x\r\n", ethBuf0[12], ethBuf0[13]);

      if(ethBuf0[12] == 0x08 && ethBuf0[13] == 0x00)
      {
        printf("Type IPv4\r\n");

        #if 0
        ethBuf0[14];ethBuf0[15];
        
        ethBuf0[16];ethBuf0[17];ethBuf0[18];ethBuf0[19];
        ethBuf0[20];ethBuf0[21];ethBuf0[22];ethBuf0[23];

        ethBuf0[24];ethBuf0[25];
        #endif

        printf("src IP : %d.%d.%d.%d\r\n", ethBuf0[26],ethBuf0[27],ethBuf0[28],ethBuf0[29]);
        printf("dst IP : %d.%d.%d.%d\r\n", ethBuf0[30],ethBuf0[31],ethBuf0[32],ethBuf0[33]);
      }
      else if(ethBuf0[12] == 0x86 && ethBuf0[13] == 0xDD)
      {
        printf("Type IPv6\r\n");

        #if 0
        ethBuf0[14];ethBuf0[15];
        
        ethBuf0[16];ethBuf0[17];ethBuf0[18];ethBuf0[19];
        ethBuf0[20];ethBuf0[21];
        #endif

        printf("src IPv6 : ");
        for(i=22; i<38; i++)
        {
          printf("%x", ethBuf0[i]);
          
          if(i!=37)
          {
            printf(":");
          }
        }
        printf("\r\n");

        printf("dst IPv6 : ");
        for(i=38; i<54; i++)
        {
          printf("%x", ethBuf0[i]);
          
          if(i!=53)
          {
            printf(":");
          }
        }
        printf("\r\n");
      }
      else
      {
        printf("Type unknown\r\n");
#if 0
        printf("payload : ");
        for(i=14; i<rxlen; i++)
        {
          printf("0x%x ", ethBuf0[i]);
        }
#endif
        printf("\r\n");
      }

      
      
      printf("\r\n\r\n");
    }
  }
  
  #endif
  
  // 20230725 IPRAW
  #ifdef TEST_IPRAW
  // https://wizconfluence.atlassian.net/wiki/spaces/TP/pages/279375/IP01-+ICMP+TEST

  printf("Start TEST_IPRAW\r\n");

  // ICMP6 58
  // ICMP 1

  #if 1
  // IPv6
  //2001:470:1f04:6ff:64bf:3d06:f6ab:b9f5
  // fe80::768e:1eaa:4b74:b98e
  uint8_t pDestaddr[16] = {
  #if 1

  #endif
    }; //Destination Device IP

  //request_flag = 0;   //Send Request ping from outside to Ethernet Chip (W5100S)
  uint8_t request_flag = 1;   //Send Request ping from Ethernet CHIP(W5100S) to outside.

  uint32_t return_len = ping6_auto(0, pDestaddr, request_flag);
  #else
  uint8_t pDestaddr[4] = {192, 168, 50, 152}; //Destination Device IP

  //request_flag = 0;   //Send Request ping from outside to Ethernet Chip (W5100S)
  uint8_t request_flag = 1;   //Send Request ping from Ethernet CHIP(W5100S) to outside.

  uint32_t return_len = ping_auto(0, pDestaddr, request_flag);
  #endif

  printf("\r\n ping_auto return value: %d", return_len);

  printf("\t>Done TEST_IPRAW\r\n");
  #endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  #if 0
    loopback_udps(0,ethBuf0,50000,AS_IPV4);
		loopback_tcps(1,ethBuf3,50003,AS_IPV4);
		loopback_tcps(2,ethBuf4,50004,AS_IPV6);
		loopback_tcps(3,ethBuf5,50005,AS_IPDUAL);
  #endif

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, W6100_RESET_Pin|W6100_SPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : W6100_RESET_Pin W6100_SPI_CS_Pin */
  GPIO_InitStruct.Pin = W6100_RESET_Pin|W6100_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* UART */
int _write(int fd, char *str, int len)
{
	for(int i=0; i<len; i++)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)&str[i], 1, 0xFFFF);
	}
	return len;
}

void print_network_information(void)
{
	wizchip_getnetinfo(&gWIZNETINFO);

	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("IP address : %d.%d.%d.%d\n\r",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("SM Mask    : %d.%d.%d.%d\n\r",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);

	print_ipv6_addr("GW6 ", gWIZNETINFO.gw6);
	print_ipv6_addr("LLA ", gWIZNETINFO.lla);
	print_ipv6_addr("GUA ", gWIZNETINFO.gua);
	print_ipv6_addr("SUB6", gWIZNETINFO.sn6);

	printf("\r\nNETCFGLOCK : %x\r\n", getNETLCKR());
}

void print_ipv6_addr(uint8_t* name, uint8_t* ip6addr)
{
	printf("%s : ", name);
	printf("%04X:%04X", ((uint16_t)ip6addr[0] << 8) | ((uint16_t)ip6addr[1]), ((uint16_t)ip6addr[2] << 8) | ((uint16_t)ip6addr[3]));
	printf(":%04X:%04X", ((uint16_t)ip6addr[4] << 8) | ((uint16_t)ip6addr[5]), ((uint16_t)ip6addr[6] << 8) | ((uint16_t)ip6addr[7]));
	printf(":%04X:%04X", ((uint16_t)ip6addr[8] << 8) | ((uint16_t)ip6addr[9]), ((uint16_t)ip6addr[10] << 8) | ((uint16_t)ip6addr[11]));
	printf(":%04X:%04X\r\n", ((uint16_t)ip6addr[12] << 8) | ((uint16_t)ip6addr[13]), ((uint16_t)ip6addr[14] << 8) | ((uint16_t)ip6addr[15]));
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
