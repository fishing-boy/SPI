#include "stm32f4xx.h"
#include "uart_REG.h"
#include "stdio.h"

//#define MCU_TEST

uint16_t Tx_buffer[10] = {0X0011,0X0022,0X0033,0X0044,0X0055,0X0066,0X0077,0X0088,0X0099,0X00AA};
void dma_Init(void);
void SPI4_MY_INIT(void);

int main(void)
{
  UartStdOutInit();
  printf("test start\r\n");

  dma_Init();
  SPI4_MY_INIT();

  int i = 10000;
  while(i--);

  while(!DMA2_Stream1->NDTR);

  printf("Send over\r\n");

  RCC->APB2RSTR |= RCC_APB2RSTR_SPI4RST;

  i = 1000;
  while(i--);

  RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI4RST;

  printf("Reset SPI4\r\n");

  printf("SPI4->SR = 0x%08x\r\n",SPI4->SR);
  printf("SPI4->CR1 = 0x%08x\r\nSPI4->CR2 = 0x%08x\r\nSPI4->CRCPR = 0x%08x\r\nSPI4->DR = 0x%08x\r\n",SPI4->CR1,SPI4->CR2,SPI4->CRCPR,SPI4->DR);
	printf("SPI4->I2SCFGR = 0x%08x\r\nSPI4->I2SPR = 0x%08x\r\nSPI4->RXCRCR = 0x%08x\r\nSPI4->TXCRCR = 0x%08x\r\n",SPI4->I2SCFGR,SPI4->I2SPR,SPI4->RXCRCR,SPI4->TXCRCR);
	while(1)
	{
    
	}
}

void dma_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI4->DR));
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

	DMA_Init(DMA2_Stream1, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream1, ENABLE);
}

void SPI4_MY_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_SPI4);

	NVIC_InitStructure.NVIC_IRQChannel = SPI4_IRQn;         // SPI1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);

  SPI4->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SSM | SPI_CR1_SSI ;
  SPI4->CR2 |= SPI_CR2_TXDMAEN;
  SPI4->CR1 |= SPI_CR1_SPE;
}

