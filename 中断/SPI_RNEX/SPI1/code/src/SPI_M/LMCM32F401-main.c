#include "stm32f4xx.h"
#include "spi.h"
#include "uart_REG.h"
#include "stdio.h"

//#define SPI_BUF_LEN 5

u8 ucSPI1_TxBuf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};


void delay(unsigned int t)
{
	while(t--)
	{
		unsigned int te=1000;
		while(te--)
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
}



int main(void)
{
	
	
	UartStdOutInit();
	
	
	static u8 i = 0;
	uint8_t ucSPI1_RxBuf[5] = {0};
	
	bsp_SPI1_Init();
	
	//GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	

	
	
	
	while (1)
	{
		/* 主机发、收数据 */
		for (i = 0; i < 5; i++)
		{

			
			ucSPI1_RxBuf[i] = SPI1_DataTransfer(ucSPI1_TxBuf[i]);
			

			
			delay(10);
		}
		//i = 0;
		//delay(500);
	}
	
//	return 0;
}




