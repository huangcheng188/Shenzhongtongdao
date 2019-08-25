
#include "usart3.h"
#include "laser.h"

  
void usart3_init(u32 bound)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef  NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	laser power enable
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //laser power
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        
	NVIC_Init(&NVIC_InitStructure); 

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); //��ʼ������1

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���1 

	printf("\r\nusart3_init");

}
/*static*/ void usart3_putc(u8 chr){
	USART_SendData(USART3, chr);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC)!=SET);
}
//��Ҫ���ô˺����߼�����
extern void usart3_send_string(u8 *str)
{
	while(*str!='\0'){
		usart3_putc(*str);
		//intf("0x%x",*str);
		str++;
	}
}

void USART3_IRQHandler(void)
{
	u8 data;
	if(USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET){
		//add your code
		data = USART_ReceiveData(USART3);
		laser_receive_callback(data, &laser_t);//put data into laser_t.buffer[]
		//printf("%d",data);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

