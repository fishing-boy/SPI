#include "SPI.h"
//#include "usart.h"
#include "delay.h"
#include "uart_REG.h"
#include "stdio.h"

#ifdef   SPI_FULL_HALF


//��ʼ���������
void GPIO_SPI_Init(void)
{
	//B10(SCK) C2(MISO) C3(MOSI) B12(NSS)
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE );//PORTB\C ʱ��ʹ��

	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC,&GPIO_InitStructure);


}

void SPI_Init_User(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ��SPIʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );//SPI2 ʱ��ʹ��

	//��ʼ���������
	GPIO_SPI_Init();

	//����SPI2
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //˫��˫��ȫ˫��
#ifdef   SPI_MASTER
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    //�� SPI
#endif   //SPI_MASTER
#ifdef   SPI_SLAVE
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;    //�� SPI
#endif   //SPI_SLAVE
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI ���ͽ��� 8 λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//�ڶ������������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS �ź����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //Ԥ��Ƶ 256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //���ݴ���� MSB λ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;  //CRC ֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);   //����ָ���Ĳ�����ʼ������ SPIx �Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel=SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
    NVIC_Init(&NVIC_InitStructure);	
	SPI2->CR1|=1<<8;  //SSI����Ϊ�ߵ�ƽ
		
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);	 //������8bit�����ж�
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����	
}

u8 buff=0;
void SPI2_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==SET)
	{         
		buff=SPI2_ReadByte(0x00);					    		   
	}
}  

u8 SPI2_ReadWriteByte(u8 TxData)
{ 
	u8 count;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		count++;
		if(count>200)
		return 0;	//���200��������û��ɷ��ͣ��ͷ���0�����������	
	}
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	count=0;
	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		count++;
		if(count>200)
		return 0;	//���200��������û��ɷ��ͣ��ͷ���0�����������	
	}
	
	return SPI_I2S_ReceiveData(SPI2); //���ؽ��յ�����
	//����һ�����ݵ�ͬʱ��������ν��յ�������
}

u8 SPI2_ReadByte(u8 TxData)
{
	u8 count;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		count++;
		if(count>200)
		return 0;	//���200��������û��ɷ��ͣ��ͷ���0�����������	
	}

	return SPI_I2S_ReceiveData(SPI2); //���ؽ��յ�����
}

void SPI2_WriteByte(u8 TxData)
{
	u8 count;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		count++;
		if(count>200)
		return;
	}		

	SPI_I2S_SendData(SPI2, TxData); 
}

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI2,ENABLE); 
} 
#endif   //SPI_FULL_HALF


#ifdef SPI_REG

unsigned int SPI1_REG[9] = 
{
	0x40013000,0x40013004,0x40013008,
	0x4001300C,0x40013010,0x40013014,
	0x40013018,0x4001301C,0x40013020
};

unsigned int SPI2_REG[9] = 
{
	0x40003800,0x40003804,0x40003808,
	0x4000380C,0x40003810,0x40003814,
	0x40003818,0x4000381C,0x40003820
};

unsigned int SPI3_REG[9] = 
{
	0x40003C00,0x40003C04,0x40003C08,
	0x40003C0C,0x40003C10,0x40003C14,
	0x40003C18,0x40003C1C,0x40003C20
};

unsigned int SPI1_W_FFFF[9] =
{
	0x0000ffff,0x000000f7,0x00000080,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_FFFF[9] =
{
	0x0000ffff,0x000000f7,0x00000080,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000fbf,0x000003ff
};

unsigned int SPI3_W_FFFF[9] =
{
	0x0000ffff,0x000000f7,0x00000080,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000fbf,0x000003ff
};



unsigned int SPI1_W_0000[9] =
{
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_0000[9] =
{
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI3_W_0000[9] =
{
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI1_W_A5A5[9] =
{
	0x0000a5a5,0x000000a5,0x00000000,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_A5A5[9] =
{
	0x0000a5a5,0x000000a5,0x00000000,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x000005a5,0x000001a5
};

unsigned int SPI3_W_A5A5[9] =
{
	0x0000a5a5,0x000000a5,0x00000000,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x000005a5,0x000001a5
};


unsigned int SPI1_W_5A5A[9] =
{
	0x00005a5a,0x00000052,0x00000000,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_5A5A[9] =
{
	0x00005a5a,0x00000052,0x00000080,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000a1a,0x0000025a
};

unsigned int SPI3_W_5A5A[9] =
{
	0x00005a5a,0x00000052,0x00000080,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000a1a,0x0000025a
};

void SPI1_W_FF(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI1_REG[i]) = 0xFFFFFFFF;
	}
}
void SPI1_W_FF_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI1_W_FFFF[i] != *((int*)SPI1_REG[i]))
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\n",SPI1_REG[i],SPI1_W_FFFF[i]^(*((int*)SPI1_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0xFFFFFFFF PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI1 Write 0xFFFFFFFF %d REG ERR\r\n",check);
		printf("\r\n");
	}
}

void SPI1_W_00(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI1_REG[i]) = 0x00000000;
	}
}
void SPI1_W_00_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI1_W_0000[i] != *((int*)SPI1_REG[i]))
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\n",SPI1_REG[i],SPI1_W_0000[i]^(*((int*)SPI1_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0x00000000 PASS\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI1 Write 0x00000000 %d REG ERR\n",check);
		printf("\r\n");
	}
}

void SPI1_W_A5(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI1_REG[i]) = 0xA5A5A5A5;
	}
}
void SPI1_W_A5_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI1_W_A5A5[i] != *((int*)SPI1_REG[i]))
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\n",SPI1_REG[i],SPI1_W_A5A5[i]^(*((int*)SPI1_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0xA5A5A5A5 PASS\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI1 Write 0xA5A5A5A5 %d REG ERR\n",check);
		printf("\r\n");
	}
}

void SPI1_W_5A(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI1_REG[i]) = 0x5A5A5A5A;
	}
}
void SPI1_W_5A_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI1_W_5A5A[i] != *((int*)SPI1_REG[i]))
		{
			printf(" 0x%08x 0x5A5A5A5A ERR,ERR DATA : 0x%08x\n",SPI1_REG[i],SPI1_W_5A5A[i]^(*((int*)SPI1_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0x5A5A5A5A PASS\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI1 Write 0x5A5A5A5A %d REG ERR\n",check);
		printf("\r\n");
	}
}

void output_SPI1(void)
{
	for (int i = 0; i < 9; i++)
	{
		printf("0x%08x,",*((int*)SPI1_REG[i]));
	}
}

void Rtc_Init(void)
{
 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//�򿪵�Դ������PWRʱ��
 //���RTC�Ĵ���д����
 PWR_BackupAccessCmd(ENABLE);//DBPλ��һ
 RTC_WriteProtectionCmd(ENABLE);//д����Կ
 
 RCC_LSEConfig(RCC_LSE_ON);//��LSEʱ��
}

void SPI2_3_W_FF(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI2_REG[i]) = 0xFFFFFFFF;
		*((int*)SPI3_REG[i]) = 0xFFFFFFFF;
	}
}
void SPI2_3_W_FF_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI2_W_FFFF[i] != *((int*)SPI2_REG[i]))
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\n",SPI2_REG[i],SPI2_W_FFFF[i]^(*((int*)SPI2_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI2 Write 0xFFFFFFFF PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI2 Write 0xFFFFFFFF %d REG ERR\r\n",check);
		printf("\r\n");
	}

	check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI3_W_FFFF[i] != *((int*)SPI3_REG[i]))
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\n",SPI3_REG[i],SPI3_W_FFFF[i]^(*((int*)SPI3_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI3 Write 0xFFFFFFFF PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI3 Write 0xFFFFFFFF %d REG ERR\r\n",check);
		printf("\r\n");
	}

}

void SPI2_3_W_00(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI2_REG[i]) = 0x00000000;
		*((int*)SPI3_REG[i]) = 0x00000000;
	}
}
void SPI2_3_W_00_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI2_W_0000[i] != *((int*)SPI2_REG[i]))
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\r\n",SPI2_REG[i],SPI2_W_0000[i]^(*((int*)SPI2_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI2 Write 0x00000000 PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI2 Write 0x00000000 %d REG ERR\r\n",check);
		printf("\r\n");
	}

	check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI3_W_0000[i] != *((int*)SPI3_REG[i]))
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\r\n",SPI3_REG[i],SPI3_W_0000[i]^(*((int*)SPI3_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI3 Write 0x00000000 PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI3 Write 0x00000000 %d REG ERR\r\n",check);
		printf("\r\n");
	}

}

void SPI2_3_W_A5(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI2_REG[i]) = 0xA5A5A5A5;
		*((int*)SPI3_REG[i]) = 0xA5A5A5A5;
	}
}
void SPI2_3_W_A5_CHECK(void)
{
	int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI2_W_A5A5[i] != *((int*)SPI2_REG[i]))
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\n",SPI2_REG[i],SPI2_W_A5A5[i]^(*((int*)SPI2_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI2 Write 0xA5A5A5A5 PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI2 Write 0xA5A5A5A5 %d REG ERR\r\n",check);
		printf("\r\n");
	}

	check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI3_W_A5A5[i] != *((int*)SPI3_REG[i]))
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\n",SPI3_REG[i],SPI3_W_A5A5[i]^(*((int*)SPI3_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI3 Write 0xA5A5A5A5 PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI3 Write 0xA5A5A5A5 %d REG ERR\r\n",check);
		printf("\r\n");
	}
}

void SPI2_3_W_5A(void)
{
	for (int i = 0; i < 9; i++)
	{
		*((int*)SPI2_REG[i]) = 0x5A5A5A5A;
		*((int*)SPI3_REG[i]) = 0x5A5A5A5A;
	}
}
void SPI2_3_W_5A_CHECK(void)
{
			int check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI2_W_5A5A[i] != *((int*)SPI2_REG[i]))
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\n",SPI2_REG[i],SPI2_W_5A5A[i]^(*((int*)SPI2_REG[i])));
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI2 Write 0xA5A5A5A5 PASS\r\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI2 Write 0xA5A5A5A5 %d REG ERR\r\r\n",check);
		printf("\r\n");
	}

	check = 0;
	
	for (int i = 0; i < 9; i++)
	{
		if(SPI3_W_5A5A[i] != *((int*)SPI3_REG[i]))
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\n",SPI3_REG[i],SPI3_W_5A5A[i]^(*((int*)SPI3_REG[i])));
			printf("\r\n");
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI3 Write 0xA5A5A5A5 PASS\r\n");
		printf("\r\n");
	}
	else
	{
		printf("SPI3 Write 0xA5A5A5A5 %d REG ERR\r\n",check);
		printf("\r\n");
	}
}

void output_SPI2_3(void)
{
	for (int i = 0; i < 9; i++)
	{
		printf("0x%08x,",*((int*)SPI2_REG[i]));
	}
	printf("\r\n");
	for (int i = 0; i < 9; i++)
	{
		printf("0x%08x,",*((int*)SPI3_REG[i]));
	}
}

#endif
