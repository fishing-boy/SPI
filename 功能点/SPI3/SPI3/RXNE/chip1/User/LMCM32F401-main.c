#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

//u8 ucSPI3_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

void bsp_SPI3_Init(void);
u8 SPI3_DataTransfer(u8 data);

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
	printf("chip1:hello world\r\n");
	static u8 i = 0;
	bsp_SPI3_Init();
	
	while(1)
	{
		/* 主机发、收数据 */
		for(i=0;i<5;i++)
		{
			SPI3_DataTransfer(i);
			delay(10);
		}
	}
}


u8 SPI3_DataTransfer(u8 data)
{
	////////////////////SPI控制器///////////////////////
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI3,data);
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI3);
}

void bsp_SPI3_Init(void)
{
	/////////////SPI控制器/////////////////////
  // Enable GPIOA and SPI3 peripheral clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*验证*/
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;//配置第4号管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//配置复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;//配置第3、4、5号管脚
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);//复用使能
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
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);//复用使能
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);//复用使能
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);//复用使能
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4分频
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit传输
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先出
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主模式 Master Slave
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
//	
	SPI_Init(SPI3,&SPI_InitStruct);
//	
	SPI_Cmd(SPI3,ENABLE);//使能SPI3
	*(int*)0x40013000 = 0x0000034C;//SPI3 init
	
		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);  // 使能接收中断
	
	
	/* NVIC中断控制器配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // 中断优先级分组2

    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;         // SPI3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器
}
