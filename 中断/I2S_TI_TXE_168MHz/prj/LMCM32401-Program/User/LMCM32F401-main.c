#include "stm32f4xx.h"
#include "stdio.h"
#include "uart_REG.h"

/* I2S2 and I2S3 handles */
SPI_TypeDef *I2S2 = SPI2;
SPI_TypeDef *I2S3 = SPI3;

/* Buffer for transmitting data */
uint16_t tx_buffer[2] = {0x1234, 0x5678};

int main(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	UartStdOutInit();
	printf("i2s txe test\r\n");
	/* Enable GPIO clocks */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->CR |= RCC_CR_PLLI2SON;
  // /* Configure GPIO pins for I2S2 and I2S3 */
  // GPIOA->MODER |= GPIO_MODER_MODER4_1;
  // GPIOB->MODER |= GPIO_MODER_MODER10_1|GPIO_MODER_MODER9_1; // PB10,9 to AF
  // GPIOC->MODER |= GPIO_MODER_MODER3_1|GPIO_MODER_MODER10_1|GPIO_MODER_MODER12_1;  // PC3 to AF

  // GPIOB->AFR[1] |= (5 << 8);  // PB10 to AF5 (I2S2)
  // GPIOC->AFR[0] |= (6 << 12); // PC3 to AF6 (I2S3)
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI3);  
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);

  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10; 
  GPIO_Init(GPIOB,&GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_12; 
  GPIO_Init(GPIOC,&GPIO_InitStructure); 

  /* Enable SPI clocks */
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  /* Configure I2S2 interface */
  I2S2->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S2
  I2S2->I2SCFGR = 0;                  // Reset I2S2 config

  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S2 mode
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S2 master transmitter
  //I2S2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S2 standard Philips I²S

  I2S2->I2SPR = 0;     // Reset I2S2 prescaler
  I2S2->I2SPR |= 39;   // I2S2 prescaler = 40
  I2S2->I2SPR |= SPI_I2SPR_ODD;  // I2S2 odd prescaler

   /* Configure I2S3 interface */
  I2S3->I2SCFGR &= ~SPI_I2SCFGR_I2SE; // Disable I2S3
  I2S3->I2SCFGR = 0;                  // Reset I2S3 config

  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SMOD; // I2S3 mode
  //I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1; // I2S3 slave receiver
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0; // I2S3 standard Philips I²S

  I2S3->I2SPR = 0;     // Reset I2S3 prescaler
  I2S3->I2SPR |= 39;   // I2S3 prescaler = 40
  I2S3->I2SPR |= SPI_I2SPR_ODD;  // I2S3 odd prescaler

  /* Enable I2S2 and I2S3 */
  I2S2->I2SCFGR |= SPI_I2SCFGR_I2SE;
  I2S3->I2SCFGR |= SPI_I2SCFGR_I2SE;

  SPI2->CR2 |= SPI_CR2_TXEIE;
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);

  /* Loop to transmit data */
  while(1)
  {
//    /* Wait for I2S2 to be ready for data */
//    while(!(I2S2->SR & SPI_SR_TXE));

//    /* Send data over I2S2 */
//    I2S2->DR = tx_buffer[0];

//    /* Wait for I2S3 to receive data */
//    while(!(I2S3->SR & SPI_SR_RXNE));

//    /* Read data from I2S3 */
//    uint16_t rx_data = I2S3->DR;
  }
}

void SPI2_IRQHandler(void)
{
	if((SPI2->SR & ((uint16_t)0x0002)) != (uint16_t)RESET)//检测中断标志位
  {
    SPI2->CR2 &= ~SPI_CR2_TXEIE;
		printf("test pass\r\n");
  }
}



// #include "stm32f4xx.h"

// #define I2S2_WS_PIN     GPIO_Pin_12     // IIS2 WS
// #define I2S2_CK_PIN     GPIO_Pin_13     // IIS2 CK
// #define I2S2_SD_PIN     GPIO_Pin_15     // IIS2 SD

// #define I2S3_WS_PIN     GPIO_Pin_4      // IIS3 WS
// #define I2S3_CK_PIN     GPIO_Pin_5      // IIS3 CK
// #define I2S3_SD_PIN     GPIO_Pin_7      // IIS3 SD

// /* Configure I2S */
// static void I2S_Config(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct;
//     I2S_InitTypeDef I2S_InitStruct;

//     /* Enable GPIO clock */
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

//     /* Configure GPIO pins */
//     GPIO_InitStruct.GPIO_Pin = I2S2_WS_PIN | I2S2_CK_PIN | I2S2_SD_PIN;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//     GPIO_Init(GPIOB, &GPIO_InitStruct);

//     GPIO_InitStruct.GPIO_Pin = I2S3_WS_PIN | I2S3_CK_PIN | I2S3_SD_PIN;
//     GPIO_Init(GPIOC, &GPIO_InitStruct);

//     /* Enable SPI/I2S clock */
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

//     /* Configure I2S */
//     I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
//     I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
//     I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
//     I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
//     I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
//     I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
//     I2S_Init(SPI2, &I2S_InitStruct);

//     I2S_InitStruct.I2S_Mode = I2S_Mode_MasterRx;
//     I2S_Init(SPI3, &I2S_InitStruct);

//     /* Configure I2S GPIOs */
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
//     GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_SPI3);
//     GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_SPI3);
//     GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);

//     /* Enable I2S */
//     I2S_Cmd(SPI2, ENABLE);
//     I2S_Cmd(SPI3, ENABLE);
// }

// int main(void)
// {
//     I2S_Config();
    
//      while (1)
//   {
//     /* Read data from IIS2 */
//     IIS_ReadData(IIS2, data, 2);
    
//     /* Print received data */
//     printf("Received from IIS2: %d %d\r\n", data[0], data[1]);
    
//     /* Send data to IIS3 */
//     data[0]++;
//     data[1]++;
//     IIS_SendData(IIS3, data, 2);
    
//     /* Wait for a short delay */
//     HAL_Delay(100);
//   }
// }















// #include "stm32f4xx.h"
// #include "stm32f4xx_spi.h"
// //#include "stm32f4xx_i2s.h"

// void SPI_Configuration(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;
//     SPI_InitTypeDef SPI_InitStructure;

//     /* Enable GPIO clocks */
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

//     /* Enable SPI clock */
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

//     /* Configure SPI pins */
//     GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
//     GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
//     GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStructure);

// SPI_CR2_SSOE

//     /* Configure SPI */
//     SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//     SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//     SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
//     SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//     SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//     SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//     SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//     SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStructure.SPI_CRCPolynomial = 7;
//     SPI_Init(SPI1, &SPI_InitStructure);

//     SPI_Cmd(SPI1, ENABLE);
// }

// void I2S_Configuration(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;
//     I2S_InitTypeDef I2S_InitStructure;

//     /* Enable GPIO clocks */
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

//     /* Enable I2S clock */
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

//     /* Configure I2S pins */
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
//     GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI3);

//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_10;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOB, &GPIO_InitStructure);

// 	/* Configure I2S */
//     I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
//     I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
//     I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
//     I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
//     I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
//     I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
//     I2S_Init(SPI3, &I2S_InitStructure);

//     /* Configure I2S DMA */
//     DMA_InitTypeDef DMA_InitStructure;

//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

//     DMA_InitStructure.DMA_Channel = DMA_Channel_0;
//     DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//     DMA_InitStructure.DMA_BufferSize = AUDIO_BUFFER_SIZE / 2;
//     DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (SPI3->DR);
//     DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) audio_buffer;
//     DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//     DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//     DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//     DMA_Init(DMA1_Stream5, &DMA_InitStructure);

//     DMA_Cmd(DMA1_Stream5, ENABLE);

//     /* Configure interrupts */
//     NVIC_InitTypeDef NVIC_InitStructure;

//     /* Enable DMA1_Stream5 IRQ Channel */
//     NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);

//     /* Enable SPI3 IRQ Channel */
//     NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);

//     /* Enable the SPI3 DMA requests */
//     SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

//     /* Enable the I2S DMA interrupts */
//     DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

//     /* Enable the I2S TXE interrupt */
//     SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);

//     /* Start the I2S data transmission */
//     I2S_Cmd(SPI3, ENABLE);

