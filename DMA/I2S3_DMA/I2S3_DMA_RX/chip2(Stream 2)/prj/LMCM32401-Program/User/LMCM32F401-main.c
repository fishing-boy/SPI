#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;
SPI_TypeDef *I2S3 = SPI3;

uint16_t Rx_buffer[10] = {0};

void dma_Init(void);
void putdata(uint16_t *buff,int len);

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("chip2:i2s3 rx dma test\r\n");

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);  

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_Init(GPIOA,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 

  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  /* Configure I2S2 interface */
  I2S3->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
  I2S3->I2SCFGR = 0;

  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0;
  //I2S3->I2SCFGR |= SPI_I2SCFGR_DATLEN;

  I2S3->I2SPR = 0;
  I2S3->I2SPR |= 39;
  I2S3->I2SPR |= SPI_I2SPR_ODD;

	
	I2S3->I2SCFGR |= SPI_I2SCFGR_I2SE;

  I2S3->CR2 |= SPI_CR2_RXNEIE;
  I2S3->CR2 |= SPI_CR2_RXDMAEN;

	
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	//NVIC_Init(&NVIC_InitStructure);

  dma_Init();

  /* Loop to transmit data */
  while(1)
  {
    if(Rx_buffer[9])
    {
      putdata(Rx_buffer,10);
      UartEndSimulation();
    }
  }
}

void SPI3_IRQHandler(void)
{
	if((I2S3->SR & ((uint32_t)0x0001)) != (uint32_t)RESET)//检测中断标志位
  {
    I2S3->CR2 &= ~SPI_CR2_RXNEIE;
    printf("rxdata:0x%08x\r\n",I2S3->DR);
		printf("test pass\r\n");
  }
}

void dma_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream2);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI3->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(Rx_buffer);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE);
}

void putdata(uint16_t *buff,int len)
{
  while(len--)
  {
    printf("0x%08x\r\n",*buff);
    buff++;
  }
  printf("\r\n");
}