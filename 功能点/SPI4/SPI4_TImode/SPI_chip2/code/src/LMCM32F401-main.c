#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

void SPI4_MY_INIT(void);

int main(void)
{
  int num = 0;

  UartStdOutInit();
  printf("chip2:test start\r\n");

  SPI4_MY_INIT();
	
	while(num<10)
	{
		SPI4->DR = 0X55;

		while (!(SPI4->SR & SPI_SR_RXNE));
		while (SPI4->SR & SPI_SR_BSY);
		
		printf("chip2:SPI4->DR = %d\r\n",SPI4->DR);
    num++;

		for(int i = 99;i < 0;i--)
		{
			__NOP();
		}
	}

  printf("chip2:test over\r\n");

  RCC->APB2RSTR |= RCC_APB2RSTR_SPI4RST;

	int	i = 1000;
  while(i--);

  RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI4RST;

  printf("chip2:Reset SPI4\r\n");

  printf("chip2:SPI4->SR = 0x%08x\r\n",SPI4->SR);
  printf("chip2:SPI4->CR1 = 0x%08x\r\nchip2:SPI4->CR2 = 0x%08x\r\nchip2:SPI4->CRCPR = 0x%08x\r\nchip2:SPI4->DR = 0x%08x\r\n",SPI4->CR1,SPI4->CR2,SPI4->CRCPR,SPI4->DR);
	printf("chip2:SPI4->I2SCFGR = 0x%08x\r\nchip2:SPI4->I2SPR = 0x%08x\r\nchip2:SPI4->RXCRCR = 0x%08x\r\nchip2:SPI4->TXCRCR = 0x%08x\r\n",SPI4->I2SCFGR,SPI4->I2SPR,SPI4->RXCRCR,SPI4->TXCRCR);

  while(1);
}

void SPI4_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable GPIOA and SPI4 peripheral clock
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

  SPI4->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

  SPI4->CR2 |= ((uint16_t)0x0010);
	
	SPI4->CR1 |= SPI_CR1_SPE;
}
