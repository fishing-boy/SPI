#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

//#define MCU_TEST

void SPI4_MY_INIT(void);

int main(void)
{
  //UART init
  UartStdOutInit();
  printf("chip2:test start\r\n");

  SPI4_MY_INIT();
	
	while(1)
	{
		SPI4->CR1 |= SPI_CR1_CRCEN;
		
		int value = 1;

    for(int i=10;i>0;i--)
    {
		  SPI4->DR = value;
			
			value++;

      while (!(SPI4->SR & SPI_SR_RXNE));
      while (SPI4->SR & SPI_SR_BSY);
    }
		
		SPI4->CR1 |= SPI_CR1_CRCNEXT;
		
		int data = SPI4->DR;

		//printf("chip2:SPI4->DR = %d\r\n",SPI4->DR);

		uint16_t crc = SPI4->RXCRCR;

		if (crc == 0xa4) 
		{
			//printf("chip2:crc true,crc = %d\r\n",crc);
		} else
		{
			//printf("chip2:crc fail,crc = %d\r\n",crc);
		}
		
		SPI4->CR1 &= ~SPI_CR1_CRCNEXT;
		SPI4->CR1 &= ~SPI_CR1_CRCEN;
		
		for(int i = 99;i < 0;i--)
		{
			__NOP();
		}
	}
}

void SPI4_IRQHandler(void)
{
	if (SET == SPI_I2S_GetITStatus(SPI4, SPI_IT_CRCERR))
	{
		//printf("chip2:SPI4 CRCERR TEST PASS \r\n");
		SPI_I2S_ClearITPendingBit(SPI4, SPI_IT_CRCERR);
	}
}

void SPI4_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  // Enable GPIOA and SPI4 peripheral clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

  /* Configure GPIO pins for SPI4 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_SPI4);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI4_IRQn;         // SPI4中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器

  // Configure SPI4 as master mode, CPOL = 0, CPHA = 0, 8-bit data frame
  SPI4->CR1 =  SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SPE;
  SPI4->CR1 |= SPI_CR1_CRCEN;
  SPI4->CRCPR = 0XFFFF; // x^2 + x + 1
}

