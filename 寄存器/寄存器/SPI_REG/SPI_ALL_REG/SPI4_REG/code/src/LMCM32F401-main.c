#include "uart_REG.h"
#include "stdio.h"

#include <stm32f4xx.h>
#include <stm32f4xx_flash.h>

void SPI_W(unsigned int data);

int main()
{
	UartStdOutInit();  //uart初始化
	printf("TEXT START\r\n");  

	*((int*)0x40023844) |= 0x00003000;  //SPI1时钟使能
	*((int*)0x40023840) |= 0x0000C000;  //SPI23时钟使能

	SPI_W(0xFFFFFFFF);
	SPI_W(0x00000000);
	SPI_W(0xA5A5A5A5);
	SPI_W(0x5A5A5A5A);

	while (1)
	{
		/* code */
	}
}

unsigned int SPI_REG_ADDRESS[4][9] = 
{
	{0x40013000,0x40013004,0x4001300C,0x40013010,0x40013014,0x40013018,0x4001301C,0x40013020,0x40013008},
	{0x40003800,0x40003804,0x4000380C,0x40003810,0x40003814,0x40003818,0x4000381C,0x40003820,0x40003808},
	{0x40003C00,0x40003C04,0x40003C0C,0x40003C10,0x40003C14,0x40003C18,0x40003C1C,0x40003C20,0x40003C08},
	{0x40013400,0x40013404,0x4001340C,0x40013410,0x40013414,0x40013418,0x4001341C,0x40013420,0x40013408}
};

unsigned int SPI_REG_RESET[4][9] =
{
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002},
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002},
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002},
	{0x00000000,0x00000000,0x00000000,0x00000007,0x00000000,0x00000000,0x00000000,0x00000002,0x00000002}
};

unsigned int SPI_W_FF_VALE[4][9] =
{
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000000,0x00000000,0x00000002},
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000fbf,0x000003ff,0x00000002},
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000fbf,0x000003ff,0x00000002},
	{0x0000ffff,0x000000f7,0x00000000,0x0000ffff,0x00000000,0x00000000,0x00000000,0x00000000,0x00000002}
};

unsigned int SPI_W_00_VALE[4][9] =
{
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082},
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082},
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082},
	{0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000082}
};

unsigned int SPI_W_A5_VALE[4][9] =
{
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x000005a5,0x000001a5,0x00000080},
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x000005a5,0x000001a5,0x00000080},
	{0x0000a5a5,0x000000a5,0x00000000,0x0000a5a5,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080}
};

unsigned int SPI_W_5A_VALE[4][9] =
{
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080},
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000a1a,0x0000025a,0x00000080},
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000a1a,0x0000025a,0x00000080},
	{0x00005a5a,0x00000052,0x00000000,0x00005a5a,0x00000000,0x00000000,0x00000000,0x00000000,0x00000080}
}; 

void SPI_W(unsigned int data)
{
	for (int i = 3; i < 4; i++)
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
			printf("Reset this register,reset value is 0x%08x,Read again value is 0x%08x\r\n",SPI_REG_RESET[i][j],*((int*)SPI_REG_ADDRESS[i][j]));
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
}
