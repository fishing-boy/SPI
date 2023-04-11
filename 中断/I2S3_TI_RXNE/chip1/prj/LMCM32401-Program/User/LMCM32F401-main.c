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
	printf("chip1:i2s rxne test\r\n");

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);  

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_Init(GPIOA,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 

  /* Enable SPI clocks */
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  /* Configure I2S2 interface */
  I2S3->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S2
  I2S3->I2SCFGR = 0;                  // Reset I2S2 config

  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S2 mode
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S2 master transmitter
  //I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S2 standard Philips I²S

  I2S3->I2SPR = 0;     // Reset I2S2 prescaler
  I2S3->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S3->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler
	
	int i = 10000;
		
	while(i--);

  //I2S3->CR2 |= SPI_CR2_TXEIE;
	I2S3->I2SCFGR |= SPI_I2SCFGR_I2SE;
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	//NVIC_Init(&NVIC_InitStructure);

  while(1)
  {
    while(!(I2S3->SR & SPI_SR_TXE));
		
		I2S3->DR = 0Xaa;
  }
}

void SPI3_IRQHandler(void)
{
	//if((SPI3->SR & ((uint16_t)0x0002)) != (uint16_t)RESET)//检测中断标志位
	if(SPI_I2S_GetITStatus(I2S3,SPI_I2S_IT_TXE) != (uint16_t)RESET)
  {	
		int i = 10000;
		while(i--);
		I2S3->DR = 0Xaa;
  }
}
