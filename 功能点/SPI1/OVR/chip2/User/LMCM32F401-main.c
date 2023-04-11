#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

uint8_t ucSPI1_RxBuf[5] = {0};
uint8_t ucSPI1_RxCount = 0;

void bsp_SPI1_Init(void);

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

u8 SPI1_DataTransfer(u8 data)
{
	////////////////////SPI������///////////////////////
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI1,data);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI1);
}



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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//��SPI1ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//��GPIOBʱ��
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*��֤*/
	GPIO_InitTypeDef GPIO_InitStruct={0};//����ṹ�������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;//���õ�15�Źܽ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//���ø���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;//���õ�3��4��5�Źܽ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);//����ʹ��
	/*Ƕ��ʽ*/
//	GPIO_InitTypeDef GPIO_InitStruct={0};//����ṹ�������
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 ;//���õ�15�Źܽ�
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//���ø���ģʽ
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;//���õ�3��4��5�Źܽ�
//	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);//����ʹ��
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);//����ʹ��
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);//����ʹ��
	
	SPI_InitTypeDef SPI_InitStruct={0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//4��Ƶ
	//W25Q64 MODE0    MODE3
	SPI_InitStruct.SPI_CPHA = 0;
	SPI_InitStruct.SPI_CPOL = 0;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8bit����
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//��λ�ȳ�
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;//��ģʽ Master Slave
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI_Cmd(SPI1,ENABLE);//ʹ��SPI1
	
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, ENABLE);  // ʹ�ܽ����ж�
	
	/* NVIC�жϿ��������� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // �ж����ȼ�����2

    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;         // SPI1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                         // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

u8 ovr=0;
/*spi1�жϺ���*/
void SPI1_IRQHandler(void)
{
    if (SET == SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_OVR))
    {
		printf("chip2:OVR PASS \r\n");
		ovr = SPI1->DR ;  
		SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_OVR);
		UartEndSimulation();
		//SPI_Cmd(SPI1,DISABLE);//��ʹ��SPI1
//		SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE); // ��ʹ�ܽ����ж�
//		SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, DISABLE);  // ��ʹ�ܽ����ж�
    }
	
	/* �жϽ��ջ������Ƿ�Ϊ�ǿ� */
    if (SET == SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE))
    {
		//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);//�ȴ����ܻ������ǿ�
        //SPI1->DR = ucSPI1_TxBuf[ucSPI1_RxCount];                      /* �����ͻ����������� */
		//printf("%x\r\n",ucSPI1_TxBuf[ucSPI1_RxCount]);
        // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); /* �ȴ��������� */ 
        // SPI1->DR = ucSPI1_RxBuf[ucSPI1_RxCount];                  	   /* �����ͻ����������� */
	    //SPI_I2S_ClearITPendingBit()
        /* ���������� */
//        ucSPI1_RxCount++;
//        if (ucSPI1_RxCount > 4)
//        {
//            ucSPI1_RxCount = 0;
//        }

        /* ���жϱ�־ */
        //SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
    }
}




