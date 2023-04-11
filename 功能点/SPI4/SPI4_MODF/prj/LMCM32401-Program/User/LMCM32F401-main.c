#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

void bsp_SPI4_Init(void);

int main(void)
{
	UartStdOutInit();

	printf("SPI4 MODF TEST START\r\n");
	bsp_SPI4_Init();
	
	SPI4->CR1 &= ~0x0100;
	while (1)
	{

	}
}

void bsp_SPI4_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI4);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI4);
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI4,&SPI_InitStruct);
	
	SPI_Cmd(SPI4,ENABLE);
	
	SPI_I2S_ITConfig(SPI4, SPI_I2S_IT_ERR, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = SPI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int a = 0;

void SPI4_IRQHandler(void)
{
	SPI4->CR1 |= 0x0100;
	if(a)
	{
		SPI_I2S_ClearITPendingBit(SPI4, SPI_IT_MODF);
	}
	a++;
	printf("SPI4 MODF TEST PASS\r\n");
}
