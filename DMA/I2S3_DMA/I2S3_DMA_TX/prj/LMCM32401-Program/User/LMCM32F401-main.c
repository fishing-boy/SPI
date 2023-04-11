#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;
SPI_TypeDef *I2S3 = SPI3;

/* Buffer for transmitting data */
uint16_t Tx_buffer[10] = {0X0011,0X0022,0X0033,0X0044,0X0055,0X0066,0X0077,0X0088,0X0099,0X00AA};

void dma_Init(void);

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("i2s3 tx dma test\r\n");

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

  I2S3->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S2
  I2S3->I2SCFGR = 0;                  // Reset I2S2 config

  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S2 mode
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S2 master transmitter

  I2S3->I2SPR = 0;     // Reset I2S2 prescaler
  I2S3->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S3->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

  /* Enable I2S2 and I2S3 */
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SE;

  SPI3->CR2 |= SPI_CR2_TXDMAEN;

  dma_Init();

  while(1)
  {
		//while(!(I2S3->SR & SPI_SR_TXE));

    /* Send data over I2S2 */
    //I2S3->DR =1;
  }
}

void dma_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI3->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(Tx_buffer);
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 10;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

void putdata(uint16_t *buff,int len)
{
  while(len--)
  {
    printf("0x%08x,",*buff);
    buff++;
  }
  printf("\r\n");
}

