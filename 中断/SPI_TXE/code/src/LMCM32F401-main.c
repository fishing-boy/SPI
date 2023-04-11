/*
本工程主要是用于测试spi的中断功能
*/
 
//#include "SPI.h"
//#include "usart.h"
#include "delay.h"
#include "uart_REG.h"
#include "stdio.h"

#include <stm32f4xx.h>
#include <stm32f4xx_flash.h>

void SPI_Init_User(void);

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	UartStdOutInit();	 //串口初始化为115200

	SPI_Init_User();  //初始化spi2
	//SPI2_SetSpeed(SPI_BaudRatePrescaler_128);//设置spi时钟速度
	//printf("TEST SPI EXTI START\r\n");
	//SPI2_WriteByte(0xff);
	while(1)
	{

	}
}

void SPI_Init_User(void)
{
/*	
	*((int*)0x40023844) |= 0x00003000;  //SPI1、4时钟使能   实际上无法使能spi4
	*((int*)0x40023840) |= 0x0000C000;  //SPI2、3时钟使能

	*((int*)0x40013400) |= 0x0000033F;  //SPI4->CR1
	*((int*)0x40013404) |= 0x00000080;  //SPI4->CR2
	*((int*)0x40013410) |= 0x00000007;  //SPI4->CRCPR

	*((int*)0x40013C00) |= 0x0000033F;  //SPI3->CR1
	*((int*)0x40013C04) |= 0x00000080;  //SPI3->CR2
	*((int*)0x40013C10) |= 0x00000007;  //SPI3->CRCPR

	*((int*)0x40013800) |= 0x0000033F;  //SPI2->CR1
	*((int*)0x40013804) |= 0x00000080;  //SPI2->CR2
	*((int*)0x40013810) |= 0x00000007;  //SPI2->CRCPR

	*((int*)0x40013000) |= 0x0000033F;  //SPI1->CR1
	*((int*)0x40013004) |= 0x00000080;  //SPI1->CR2
	*((int*)0x40013010) |= 0x00000007;  //SPI1->CRCPR

	*((int*)0xE000E104) |= 0x00080018;  //NVIC->ISER1
	*((int*)0xE000E108) |= 0x00100000;  //NVIC->ISER2

	*((int*)0x40013000) |= 0x00000040;  //SPI1->CR1
	*((int*)0x40013800) |= 0x00000040;  //SPI2->CR1
	*((int*)0x40013C00) |= 0x00000040;  //SPI3->CR1
	*((int*)0x40013400) |= 0x00000040;  //SPI4->CR1

*/
	SPI_InitTypeDef   SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能SPI时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );//SPI1 时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );//SPI2 时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );//SPI3 时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE );//SPI4 时钟使能

	//配置SPI2
	//SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    //主 SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI 发送接收 8 位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//第二个跳变沿数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS 信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //预分频 256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //数据传输从 MSB 位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;  //CRC 值计算的多项式
	SPI_Init(SPI4, &SPI_InitStructure);   //根据指定的参数初始化外设 SPIx 寄存器
	SPI_Init(SPI3, &SPI_InitStructure);   //根据指定的参数初始化外设 SPIx 寄存器
	SPI_Init(SPI2, &SPI_InitStructure);   //根据指定的参数初始化外设 SPIx 寄存器
	SPI_Init(SPI1, &SPI_InitStructure);   //根据指定的参数初始化外设 SPIx 寄存器

	NVIC_InitStructure.NVIC_IRQChannel=SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
    NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel=SPI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel=SPI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel=SPI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);	
		
	//SPI_I2S_SendData(SPI1, 0xff);
	//SPI_I2S_SendData(SPI2, 0xff);	

	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);  //发送缓冲区为空的时候进入中断
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE);  //发送缓冲区为空的时候进入中断
	SPI_I2S_ITConfig(SPI3,SPI_I2S_IT_TXE,ENABLE);  //发送缓冲区为空的时候进入中断
	SPI_I2S_ITConfig(SPI4,SPI_I2S_IT_TXE,ENABLE);  //发送缓冲区为空的时候进入中断

	SPI_I2S_ClearFlag(SPI1,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI2,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI3,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI4,SPI_I2S_IT_TXE);

	SPI_Cmd(SPI4, ENABLE); //使能SPI外设
	SPI_Cmd(SPI3, ENABLE); //使能SPI外设
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

}

void SPI1_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==SET) //发送区为空
	{         
		printf("SPI1 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
	}
} 

void SPI2_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==SET) //发送区为空
	{         
		printf("SPI2 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
	}
}  

void SPI3_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==SET) //发送区为空
	{         
		printf("SPI3 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI3,SPI_I2S_IT_TXE,DISABLE);
	}
} 

void SPI4_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI4,SPI_I2S_FLAG_TXE)==SET) //发送区为空
	{         
		printf("SPI4 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI4,SPI_I2S_IT_TXE,DISABLE);
	}
}  

