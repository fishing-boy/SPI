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
  printf("chip2:test start\r\n");

  SPI1_MY_INIT();
  //SPI2_MY_INIT();
	
	while(1)
	{	
		//SPI2->DR = 0XAA;
		SPI1->DR = 0X55;


		while (!(SPI1->SR & SPI_SR_RXNE));
		while (SPI1->SR & SPI_SR_BSY);
		
		printf("chip2:SPI1->DR = %d\r\n",SPI1->DR);
		
		for(int i = 99;i < 0;i--)
		{
			__NOP();
		}
	}
}

void SPI1_IRQHandler(void)
{
	if (SET == SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE))
	{
		;
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
#endif
#ifndef MCU_TEST
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
#endif
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;         // SPI1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
	//NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器

  SPI1->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

  SPI1->CR2 |= ((uint16_t)0x0010);
	
	SPI1->CR1 |= SPI_CR1_SPE;
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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9,GPIO_AF_SPI2);
#endif
  // Configure SPI1 as master mode, CPOL = 0, CPHA = 0, 8-bit data frame
  SPI2->CR1 =  SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

  SPI2->CR2 |= ((uint16_t)0x0010);
	
	SPI2->CR1 |= SPI_CR1_SPE;
}
