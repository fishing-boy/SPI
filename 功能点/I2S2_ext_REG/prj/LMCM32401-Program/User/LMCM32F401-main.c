#include "uart_REG.h"
#include "stdio.h"

#include <stm32f4xx.h>
#include <stm32f4xx_flash.h>

void SPI_W(unsigned int data);

int main()
{
	UartStdOutInit();  //uart初始化
	printf("TEXT START\r\n");  

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->CR |= RCC_CR_PLLI2SON;

	SPI_W(0xFFFFFFFF);
	SPI_W(0x00000000);
	SPI_W(0xA5A5A5A5);
	SPI_W(0x5A5A5A5A);

	while (1)
	{
		/* code */
	}
}

unsigned int I2S2_ext_REG_ADDRESS[9] = 
{
	0x40003400,0x40003404,0x4000340C,0x40003410,0x40003414,0x40003418,0x4000341C,0x40003420,0x40003408
};

unsigned int I2S2_ext_REG_RESET[9] =
{
	0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002
};

unsigned int I2S2_ext_W_FF_VALE[9] =
{
	
	0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000fbf,0x000003ff,0x00000002
};

unsigned int I2S2_ext_W_00_VALE[9] =
{
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082
};

unsigned int I2S2_ext_W_A5_VALE[9] =
{
	0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x000005a5,0x000001a5,0x00000080
};

unsigned int I2S2_ext_W_5A_VALE[9] =
{
	0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000a1a,0x0000025a,0x00000080
}; 

void SPI_W(unsigned int data)
{
	*((int*)0x40023844) |= 0x00001000;  //SPI1时钟使能
	printf("ENABLE SPI1 RCC\r\n");

	int check = 0;
	unsigned int expected = 0;
	for (int i = 8; i >= 0; i--)
	{
		*((int*)I2S2_ext_REG_ADDRESS[i]) = data;

		unsigned int vale = *((int*)I2S2_ext_REG_ADDRESS[i]);
		if(data == 0xFFFFFFFF)
		{
		expected = I2S2_ext_W_FF_VALE[i];
		}
		else if(data == 0x00000000)
		{
		expected = I2S2_ext_W_00_VALE[i];
		}
		else if(data == 0xA5A5A5A5)
		{
		expected = I2S2_ext_W_A5_VALE[i];
		}
		else if(data == 0x5A5A5A5A)
		{
		expected = I2S2_ext_W_5A_VALE[i];
		}	
		if(vale == expected)
		{
		printf("0x%08x wirte 0x%08x success,vale = 0x%08x\r\n",I2S2_ext_REG_ADDRESS[i],data,vale);
		}
		else
		{
		printf("0x%08x wirte 0x%08x fail,vale = 0x%08x,expected value = 0x%08x\r\n",I2S2_ext_REG_ADDRESS[i],data,vale,expected);
		check += 1; 
		}

		*((int*)I2S2_ext_REG_ADDRESS[i][j]) = I2S2_ext_REG_RESET[i][j];
		printf("Reset this register,reset value is 0x%08x,Read again value is 0x%08x\r\n",I2S2_ext_REG_RESET[i][j],*((int*)I2S2_ext_REG_ADDRESS[i][j]));

	}
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
	RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
printf("DISABLE SPI1 RCC\r\n");
}
