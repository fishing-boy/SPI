#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

void bsp_SPI1_Init(void);


int main(void)
{
	UartStdOutInit();
	printf("chip2:hello world \r\n");
	bsp_SPI1_Init();
	while(1)
	{
		
	}
}

void bsp_SPI1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//打开SPI1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//打开GPIOB时钟
	NVIC_InitTypeDef NVIC_InitStructure;
	
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
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4分频
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit传输
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先出
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;//从模式 Master Slave
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI_Cmd(SPI1,ENABLE);//使能SPI1
	
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);  // 使能接收中断
	
	
	/* NVIC中断控制器配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // 中断优先级分组2

    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;         // SPI1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器
}


/*spi1中断函数*/
static u8 i=0;
void SPI1_IRQHandler(void)
{
    if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE)==SET)
	{
		if(i==0)
		{	
			printf("RXNE PASS \r\n");
			i++;
		}
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
		//SPI_Cmd(SPI1,DISABLE);//不使能SPI1
		//SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE); // 不使能接收中断
	}
}




