#include "SPI.h"
#include "uart_REG.h"
#include "stdio.h"


uint8_t ucSPI1_RxCount=0;
uint16_t ucSPI1_RxBuf[5] = {0x06,0x07,0x08,0x09,0x0A};
uint8_t   i;

void bsp_SPI1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//打开SPI1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//打开GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//打开GPIOB时钟
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义结构体变量名
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 ;//配置第15号管脚
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//配置复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;//配置第3、4、5号管脚
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);//复用使能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);//复用使能
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4分频
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit传输
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先出
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;//主模式 Master Slave
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

/*spi2中断函数*/

//int i =0;
void SPI1_IRQHandler(void)
{
    /* 判断接收缓冲区是否为非空 */
//    if (SET == SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE))
//    {
//        ucSPI2_RxBuf[ucSPI2_RxCount] = SPI2->DR;                         /* 读取接收缓冲区数据 */
//		printf("0x%08x \r\n",ucSPI2_RxBuf[ucSPI2_RxCount]);
//        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);  /* 等待发送区空 */ 
//        SPI2->DR = ucSPI2_RxBuf[ucSPI2_RxCount];                  /* 往发送缓冲区填数据 */
////SPI_I2S_ClearITPendingBit()
//        /* 计数器处理 */
//        ucSPI2_RxCount++;
//        if (ucSPI2_RxCount > 4)
//        {
//            ucSPI2_RxCount = 0;
//        }

//        /* 清中断标志 */
//        SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
//    }
	
	if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE)==SET)
	{
		//SPI_I2S_ClearITPendingBit(SPI2,SPI_I2S_IT_RXNE);
//		SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE, DISABLE);//关闭中断
		//for(i=0;i<5;i++)
		if(i<5)
		{
			while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);//等待接受缓冲器空
			ucSPI1_RxBuf[i]=SPI_I2S_ReceiveData(SPI1);
			printf("%x \r\n",ucSPI1_RxBuf[i]);
			i++;
		}
		else
		{
			i =0;
		}
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
//		SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);//开启中断
	}
}





