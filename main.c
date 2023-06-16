
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <system_timetick.h>
GPIO_InitTypeDef GPIO_InitStruct;
void systick_init(void);
void DelayMillis(void);
void DelayMs(unsigned long t);
void GPIOConfig (void);
void spi_init(unsigned short spi);
char spi_tx(unsigned short spi, char tx_char);
void Sort_values(int32_t A[], uint8_t L);
void IntToStr4(int32_t u, uint8_t *y);
void TIM3_Config(void);
void init_main(void);
volatile uint8_t buffer[7];
int32_t sample[10];
uint8_t  y[8];
int32_t temp=0;
int32_t avg=0;
float see;
float value=0;
int32_t kg;
#define NS       10         // Number of samples to get from HX711
#define SRT       4          // Samples removed after sorting, 4=(2 highest & 2 lowest)
#define		BUFF_SIZE			8
uint8_t i=0;
int main(void){
	  systick_init();
	  GPIOConfig ();
		spi_init(1);
		init_main();
		/* Enable SysTick at 10ms interrupt */
	SysTick_Config(SystemCoreClock/100);
	while(1){
				DelayMs(100);
				spi_tx(1,0x00);
				buffer[0]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[1]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[2]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[3]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[4]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[5]=spi_tx(1,0xAA);
				spi_tx(1,0x00);
				buffer[6]=spi_tx(1,0x80);
				temp = ~((buffer[0] << 16) + (buffer[1] << 8) + buffer[2]);
	      temp=temp*(-1);
				
				for (i=0;i<10;i++){
					sample[i]=temp;
				}
				 Sort_values(sample, NS);
  
				avg = 0;
  for(i = SRT/2; i < NS-SRT/2; i++)
  {
    avg += sample[i];
  }
  avg /= (NS-SRT);
	value=avg;
	value=16724944-value;
	see=value;
	if(   0<value && value <9950){    //calib 0-200g
	value=value/50;
}
	else if(9951<value && value <10179){
	
		value=value-8979;								// calib 200-250g xx 
		value/=4.8;
	}
	else if(10179<value && value <12310){
	
		value=value+441;								// calib 250-300g xx 
		value/=42.5;
	}
	else if(12311<value && value<13280){   //calib 300-400g
		value=value-9434;
		value/=9.6;
	}
	else if(13281<value&&value<144545){    // calibe 400-450g xx
		value+=1036800;
		value/=2625.2;
	}
	else if(144546<value&&value<145160){    // calibe 450-500g xx
		value-=138960;
		value/=12.4;
	}
	else if(145161<value&&value<147650){	//calib 500-600g
		value=value-132672;
		value/=25;
	}
	else if(147651<value&&value<148370){	//calib 600-650gxx
		value-=134467;
		value/=21.4;
	}
	else if(148371<value&&value<156640){	//calib 650-700gxx
		value-=40923;
		value/=165.3;
	}
	else if(156641<value&&value<157600){	//calib 700-800g
		value-=149916;
		value/=9.6;
	}
	else if(157601<value&&value<160470){  //calib 800-900g
		value-=134628;
		value/=28.71;
	}
	else if(160471<value&&value<193490){  //calib 900-950g xx
		value+=433854;
		value/=660.4;
	}
	else if(193491<value&&value<193750){  //calib 950-1000g xx
		value-=188928;
		value/=4.8;
	}
	else if(193751<value&&value<194320){  //calib 1000-1100 
		value-=188048;
		value/=5.7;
	}
	else if(194321<value&&value<196820){  //calib 1100-1200g 
		value-=166884;
		value/=25;
	}
	else if(196821<value&&value<197570){  //calib 1200-1250g xx
		value-=178668;
		value/=15.12;
	}
	else if(197571<value&&value<205840){  //calib 1250-1300g xx
		value+=9132;
		value/=165.4;
	}
	else if(205840<value&&value<206790){  //calib 1300-1400g 
		value-=193512;
		value/=9.48;
	}
	else if(206791<value&&value<209060){  //calib 1400-1450g
		value-=143196;
		value/=45.42;
	}
	else if(209061<value&&value<209770){  //calib 1450-1500g
		value-=191394;
		value/=12.18;
	}
	else if(value<0){
		value=0;
	}
	else if(value>209771){
		value=1500;
	}
	if(tick_count == 100){
			tick_count = 0;
			kg=(int32_t) value;
			IntToStr4(kg,y);
			DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
			DMA1_Stream4->NDTR = BUFF_SIZE;
			DMA_Cmd(DMA1_Stream4, ENABLE);
			//while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
			//USART_SendData(UART4,tx_data);
			//while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
			//USART_SendData(UART4,(uint8_t)66);
		}		 
	}

}
void systick_init(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 0x00FFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL |= 5;
}

void DelayMillis(void)
{
	SysTick->LOAD=0x29040;
	SysTick->VAL = 0;
	while((SysTick->CTRL & 0x00010000) == 0);
}

void DelayMs(unsigned long t)
{
	for(;t>0;t--)
		{
			DelayMillis();
		}
}
void GPIOConfig (void)
{
	RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock
	
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14)|(1<<8);  // Alternate functions for PA5, PA6, PA7 and Output for PA9
	
	GPIOA->OSPEEDR |= (1<<10)|(1<<12)|(1<<14)|(1<<8);  // HIGH Speed for PA5, PA6, PA7, PA9
	
	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
}
void spi_init(unsigned short spi){
				RCC->APB2ENR |= (1<<12);
				SPI1->CR1 |= 0x4; // Master Mode
				SPI1->CR1 |= 0x31; // fclk / 265
				SPI1->CR2 |= 0x4;
				SPI1->CR1 |= 0x40;
				
}
char spi_tx(unsigned short spi, char tx_char)
{

	char rx_val;
	if (spi ==1)
	{
		GPIOA->ODR ^= (1<<4);
		SPI1->DR = tx_char;
		while(SPI1->SR & 0x80){};
		while(SPI1->SR & 0x01){rx_val=SPI1->DR;}
		GPIOA->ODR |= (1<<4);
		
	}
	return rx_val;
}
void Sort_values(int32_t A[], uint8_t L)
{
  uint8_t i = 0;
  uint8_t status = 1;
 
  while(status == 1)
  {
    status = 0;
    for(i = 0; i < L-1; i++)
    {
     if (A[i] > A[i+1])
      {
        A[i]^=A[i+1];
        A[i+1]^=A[i];
        A[i]^=A[i+1];
        status = 1;    
      }
    }
  }
}
void IntToStr4(int32_t u, uint8_t *y)
{
	int16_t a;
	a = u;
	if (a < 0)
	{
		a = -a;
		y[0] = '-';
	}
	else y[0] = ' ';
	
	y[4] = a % 10 + 0x30;
	a = a/10;
	y[3] = a % 10 + 0x30;
	a = a/10;
	y[2] = a % 10 + 0x30;
	a = a/10;
	y[1] = a + 0x30;
}
void init_main(void)
{
  GPIO_InitTypeDef 	GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure;   
  DMA_InitTypeDef  	DMA_InitStructure;
	
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	
  /* Connect UART4 pins to AF2 */  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); 

  /* GPIO Configuration for UART4 Tx */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIO Configuration for USART Rx */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
       
  /* USARTx configured as follow:
		- BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(UART4, &USART_InitStructure);

  /* Enable USART */
  USART_Cmd(UART4, ENABLE);
	
	
		/* Enable UART4 DMA */
  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); 
	
	
	/* DMA1 Stream4 Channel4 for UART4 Tx configuration */			
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)y;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = BUFF_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);
  DMA_Cmd(DMA1_Stream4, ENABLE);
}
void TIM3_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM3_TimeBase;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
  TIM_TimeBaseStructInit(&TIM3_TimeBase); 
  TIM3_TimeBase.TIM_Period        = 2000; // Trigger = CK_CNT/(49+1) = 2kHz
  TIM3_TimeBase.TIM_Prescaler     = 4200;          // CK_CNT = 42MHz/420 = 100kHz
  TIM3_TimeBase.TIM_ClockDivision = 0;
  TIM3_TimeBase.TIM_RepetitionCounter=0;
	TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
	
  TIM_Cmd(TIM3, ENABLE);
}
