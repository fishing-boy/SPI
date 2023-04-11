#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;
SPI_TypeDef *I2S3 = SPI3;

/* Buffer for transmitting data */
uint16_t tx_buffer[2] = {0x1234, 0x5678};

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("chip1:i2s FRE test\r\n");

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_12; 
  GPIO_Init(GPIOC,&GPIO_InitStructure); 

  /* Enable SPI clocks */
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  /* Configure I2S2 interface */
  I2S2->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S2
  I2S2->I2SCFGR = 0;                  // Reset I2S2 config

  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S2 mode
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S2 master transmitter
  //I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S2 standard Philips I²S

  I2S2->I2SPR = 0;     // Reset I2S2 prescaler
  I2S2->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

   /* Configure I2S3 interface */
  I2S3->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S3
  I2S3->I2SCFGR = 0;                  // Reset I2S3 config

  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S3 mode
  //I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S3 slave receiver
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S3 standard Philips I²S

  I2S3->I2SPR = 0;     // Reset I2S3 prescaler
  I2S3->I2SPR |= 39;   // I2S3 prescaler = 40
  I2S3->I2SPR |= SPI_I2SPR_ODD;  // I2S3 odd prescaler

  /* Enable I2S2 and I2S3 */
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SE;
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SE;

  SPI3->CR2 |= SPI_CR2_RXNEIE;
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);

  /* Loop to transmit data */
  while(1)
  {
    /* Wait for I2S2 to be ready for data */
    while(!(I2S2->SR & SPI_SR_TXE));

    /* Send data over I2S2 */
    I2S2->DR = tx_buffer[0];
		
		int i = 1000;
		while(i--);

//    /* Wait for I2S3 to receive data */
//    while(!(I2S3->SR & SPI_SR_RXNE));

//    /* Read data from I2S3 */
//    uint16_t rx_data = I2S3->DR;
  }
}

void SPI3_IRQHandler(void)
{
	if((SPI3->SR & ((uint16_t)0x0001)) != (uint16_t)RESET)//检测中断标志位
  {
    SPI3->CR2 &= ~SPI_CR2_RXNEIE;
		printf("test pass\r\n");
  }
}
