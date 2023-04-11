/*
��������Ҫ�����ڲ���spi���жϹ���
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	UartStdOutInit();	 //���ڳ�ʼ��Ϊ115200

	SPI_Init_User();  //��ʼ��spi2
	//SPI2_SetSpeed(SPI_BaudRatePrescaler_128);//����spiʱ���ٶ�
	//printf("TEST SPI EXTI START\r\n");
	//SPI2_WriteByte(0xff);
	while(1)
	{

	}
}

void SPI_Init_User(void)
{
/*	
	*((int*)0x40023844) |= 0x00003000;  //SPI1��4ʱ��ʹ��   ʵ�����޷�ʹ��spi4
	*((int*)0x40023840) |= 0x0000C000;  //SPI2��3ʱ��ʹ��

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

	//ʹ��SPIʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );//SPI1 ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );//SPI2 ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );//SPI3 ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE );//SPI4 ʱ��ʹ��

	//����SPI2
	//SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    //�� SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI ���ͽ��� 8 λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//�ڶ������������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS �ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //Ԥ��Ƶ 256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //���ݴ���� MSB λ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;  //CRC ֵ����Ķ���ʽ
	SPI_Init(SPI4, &SPI_InitStructure);   //����ָ���Ĳ�����ʼ������ SPIx �Ĵ���
	SPI_Init(SPI3, &SPI_InitStructure);   //����ָ���Ĳ�����ʼ������ SPIx �Ĵ���
	SPI_Init(SPI2, &SPI_InitStructure);   //����ָ���Ĳ�����ʼ������ SPIx �Ĵ���
	SPI_Init(SPI1, &SPI_InitStructure);   //����ָ���Ĳ�����ʼ������ SPIx �Ĵ���

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

	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);  //���ͻ�����Ϊ�յ�ʱ������ж�
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE);  //���ͻ�����Ϊ�յ�ʱ������ж�
	SPI_I2S_ITConfig(SPI3,SPI_I2S_IT_TXE,ENABLE);  //���ͻ�����Ϊ�յ�ʱ������ж�
	SPI_I2S_ITConfig(SPI4,SPI_I2S_IT_TXE,ENABLE);  //���ͻ�����Ϊ�յ�ʱ������ж�

	SPI_I2S_ClearFlag(SPI1,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI2,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI3,SPI_I2S_IT_TXE);
	SPI_I2S_ClearFlag(SPI4,SPI_I2S_IT_TXE);

	SPI_Cmd(SPI4, ENABLE); //ʹ��SPI����
	SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

}

void SPI1_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==SET) //������Ϊ��
	{         
		printf("SPI1 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
	}
} 

void SPI2_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==SET) //������Ϊ��
	{         
		printf("SPI2 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
	}
}  

void SPI3_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==SET) //������Ϊ��
	{         
		printf("SPI3 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI3,SPI_I2S_IT_TXE,DISABLE);
	}
} 

void SPI4_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI4,SPI_I2S_FLAG_TXE)==SET) //������Ϊ��
	{         
		printf("SPI4 TEX EXTI PASS\r\n");
		SPI_I2S_ITConfig(SPI4,SPI_I2S_IT_TXE,DISABLE);
	}
}  

