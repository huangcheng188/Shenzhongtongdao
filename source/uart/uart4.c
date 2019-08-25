#include "uart4.h"
#include "stdio.h"
#include "rj45.h"
extern void uart4_init(u32 baudrate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//UART4_TX   PC.11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//UART4_RX   Pc.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		
	NVIC_Init(&NVIC_InitStructure);


	USART_InitStructure.USART_BaudRate =baudrate; 	 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx; 

	USART_Init(UART4, &USART_InitStructure);
		  
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

	USART_Cmd(UART4, ENABLE);	
	printf("\r\nuart4 init");	
}
void uart4_putc(u8 chr)
{
	USART_SendData(UART4, chr);
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC)!=SET);
}
extern void uart4_send_string(u8 *str)
{
	while(*str !='\0'){
		uart4_putc(*str);
		printf("%x",*str);
		str++;
	}
}
extern int flag;
extern u8 uart4_data;
void UART4_IRQHandler(void)
{
	u8 receive_data;
	//printf("4");
	if(USART_GetITStatus(UART4, USART_IT_RXNE)!=RESET){
    		receive_data=(u8)USART_ReceiveData(UART4);
		//printf("%c",receive_data);
		rj45_receive_callback(receive_data, &rj45_t);
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
	}
}

