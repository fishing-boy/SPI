#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;

/* Buffer for transmitting data */
uint16_t tx_buffer[2] = {0x1234, 0x5678};

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("test\r\n");

  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  I2S2ext->I2SCFGR &= ~SPI_I2SCFGR_I2SE; 
  I2S2ext->I2SCFGR = 0;
  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0;

  I2S2ext->I2SPR = 0;
  I2S2ext->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2ext->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

  I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SE;

  I2S2ext->CR2 |= SPI_CR2_RXNEIE;
	
	printf("0x%08x,0x%08x,0x%08x\r\n",I2S2ext->I2SCFGR,I2S2ext->I2SPR,I2S2ext->CR2);
	printf("0x%08x,0x%08x,0x%08x\r\n",I2S2->I2SCFGR,I2S2->I2SPR,I2S2->CR2);

  while(1)
  {

  }
}

void SPI2_IRQHandler(void)
{
	if((I2S2ext->SR & ((uint16_t)0x0001)) != (uint16_t)RESET)//检测中断标志位
  {
    printf("rxdata:0x%08x\r\n",I2S2ext->DR);
  }
}
