/*
本工程主要是用于测试F401的SPI部件

用于测试将SPI接口设置为SPI协议的各个功能点
*/
 
#include "SPI.h"
//#include "usart.h"
#include "delay.h"
#include "uart_REG.h"
#include "stdio.h"

#include <stm32f4xx.h>
#include <stm32f4xx_flash.h>

//测试部件选择开关区
#define  LMCM32F401_SPI

int main()
{
#ifdef LMCM32F401_SPI
#ifdef SPI_FULL_HALF
#ifdef SPI_MASTER
	u8 table[]="hello!";	 
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init(84);	    	 //延时函数初始化	  
	uart_init(115200);	 //串口初始化为115200

	SPI_Init_User();  //初始化spi2
	SPI2_SetSpeed(SPI_BaudRatePrescaler_128);//设置spi时钟速度

	while(1)
	{  
		for(i=0;i<6;i++)
		{
			SPI2_WriteByte(table[i]);
			delay_ms(10);
		}
		SPI2_WriteByte('\n');
	}
#endif   //SPI_MASTER	

#ifdef   SPI_SLAVE
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 //串口初始化为115200
	   
	SPI_Init_User();  //初始化spi2
	SPI2_SetSpeed(SPI_BaudRatePrescaler_128);//设置spi时钟速度
	
	while(1)
	{

	}
#endif   //SPI_SLAVE

#endif   //SPI_FULL_HALF
#endif   //LMCM32F401_SPI

#ifdef SPI_REG

	UartStdOutInit();  //uart初始化
	printf("TEXT START\r\n");  

	*((int*)0x40023844) |= 0x00001000;  //SPI1时钟使能
	*((int*)0x40023840) |= 0x0000C000;  //SPI23时钟使能

	Rtc_Init();

	if(*((int*)0x40002854) == 1)
	{
		*((int*)0x40002850) = 0;
		*((int*)0x40002854) = 0;
	}

	switch (*((int*)0x40002850))
	{
	case 0:
		//printf("TEXT W 0XFFFFFFFF\r\n"); 
		SPI1_W_FF();
		SPI1_W_FF_CHECK();
		SPI2_3_W_FF();
		SPI2_3_W_FF_CHECK();
		//output_SPI2_3();
		*((int*)0x40002850) = 1;
		__set_FAULTMASK(1); //STM32程序软件复位
		NVIC_SystemReset();
		break;
	case 1:
		//printf("TEXT W 0X00000000\r\n"); 
		SPI1_W_00();
		SPI1_W_00_CHECK();
		SPI2_3_W_00();
		SPI2_3_W_00_CHECK();
		//output_SPI2_3();
		*((int*)0x40002850) = 2;
		__set_FAULTMASK(1); //STM32程序软件复位
		NVIC_SystemReset();
		break;
	case 2:
		//printf("TEXT W 0xA5A5A5A5\r\n");
		SPI1_W_A5();
		SPI1_W_A5_CHECK();
		SPI2_3_W_A5();
		SPI2_3_W_A5_CHECK();
		//output_SPI2_3();
		*((int*)0x40002850) = 3;
		__set_FAULTMASK(1); //STM32程序软件复位
		NVIC_SystemReset();
		break;
	case 3:
		//printf("TEXT W 0x5A5A5A5A\r\n");
		SPI1_W_5A();
		SPI1_W_5A_CHECK();
		SPI2_3_W_5A();
		SPI2_3_W_5A_CHECK ();
		//output_SPI2_3();
		*((int*)0x40002850) = 4;
		__set_FAULTMASK(1); //STM32程序软件复位
		NVIC_SystemReset();
		break;
	default:
		break;
	}

	while (1)
	{
		*((int*)0x40002854) = 1;
	}

#endif

}



