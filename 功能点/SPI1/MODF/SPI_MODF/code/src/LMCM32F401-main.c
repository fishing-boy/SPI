#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

uint8_t ucSPI2_RxBuf[5] = {0};
uint8_t ucSPI2_RxCount = 0;

uint8_t ucSPI1_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

void bsp_SPI2_Init(void);

int main(void)
{
	UartStdOutInit();

	printf("SPI1 MODF TEST START\r\n");
	bsp_SPI2_Init();
	
	SPI1->CR1 &= ~0x0100;
	while (1)
	{

	}
}

void bsp_SPI2_Init(void)
{
	/////////////SPI������/////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//��SPI2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��GPIOBʱ��
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStruct={0};//����ṹ�������
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//���ø���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//���õ�10�Źܽ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);//����ʹ��
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4��Ƶ
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit����
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//��λ�ȳ�
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//��ģʽ Master Slave
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI_Cmd(SPI1,ENABLE);//ʹ��SPI2
	
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, ENABLE);  // ʹ�ܽ����ж�
	
	
	/* NVIC�жϿ��������� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // �ж����ȼ�����2

    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;         // SPI2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                         // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

/*spi2�жϺ���*/
void SPI1_IRQHandler(void)
{
    if (SET == SPI_I2S_GetITStatus(SPI1, SPI_IT_MODF))
    {
		SPI1->CR1 |= 0x0100;
        SPI_I2S_ClearITPendingBit(SPI1, SPI_IT_MODF);
		printf("SPI1 MODF TEST PASS\r\n");
    }
}
