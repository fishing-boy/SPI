#include "stm32f4xx.h"
#include "SPI.h"
#include "uart_REG.h"
#include "stdio.h"




int main(void)
{
	UartStdOutInit();
	bsp_SPI1_Init();
	
	
	while(1)
	{
		//printf("1111111111111111111");
	}
}





