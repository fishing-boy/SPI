#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

uint8_t ucSPI3_RxBuf[5] = {0};
uint8_t ucSPI3_RxCount = 0;

uint8_t ucSPI3_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

void bsp_SPI3_Init(void);

int main(void)
{
	UartStdOutInit();

	printf("SPI3 MODF TEST START\r\n");
	bsp_SPI3_Init();
	
	SPI3->CR1 &= ~0x0100;
	while (1)
	{

	}
}

void bsp_SPI3_Init(void)
{
	/////////////SPI控制器/////////////////////
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;//打开SPI2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开GPIOB时钟
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//配置复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_7;//配置第10号管脚
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);//复用使能
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4分频
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit传输
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先出
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//从模式 Master Slave
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI3,&SPI_InitStruct);
	
	SPI_Cmd(SPI3,ENABLE);//使能SPI2
	
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_ERR, ENABLE);  // 使能接收中断
	
	
	/* NVIC中断控制器配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // 中断优先级分组2

    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;         // SPI2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                         // 根据指定的参数初始化NVIC寄存器
}

/*spi2中断函数*/
void SPI3_IRQHandler(void)
{
    if (SET == SPI_I2S_GetITStatus(SPI3, SPI_IT_MODF))
    {
		SPI3->CR1 |= 0x0100;
        SPI_I2S_ClearITPendingBit(SPI3, SPI_IT_MODF);
		printf("SPI3 MODF TEST PASS\r\n");
    }
}
