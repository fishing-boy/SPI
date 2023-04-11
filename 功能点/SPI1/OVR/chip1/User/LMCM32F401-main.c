#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

u8 ucSPI1_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

void bsp_SPI1_Init(void);
u8 SPI1_DataTransfer(u8 data);

void delay(unsigned int t)
{
	while(t--)
	{
		
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();
		
	}
}

int main(void)
{
	UartStdOutInit();
	printf("chip1:hello word\r\n");
	static u8 i = 0;
	uint8_t ucSPI1_RxBuf[5] = {0};
	bsp_SPI1_Init();

	/* 主机发、收数据 */
	while (1)
	{
		for (i = 0; i < 5; i++)
		{
			SPI1_DataTransfer(i);
			delay(500);
		}
	
	}
}


u8 SPI1_DataTransfer(u8 data)
{
	////////////////////SPI控制器///////////////////////
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

void bsp_SPI1_Init(void)
{
	/////////////SPI控制器/////////////////////
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//打开SPI1时钟
	RCC->APB2ENR |= 0x00001000;//打开SPI1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//打开GPIOB时钟
	
	/*验证*/
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;//配置第15号管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//配置复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;//配置第3、4、5号管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);//复用使能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);//复用使能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);//复用使能
	/*嵌入式*/
//	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 ;//配置第15号管脚
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//配置复用模式
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;//配置第3、4、5号管脚
//	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);//复用使能
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);//复用使能
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);//复用使能
	
//	SPI_InitTypeDef SPI_InitStruct={0};
//	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4分频
//	//W25Q64 MODE0    MODE3
//	SPI_InitStruct.SPI_CPHA = 0;
//	SPI_InitStruct.SPI_CPOL = 0;
//	
//	SPI_InitStruct.SPI_CRCPolynomial = 0;
//	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit传输
//	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
//	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先出
//	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主模式 Master Slave
//	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
//	
//	SPI_Init(SPI1,&SPI_InitStruct);
//	
//	SPI_Cmd(SPI1,ENABLE);//使能SPI1
	*(int*)0x40013000 = 0x0000034C;//spi1 init
}
