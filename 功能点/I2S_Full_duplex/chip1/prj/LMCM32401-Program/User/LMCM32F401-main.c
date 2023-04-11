#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;

/* Buffer for transmitting data */
uint16_t tx_buffer[2] = {0x1234, 0x5678};

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("chip1:i2s full-duplex test\r\n");

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;

//  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_SPI2);  
//  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);  
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);  
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF_I2S2ext);  

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_I2S2ext);  

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3; 
//  GPIO_Init(GPIOC,&GPIO_InitStructure); 

  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  I2S2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
  I2S2->I2SCFGR = 0;

  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1;
  //I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0;

  I2S2->I2SPR = 0;     // Reset I2S2 prescaler
  I2S2->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

  I2S2ext->I2SCFGR &= ~SPI_I2SCFGR_I2SE; 
  I2S2ext->I2SCFGR = 0;
  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0;

  I2S2ext->I2SPR = 0;
  I2S2ext->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2ext->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SE;
  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SE;

  I2S2->CR2 |= SPI_CR2_RXNEIE;
  I2S2ext->CR2 |= SPI_CR2_RXNEIE;
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);

  while(1)
  {
    while((I2S2->SR & SPI_SR_BSY));
    while(!(I2S2->SR & SPI_SR_TXE));
    I2S2->DR = tx_buffer[0];

    while(!(I2S2->SR & SPI_SR_TXE));
    while((I2S2->SR & SPI_SR_BSY));
		I2S2->DR = tx_buffer[1];
		
		int i = 1000;
		while(i--);
  }
}

void SPI2_IRQHandler(void)
{
	if((I2S2ext->SR & ((uint16_t)0x0001)) != (uint16_t)RESET)//检测中断标志位
  {
    //printf("rxdata:0x%08x\r\n",I2S2ext->DR);
		int i = I2S2ext->DR;
  }
}
