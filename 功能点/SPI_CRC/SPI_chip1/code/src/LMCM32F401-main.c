#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

//#define MCU_TEST

void SPI1_MY_INIT(void);
void SPI2_MY_INIT(void);

int main(void)
{
  //UART init
  UartStdOutInit();
  printf("chip1:test start\r\n");

  SPI1_MY_INIT();
  SPI2_MY_INIT();

  // Send data
	SPI2->DR = 0xAB;
  SPI1->DR = 0xAB;
	
	SPI2->CR1 |= SPI_CR1_CRCNEXT;
	SPI1->CR1 |= SPI_CR1_CRCNEXT;
	//SPI2->DR = 0xAB;
  
  // Wait for transmission complete
  while (!(SPI1->SR & SPI_SR_RXNE));
  //while (SPI1->SR & SPI_SR_BSY);

  printf("chip1:SPI1->DR = %d\r\n",SPI1->DR);

  // Read the CRC value
  uint16_t crc = SPI1->RXCRCR;
  //printf("SPI1->RXCRCR = %d\r\n",SPI1->RXCRCR);
  //printf("SPI2->RXCRCR = %d\r\n",SPI2->RXCRCR);

  // Compare with the expected CRC value
  if (crc == 0x58) 
  {
    printf("chip1:crc true,crc = %d\r\n",crc);
  } else
  {
    printf("chip1:crc fail,crc = %d\r\n",crc);
  }

  while(1);
}

void SPI1_IRQHandler(void)
{
	if (SET == SPI_I2S_GetITStatus(SPI1, SPI_IT_CRCERR))
	{
		printf("chip1:SPI1 CRCERR TEST PASS \r\n");
		SPI_I2S_ClearITPendingBit(SPI1, SPI_IT_CRCERR);
	}
}

void SPI1_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  // Enable GPIOA and SPI1 peripheral clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
#ifdef MCU_TEST
  /* Configure GPIO pins for SPI1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
#endif
#ifndef MCU_TEST
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
#endif
  // Configure PA5, PA6, PA7 as alternate function mode
  //GPIOA->MODER |= GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
  //GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos) | (5 << GPIO_AFRL_AFSEL7_Pos);

	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, ENABLE);
	SPI_I2S_ClearITPendingBit(SPI1, SPI_IT_CRCERR);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;         // SPI1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器

  // Configure SPI1 as master mode, CPOL = 0, CPHA = 0, 8-bit data frame
  SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;

  // Enable CRC calculation
  SPI1->CR1 |= SPI_CR1_CRCEN;

  // Set the polynomial for the CRC calculation
  SPI1->CRCPR = 7; // x^2 + x + 1
}

void SPI2_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable GPIOA and SPI1 peripheral clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

#ifdef MCU_TEST
  /* Configure GPIO pins for SPI1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_SPI2);
#endif

  // Configure PA5, PA6, PA7 as alternate function mode
  //GPIOA->MODER |= GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
  //GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos) | (5 << GPIO_AFRL_AFSEL7_Pos);

  // Configure SPI1 as master mode, CPOL = 0, CPHA = 0, 8-bit data frame
  SPI2->CR1 =  SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SPE;

  // Enable CRC calculation
  SPI2->CR1 |= SPI_CR1_CRCEN;

  // Set the polynomial for the CRC calculation
  SPI2->CRCPR = 7; // x^2 + x + 1

  // Send data
  //SPI2->DR = 0xAB;
}
















// #include "stm32f4xx.h"
// #include "spi_crc.h"

// void SPI_CRC_Init(void) {
//   GPIO_InitTypeDef GPIO_InitStructure;
//   SPI_InitTypeDef SPI_InitStructure;
//   CRC_InitTypeDef CRC_InitStructure;

//   /* Enable GPIO clocks */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

//   /* Enable SPI2 and CRC clocks */
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

//   /* Configure GPIO pins for SPI2 */
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//   GPIO_Init(GPIOB, &GPIO_InitStructure);

//   /* Connect SPI2 pins to AF */
//   GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
//   GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
//   GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

//   /* Configure SPI2 */
//   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
//   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//   SPI_InitStructure.SPI_CRCPolynomial = 
//   SPI_Init(SPI2, &SPI_InitStructure);

//   /* Enable CRC calculation */
//   CRC_InitStructure.CRC_Mode = CRC_Mode_8Bit;
//   CRC_InitStructure.CRC_Polynomial = 0x07;
//   CRC_Init(&CRC_InitStructure);

//   /* Enable SPI2 */
//   SPI_Cmd(SPI2, ENABLE);
// }

// uint8_t SPI_CRC_TransmitReceive(uint8_t data) {
//   /* Wait for TXE flag */
//   while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

//   /* Send data */
//   SPI_I2S_SendData(SPI2, data);

//   /* Wait for RXNE flag */
//   while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

//   /* Receive data */
//   uint8_t receivedData = SPI_I2S_ReceiveData(SPI2);

//   /* Calculate CRC on received data */
//   uint8_t crc = CRC_CalcCRC(receivedData);

//   /* Return CRC value */
//   return crc;
// }

// uint8_t SPI_CRC_Transmit(uint8_t data) {
//   /* Wait for TXE flag */
//   while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

//   /* Send data */
//   SPI_I2S_SendData(SPI2, data);

//   /* Wait for RXNE flag */
//   while (SPI
