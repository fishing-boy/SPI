#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

u8 ucSPI3_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};

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
	printf("chip1:hello word\r\n");
	static u8 i = 0;
	uint8_t ucSPI3_RxBuf[5] = {0};
	bsp_SPI3_Init();

	/* �������������� */
	while (1)
	{
		for (i = 0; i < 5; i++)
		{
			SPI3_DataTransfer(i);
			delay(500);
		}
	
	}
}


u8 SPI3_DataTransfer(u8 data)
{
	////////////////////SPI������///////////////////////
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI3,data);
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI3);
}

void bsp_SPI3_Init(void)
{
	/////////////SPI������/////////////////////
  // Enable GPIOA and SPI3 peripheral clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*��֤*/
	GPIO_InitTypeDef GPIO_InitStruct={0};//����ṹ�������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;//���õ�4�Źܽ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF ;//���ø���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;//���õ�3��4��5�Źܽ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);//����ʹ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);//����ʹ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);//����ʹ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);//����ʹ��
	
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
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);//����ʹ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);//����ʹ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);//����ʹ��
	
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
	
	SPI_Init(SPI3,&SPI_InitStruct);
	
	SPI_Cmd(SPI3,ENABLE);//ʹ��SPI3
	*(int*)0x40013000 = 0x0000034C;//SPI3 init
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_ERR, ENABLE);  // ʹ�ܽ����ж�
	
	/* NVIC�жϿ��������� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         // �ж����ȼ�����2

    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;         // SPI3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                         // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���
}

u8 ovr=0;
/*SPI3�жϺ���*/
void SPI3_IRQHandler(void)
{
    if (SET == SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_OVR))
    {
		printf("chip:OVR PASS \r\n");
		ovr = SPI3->DR ;  
		SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_OVR);
		UartEndSimulation();
		//SPI_Cmd(SPI3,DISABLE);//��ʹ��SPI3
//		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE); // ��ʹ�ܽ����ж�
//		SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_ERR, DISABLE);  // ��ʹ�ܽ����ж�
    }
	
	/* �жϽ��ջ������Ƿ�Ϊ�ǿ� */
    if (SET == SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE))
    {
		//while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET);//�ȴ����ܻ������ǿ�
        //SPI3->DR = ucSPI3_TxBuf[ucSPI3_RxCount];                      /* �����ͻ����������� */
		//printf("%x\r\n",ucSPI3_TxBuf[ucSPI3_RxCount]);
        // while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); /* �ȴ��������� */ 
        //SPI3->DR = ucSPI3_RxBuf[ucSPI3_RxCount];                  	   /* �����ͻ����������� */
	    //SPI_I2S_ClearITPendingBit()
        /* ���������� */
//        ucSPI3_RxCount++;
//        if (ucSPI3_RxCount > 4)
//        {
//            ucSPI3_RxCount = 0;
//        }

        /* ���жϱ�־ */
        //SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_RXNE);
    }
}