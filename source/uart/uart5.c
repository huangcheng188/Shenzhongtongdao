#include "uart5.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "iot.h"

extern void uart5_init(u32 baudrate)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//power enable
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		//USART1_TX   PC.12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		//USART1_RX   PD.2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
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

	USART_Init(UART5, &USART_InitStructure);
		  
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

	USART_Cmd(UART5, ENABLE);	
	printf("\r\nuart5 init");
}
void uart5_putc(u8 chr){
	USART_SendData(UART5, chr);
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC)!=SET);
}
extern void uart5_send_string(u8 *str)
{
	while(*str !='\0'){
		uart5_putc(*str);
		str++;
	}
}
void UART5_IRQHandler(void)
{
	u8 receive_data;
	if(USART_GetITStatus(UART5, USART_IT_RXNE)!=RESET){
    		receive_data=(u8)USART_ReceiveData(UART5);
		iot_buffer_receiver(&iot_t, receive_data);
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
	}
}
