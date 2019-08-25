
#include "usart2.h"
#include "iot.h"
#include "wifi.h" 
void usart2_init(u32 bound)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef  NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        
	NVIC_Init(&NVIC_InitStructure); 

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);                    //使能串口1 

	printf("\r\nusart2_init");

}
void usart2_putc(u8 chr){
	USART_SendData(USART2, chr);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)!=SET);
}
extern void usart2_send_string(u8 *str)
{
	while(*str !='\0'){
		usart2_putc(*str);
		str++;
	}
}

extern int flag_ttt;
extern u8 uart4_data;
void USART2_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE)!=RESET){
		//add your code
		data = USART_ReceiveData(USART2);
		wifi_receive_callback(data, &wifi_t);
		//printf("-%d",data);
		uart4_data = data;
		flag_ttt = 1;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

