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
	0x40013000,0x40013004,0x40013008,
	0x4001300C,0x40013010,0x40013014,
	0x40013018,0x4001301C,0x40013020
};

unsigned int SPI1_W_FFFF[9] =
{
	0x0000ffff,0x000000f7,0x00000080,
	0x00000000,0x0000ffff,0x00000000,
	0x00000000,0x00000000,0x00000000
};

unsigned int SPI1_W_0000[9] =
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

unsigned int SPI1_W_5A5A[9] =
{
	0x00005a5a,0x00000052,0x00000000,
	0x00000000,0x00005a5a,0x00000000,
	0x00000000,0x00000000,0x00000000
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
			printf(" 0x%08x 0xFFFFFFFF ERR,ERR DATA : 0x%08x\n",SPI1_REG[i],SPI1_W_FFFF[i]^(*((int*)SPI1_REG[i])));
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0xFFFFFFFF PASS\n");
	}
	else
	{
		printf("SPI1 Write 0xFFFFFFFF %d REG ERR\n",check);
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
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0x00000000 PASS\n");
	}
	else
	{
		printf("SPI1 Write 0x00000000 %d REG ERR\n",check);
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
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0xA5A5A5A5 PASS\n");
	}
	else
	{
		printf("SPI1 Write 0xA5A5A5A5 %d REG ERR\n",check);
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
			check+=1;
		}
	}

	if(!check)
	{
		printf("SPI1 Write 0x5A5A5A5A PASS\n");
	}
	else
	{
		printf("SPI1 Write 0x5A5A5A5A %d REG ERR\n",check);
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

#endif
