#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

void SPI4_MY_INIT(void);

int main(void)
{
  //UART init
  UartStdOutInit();
  printf("chip1:test start\r\n");

  SPI4_MY_INIT();
	
	int i = 1000;
	while(i--);
	
	while(1)
	{	
		SPI2->DR = 0XAA;
		SPI4->DR = 0X55;


		while (!(SPI4->SR & SPI_SR_RXNE));
		while (SPI4->SR & SPI_SR_BSY);
		
		printf("chip1:SPI4->DR = %d\r\n",SPI4->DR);
		
		for(int i = 99;i < 0;i--)
		{
			__NOP();
		}
	}
}


void SPI4_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_SPI4);


  SPI4->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 ;

  SPI4->CR2 |= ((uint16_t)0x0010);
	
	SPI4->CR1 |= SPI_CR1_SPE;
}
