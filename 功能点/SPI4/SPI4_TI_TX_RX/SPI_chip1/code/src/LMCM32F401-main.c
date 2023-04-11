#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

//#define MCU_TEST

void SPI4_MY_INIT(void);

int num = 0;

int main(void)
{
  //UART init
	UartStdOutInit();
	printf("chip1:test start\r\n");

	SPI4_MY_INIT();

	int t = 1000;
	while(t--);

	SPI4->CR1|=SPI_CR1_SPE;

	while (num<9);

	printf("test pass\r\n");

	while(1)
	{

	}
}

void SPI4_IRQHandler(void)
{
	SPI4->CR2 &= ~SPI_CR2_TXEIE;
	if (SET == SPI_I2S_GetITStatus(SPI4, SPI_I2S_IT_TXE))
	{
		int t = 1000;
		while(t--);
		SPI4->DR = 0x1234;
		num++;
		SPI_I2S_ClearITPendingBit(SPI4, SPI_I2S_IT_TXE);
	}
	SPI4->CR2 |= SPI_CR2_TXEIE;
}

void SPI4_MY_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable GPIOA and SPI4 peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14;
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

	SPI4->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SSI;
	SPI4->CR2 |= SPI_CR2_TXEIE;
}

