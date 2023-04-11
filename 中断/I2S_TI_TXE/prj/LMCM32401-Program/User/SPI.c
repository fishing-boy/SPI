#include "SPI.h"
//#include "usart.h"
#include "delay.h"
#include "uart_REG.h"
#include "stdio.h"

#ifdef   SPI_FULL_HALF


//初始化相关引脚
void GPIO_SPI_Init(void)
{
	//B10(SCK) C2(MISO) C3(MOSI) B12(NSS)
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE );//PORTB\C 时钟使能

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
	
	//使能SPI时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );//SPI2 时钟使能

	//初始化相关引脚
	GPIO_SPI_Init();

	//配置SPI2
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //双线双向全双工
#ifdef   SPI_MASTER
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;    //主 SPI
#endif   //SPI_MASTER
#ifdef   SPI_SLAVE
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;    //从 SPI
#endif   //SPI_SLAVE
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI 发送接收 8 位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//第二个跳变沿数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS 信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //预分频 256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //数据传输从 MSB 位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;  //CRC 值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);   //根据指定的参数初始化外设 SPIx 寄存器

	NVIC_InitStructure.NVIC_IRQChannel=SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
    NVIC_Init(&NVIC_InitStructure);	
	SPI2->CR1|=1<<8;  //SSI设置为高电平
		
	SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);	 //发送完8bit进入中断
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设	
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
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		count++;
		if(count>200)
		return 0;	//如果200个计数还没完成发送，就返回0跳出这个函数	
	}
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	count=0;
	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		count++;
		if(count>200)
		return 0;	//如果200个计数还没完成发送，就返回0跳出这个函数	
	}
	
	return SPI_I2S_ReceiveData(SPI2); //返回接收的数据
	//发送一个数据的同时，返回这次接收到的数据
}

u8 SPI2_ReadByte(u8 TxData)
{
	u8 count;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		count++;
		if(count>200)
		return 0;	//如果200个计数还没完成发送，就返回0跳出这个函数	
	}

	return SPI_I2S_ReceiveData(SPI2); //返回接收的数据
}

void SPI2_WriteByte(u8 TxData)
{
	u8 count;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
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
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI2,ENABLE); 
} 
#endif   //SPI_FULL_HALF


#ifdef SPI_REG

unsigned int SPI1_REG[9] = 
{
	0x40013008,0x40013000,0x40013004,
	0x4001300C,0x40013010,0x40013014,
	0x40013018,0x4001301C,0x40013020
};

unsigned int SPI2_REG[9] = 
{
	0x40003808,0x40003800,0x40003804,
	0x4000380C,0x40003810,0x40003814,
	0x40003818,0x4000381C,0x40003820
};

unsigned int SPI3_REG[9] = 
{
	0x40003C08,0x40003C00,0x40003C04,
	0x40003C0C,0x40003C10,0x40003C14,
	0x40003C18,0x40003C1C,0x40003C20
};

unsigned int SPI1_W_FFFF[9] =
{
	0x00000080,0x0000efff,0x000000f7,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_FFFF[9] =
{
	0x00000080,0x0000ffff,0x000000f7,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000fbf,0x000003ff
};

unsigned int SPI3_W_FFFF[9] =
{
	0x00000080,0x0000ffff,0x000000f7,
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
	0x00000000,0x0000a5a5,0x000000a5,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_A5A5[9] =
{
	0x00000000,0x0000a5a5,0x000000a5,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x000005a5,0x000001a5
};

unsigned int SPI3_W_A5A5[9] =
{
	0x00000000,0x0000a5a5,0x000000a5,
	0x00000000,0x0000a5a5,0x00000000,
	0x00000000,0x000005a5,0x000001a5
};


unsigned int SPI1_W_5A5A[9] =
{
	0x00000000,0x00005a5a,0x00000052,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI2_W_5A5A[9] =
{
	0x00000080,0x00005a5a,0x00000052,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000a1a,0x0000025a
};

unsigned int SPI3_W_5A5A[9] =
{
	0x00000080,0x00005a5a,0x00000052,
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
		unsigned int vale = *((int*)SPI1_REG[i]);
		if(SPI1_W_FFFF[i] != vale)
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI1_REG[i],SPI1_W_FFFF[i]^(*((int*)SPI1_REG[i])),vale,*((int*)SPI1_REG[i]),SPI1_W_FFFF[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI1_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI1_REG[i]);
		if(SPI1_W_0000[i] != vale)
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI1_REG[i],SPI1_W_0000[i]^(*((int*)SPI1_REG[i])),vale,*((int*)SPI1_REG[i]),SPI1_W_0000[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI1_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI1_REG[i]);
		if(SPI1_W_A5A5[i] != vale)
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI1_REG[i],SPI1_W_A5A5[i]^(*((int*)SPI1_REG[i])),vale,*((int*)SPI1_REG[i]),SPI1_W_A5A5[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI1_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI1_REG[i]);
		if(SPI1_W_5A5A[i] != vale)
		{
			printf(" 0x%08x 0x5A5A5A5A ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI1_REG[i],SPI1_W_5A5A[i]^(*((int*)SPI1_REG[i])),vale,*((int*)SPI1_REG[i]),SPI1_W_5A5A[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI1_REG[i],vale);
			printf("\r\n");
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
 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//打开电源管理器PWR时钟
 //解除RTC寄存器写保护
 PWR_BackupAccessCmd(ENABLE);//DBP位置一
 RTC_WriteProtectionCmd(ENABLE);//写入密钥
 
 RCC_LSEConfig(RCC_LSE_ON);//打开LSE时钟
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
		unsigned int vale = *((int*)SPI2_REG[i]);
		if(SPI2_W_FFFF[i] !=vale)
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI2_REG[i],SPI2_W_FFFF[i]^(*((int*)SPI2_REG[i])),vale,*((int*)SPI2_REG[i]),SPI2_W_FFFF[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI2_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI3_REG[i]);
		if(SPI3_W_FFFF[i] != vale)
		{
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI3_REG[i],SPI3_W_FFFF[i]^(*((int*)SPI3_REG[i])),vale,*((int*)SPI3_REG[i]),SPI3_W_FFFF[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI3_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI2_REG[i]);
		if(SPI2_W_0000[i] != vale)
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI2_REG[i],SPI2_W_0000[i]^(*((int*)SPI2_REG[i])),vale,*((int*)SPI2_REG[i]),SPI2_W_0000[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI2_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI3_REG[i]);
		if(SPI3_W_0000[i] != vale)
		{
			printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI3_REG[i],SPI3_W_0000[i]^(*((int*)SPI3_REG[i])),vale,*((int*)SPI3_REG[i]),SPI3_W_0000[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI3_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI2_REG[i]);
		if(SPI2_W_A5A5[i] != vale)
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI2_REG[i],SPI2_W_A5A5[i]^(*((int*)SPI2_REG[i])),vale,*((int*)SPI2_REG[i]),SPI2_W_A5A5[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI2_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI3_REG[i]);
		if(SPI3_W_A5A5[i] != vale)
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI3_REG[i],SPI3_W_A5A5[i]^(*((int*)SPI3_REG[i])),vale,*((int*)SPI3_REG[i]),SPI3_W_A5A5[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI3_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI2_REG[i]);
		if(SPI2_W_5A5A[i] != vale)
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI2_REG[i],SPI2_W_5A5A[i]^(*((int*)SPI2_REG[i])),vale,*((int*)SPI2_REG[i]),SPI2_W_5A5A[i]);
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI2_REG[i],vale);
			printf("\r\n");
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
		unsigned int vale = *((int*)SPI3_REG[i]);
		if(SPI3_W_5A5A[i] != vale)
		{
			printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
			SPI3_REG[i],SPI3_W_5A5A[i]^(*((int*)SPI3_REG[i])),vale,*((int*)SPI3_REG[i]),SPI3_W_5A5A[i]);
			printf("\r\n");
			check+=1;
		}
		else
		{
			printf(" 0x%08x test pass,VALE = 0x%08x\r\n",SPI3_REG[i],vale);
			printf("\r\n");
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

unsigned int SPI_REG_ADDRESS[3][9] = 
{
	{0x40013000,0x40013004,0x4001300C,0x40013010,0x40013014,0x40013018,0x4001301C,0x40013020,0x40013008},
	{0x40003800,0x40003804,0x4000380C,0x40003810,0x40003814,0x40003818,0x4000381C,0x40003820,0x40003808},
	{0x40003C00,0x40003C04,0x40003C0C,0x40003C10,0x40003C14,0x40003C18,0x40003C1C,0x40003C20,0x40003C08}
};

unsigned int SPI_REG_RESET[3][9] =
{
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000000,0x00000002},
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002},
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002}
};

unsigned int SPI_W_FF_VALE[3][9] =
{
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000000,0x00000000,0x00000002},
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000fbf,0x000003ff,0x00000002},
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000fbf,0x000003ff,0x00000002}
};

unsigned int SPI_W_00_VALE[3][9] =
{
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082},
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082},
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082}
};

unsigned int SPI_W_A5_VALE[3][9] =
{
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x000005a5,0x000001a5,0x00000080},
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x000005a5,0x000001a5,0x00000080}
};

unsigned int SPI_W_5A_VALE[3][9] =
{
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000a1a,0x0000025a,0x00000080},
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000a1a,0x0000025a,0x00000080}
}; 

void SPI_W(unsigned int data)
{
	for (int i = 0; i < 3; i++)
	{
		int check = 0;
		unsigned int expected = 0;
		for (int j = 8; j >= 0; j--)
		{
			*((int*)SPI_REG_ADDRESS[i][j]) = data;

			unsigned int vale = *((int*)SPI_REG_ADDRESS[i][j]);
			if(data == 0xFFFFFFFF)
			{
				expected = SPI_W_FF_VALE[i][j];
			}
			else if(data == 0x00000000)
			{
				expected = SPI_W_00_VALE[i][j];
			}
			else if(data == 0xA5A5A5A5)
			{
				expected = SPI_W_A5_VALE[i][j];
			}
			else if(data == 0x5A5A5A5A)
			{
				expected = SPI_W_5A_VALE[i][j];
			}	
			if(vale == expected)
			{
				printf("0x%08x wirte 0x%08x success,vale = 0x%08x\r\n",SPI_REG_ADDRESS[i][j],data,vale);
			}
			else
			{
				printf("0x%08x wirte 0x%08x fail,vale = 0x%08x,expected value = 0x%08x\r\n",SPI_REG_ADDRESS[i][j],data,vale,expected);
				check += 1; 
			}

			*((int*)SPI_REG_ADDRESS[i][j]) = SPI_REG_RESET[i][j];

		}
		printf("\r\n");
		if(!check)
		{
			printf("SPI%d write 0x%08x test pass\r\n",i+1,data);
		}
		else
		{
			printf("SPI%d write 0x%08x test fail,error number: %d\r\n",i+1,data,check);
		}
	}
	/*
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			printf("0x%08x,",*((int*)SPI_REG_ADDRESS[j][i]));
		}
		printf("\r\n");	
	}
	*/
}


#endif


#ifdef  IIS_REG

unsigned int IIS_REG_ADDRESS[2][9] =
{
	{
		0x40003408,0x40003400,0x40003404,
	 	0x4000340C,0x40003410,0x40003414,
	 	0x40003418,0x4000341C,0x40003420
	},
	{
		0x40004008,0x40004000,0x40004004,
	 	0x4000400C,0x40004010,0x40004014,
	 	0x40004018,0x4000401C,0x40004020
	}
};

unsigned int IIS_W_FF_VALE[2][9] =
{
	{
		0x00000080,0x0000ffff,0x000000f7,
		0x00000000,0x0000ffff,0x00000000,
		0x00000000,0x00000fbf,0x000003ff
	},
	{
		0x00000080,0x0000ffff,0x000000f7,
		0x00000000,0x0000ffff,0x00000000,
		0x00000000,0x00000fbf,0x000003ff
	}
};
unsigned int IIS_W_00_VALE[2][9] =
{
	{
		0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000
	},
	{
		0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000
	}
};
unsigned int IIS_W_A5_VALE[2][9] = 
{
	{
		0x00000000,0x0000a5a5,0x000000a5,
		0x00000000,0x0000a5a5,0x00000000,
		0x00000000,0x000005a5,0x000001a5
	},
	{
		0x00000000,0x0000a5a5,0x000000a5,
		0x00000000,0x0000a5a5,0x00000000,
		0x00000000,0x000005a5,0x000001a5
	}
};
unsigned int IIS_W_5A_VALE[2][9] =
{
	{
		0x00000080,0x00005a5a,0x00000052,
		0x00000000,0x00005a5a,0x00000000,
		0x00000000,0x00000a1a,0x0000025a
	},
	{
		0x00000080,0x00005a5a,0x00000052,
		0x00000000,0x00005a5a,0x00000000,
		0x00000000,0x00000a1a,0x0000025a
	}
};

void Rtc_Init(void)
{
 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//打开电源管理器PWR时钟
 //解除RTC寄存器写保护
 PWR_BackupAccessCmd(ENABLE);//DBP位置一
 RTC_WriteProtectionCmd(ENABLE);//写入密钥
 
 RCC_LSEConfig(RCC_LSE_ON);//打开LSE时钟
}

void IIS_W_FF(void)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			*((int*)IIS_REG_ADDRESS[j][i]) = 0xFFFFFFFF;
		}	
	}
}
void IIS_W_FF_CHECK(void)
{
	int check = 0;
	
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			unsigned int vale = *((int*)IIS_REG_ADDRESS[j][i]);
			if(IIS_W_FF_VALE[j][i] != vale)
			{
				printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
				IIS_REG_ADDRESS[j][i],IIS_W_FF_VALE[j][i]^(*((int*)IIS_REG_ADDRESS[j][i])),vale,*((int*)IIS_REG_ADDRESS[j][i]),IIS_W_FF_VALE[j][i]);
				check+=1;
			}
			else
			{
				printf(" 0x%08x test pass,VALE = 0x%08x\r\n",IIS_REG_ADDRESS[j][i],vale);
				printf("\r\n");
			}
		}
	}

	if(!check)
	{
		printf("IIS Write 0xFFFFFFFF PASS\r\r\n");
		printf("\r\n");
	}
	else
	{
		printf("IIS Write 0xFFFFFFFF %d REG ERR\r\r\n",check);
		printf("\r\n");
	}
}

void IIS_W_00(void)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			*((int*)IIS_REG_ADDRESS[j][i]) = 0x00000000;
		}	
	}
}
void IIS_W_00_CHECK(void)
{
	int check = 0;
	
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			unsigned int vale = *((int*)IIS_REG_ADDRESS[j][i]);
			if(IIS_W_00_VALE[j][i] != vale)
			{
				printf(" 0x%08x 0x00000000 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
				IIS_REG_ADDRESS[j][i],IIS_W_00_VALE[j][i]^(*((int*)IIS_REG_ADDRESS[j][i])),vale,*((int*)IIS_REG_ADDRESS[j][i]),IIS_W_00_VALE[j][i]);
				check+=1;
			}
			else
			{
				printf(" 0x%08x test pass,VALE = 0x%08x\r\n",IIS_REG_ADDRESS[j][i],vale);
				printf("\r\n");
			}
		}
	}

	if(!check)
	{
		printf("IIS Write 0x00000000 PASS\r\r\n");
		printf("\r\n");
	}
	else
	{
		printf("IIS Write 0x00000000 %d REG ERR\r\r\n",check);
		printf("\r\n");
	}
}

void IIS_W_A5(void)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			*((int*)IIS_REG_ADDRESS[j][i]) = 0xA5A5A5A5;
		}	
	}
}
void IIS_W_A5_CHECK(void)
{
	int check = 0;
	
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			unsigned int vale = *((int*)IIS_REG_ADDRESS[j][i]);
			if(IIS_W_A5_VALE[j][i] != vale)
			{
				printf(" 0x%08x 0xA5A5A5A5 ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
				IIS_REG_ADDRESS[j][i],IIS_W_A5_VALE[j][i]^(*((int*)IIS_REG_ADDRESS[j][i])),vale,*((int*)IIS_REG_ADDRESS[j][i]),IIS_W_A5_VALE[j][i]);
				check+=1;
			}
			else
			{
				printf(" 0x%08x test pass,VALE = 0x%08x\r\n",IIS_REG_ADDRESS[j][i],vale);
				printf("\r\n");
			}
		}
	}

	if(!check)
	{
		printf("IIS Write 0xA5A5A5A5 PASS\r\r\n");
		printf("\r\n");
	}
	else
	{
		printf("IIS Write 0xA5A5A5A5 %d REG ERR\r\r\n",check);
		printf("\r\n");
	}
}

void IIS_W_5A(void)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			*((int*)IIS_REG_ADDRESS[j][i]) = 0x5A5A5A5A;
		}	
	}
}
void IIS_W_5A_CHECK(void)
{
	int check = 0;
	
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			unsigned int vale = *((int*)IIS_REG_ADDRESS[j][i]);
			if(IIS_W_5A_VALE[j][i] != vale)
			{
				printf(" 0x%08x 0x5A5A5A5A ERR,ERR DATA : 0x%08x\r\nREG_VALE_FIRST = 0x%08x\r\nREG_VALE_SECOND = 0x%08x\r\nExpected value =  0x%08x\r\n",
				IIS_REG_ADDRESS[j][i],IIS_W_5A_VALE[j][i]^(*((int*)IIS_REG_ADDRESS[j][i])),vale,*((int*)IIS_REG_ADDRESS[j][i]),IIS_W_5A_VALE[j][i]);
				check+=1;
			}
			else
			{
				printf(" 0x%08x test pass,VALE = 0x%08x\r\n",IIS_REG_ADDRESS[j][i],vale);
				printf("\r\n");
			}
		}
	}

	if(!check)
	{
		printf("IIS Write 0x5A5A5A5A PASS\r\r\n");
		printf("\r\n");
	}
	else
	{
		printf("IIS Write 0x5A5A5A5A %d REG ERR\r\r\n",check);
		printf("\r\n");
	}
}


void output_IIS(void)
{
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			printf("0x%08x,",*((int*)IIS_REG_ADDRESS[j][i]));
		}
		printf("\r\n");	
	}
}

#endif
