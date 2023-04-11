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
	printf("i2s rxne test\r\n");

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_SPI2);   

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6; 
  GPIO_Init(GPIOC,&GPIO_InitStructure); 

  /* Enable SPI clocks */
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  /* Configure I2S2 interface */
  I2S2->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S2
  I2S2->I2SCFGR = 0;                  // Reset I2S2 config

  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S2 mode
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S2 master transmitter
  //I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S2 standard Philips I²S
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SSTD_0;
  I2S2->I2SCFGR |= SPI_I2SCFGR_CHLEN;
	I2S2->I2SCFGR |= SPI_I2SCFGR_DATLEN_0;

  I2S2->I2SPR = 0;     // Reset I2S2 prescaler
  I2S2->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler
  I2S2->I2SPR |= SPI_I2SPR_MCKOE;

  /* Enable I2S2 and I2S3 */
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SE;

  while(1)
  {
    while(!(I2S2->SR & SPI_SR_TXE));

    /* Send data over I2S2 */
    I2S2->DR = 0xaa12;

    while(!(I2S2->SR & SPI_SR_TXE));
		I2S2->DR = 0x3456;
		
		int i = 100;
		while(i--);
  }
}

