#include "iot.h"
#include "stdio.h"
#include "can.h"
#include "stm32f10x_can.h"
can_buffer_struct can1_buffer_t={0,0,0};

void can_init(void)
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	u32 slave_id;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); //��CANʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);      //��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��IO
	
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	
	CAN_DeInit(CAN1);
    	CAN_StructInit(&CAN_InitStructure);

	//�ر�ʱ�䴥��ģʽ
	CAN_InitStructure.CAN_TTCM = DISABLE;
	//�ر��Զ����߹���
	CAN_InitStructure.CAN_ABOM = DISABLE;
	//�ر��Զ�����ģʽ
	CAN_InitStructure.CAN_AWUM = DISABLE;
	//��ֹ�����Զ��ش�
	CAN_InitStructure.CAN_NART = DISABLE; //
	//FIFO ���ʱ���ĸ���Դ�ļ�
	CAN_InitStructure.CAN_RFLM = DISABLE;
	//���ķ������ȼ�ȡ����ID��
	CAN_InitStructure.CAN_TXFP = DISABLE;
	//��������ģʽ
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	//���ò����� 500 kBPs    //CAN������=APB����Ƶ��/BRP��Ƶ��/(1+tBS1+tBS2)
	//         = 36/9/(CAN_SJW_1tq+CAN_BS1_3tq+CAN_BS2_2tq)
	//         =36/9/(1+5+2)=0.5M=500K
	// CAN_SJW_1tq  CAN_BS1_3tq  CAN_BS2_2tq �ڼ���ʱ��Ҫ��1
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq/*CAN_BS1_5tq*/;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 9; //ABP����ʱ��ʱ��9��Ƶ

	if(CAN_Init(CAN1, &CAN_InitStructure) == CAN_InitStatus_Success){
		printf("\r\nCAN_Init success");
	}else{
		printf("\r\nCAN_Init failed");
	}
	
	slave_id=0X51;
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode  =CAN_FilterMode_IdMask;		//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale =CAN_FilterScale_32bit;		//������λ��32bit
	CAN_FilterInitStructure.CAN_FilterIdHigh=slave_id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow =0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;			//��������ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

slave_id=0X52;
	CAN_FilterInitStructure.CAN_FilterNumber=1;
	CAN_FilterInitStructure.CAN_FilterMode  =CAN_FilterMode_IdMask;		//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale =CAN_FilterScale_32bit;		//������λ��32bit
	CAN_FilterInitStructure.CAN_FilterIdHigh=slave_id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow =0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;			//��������ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
slave_id=0X53;
	CAN_FilterInitStructure.CAN_FilterNumber=2;
	CAN_FilterInitStructure.CAN_FilterMode  =CAN_FilterMode_IdMask;		//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale =CAN_FilterScale_32bit;		//������λ��32bit
	CAN_FilterInitStructure.CAN_FilterIdHigh=slave_id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow =0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;			//��������ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
slave_id=0X54;
	CAN_FilterInitStructure.CAN_FilterNumber=3;
	CAN_FilterInitStructure.CAN_FilterMode  =CAN_FilterMode_IdMask;		//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale =CAN_FilterScale_32bit;		//������λ��32bit
	CAN_FilterInitStructure.CAN_FilterIdHigh=slave_id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow =0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;			//��������ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
slave_id=0X55;
	CAN_FilterInitStructure.CAN_FilterNumber=4;
	CAN_FilterInitStructure.CAN_FilterMode  =CAN_FilterMode_IdMask;		//��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale =CAN_FilterScale_32bit;		//������λ��32bit
	CAN_FilterInitStructure.CAN_FilterIdHigh=slave_id<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow =0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;			//��������ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
extern int flag;
extern CanRxMsg can_rx_msg;

int can_analysis(can_buffer_struct * pcan1_buffer_t)
{
	u8 i;
	CanRxMsg *pcan_rx_msg=NULL;
	if(pcan1_buffer_t->count>0){
		pcan_rx_msg=&pcan1_buffer_t->buffer[pcan1_buffer_t->tailer];
#if 1
		printf("\r\n%2d",pcan1_buffer_t->count);
		for(i=0;i<pcan_rx_msg->DLC;i++){
			printf("-0x%2x",pcan_rx_msg->Data[i]);
		}
#endif
		if(iot_data_t.iot_flag == SUCCESS)
		/*iot_send_cmd_data_sprintf("AT+NMGS=%d,%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n"\
		,9,pcan_rx_msg->StdId,pcan_rx_msg->Data[0],pcan_rx_msg->Data[1],pcan_rx_msg->Data[2],\
		pcan_rx_msg->Data[3],pcan_rx_msg->Data[4],pcan_rx_msg->Data[5],pcan_rx_msg->Data[6],\
		pcan_rx_msg->Data[7]);*/
		//can1_frame_analysis(pcan_rx_msg);
		pcan1_buffer_t->tailer=(pcan1_buffer_t->tailer+1)%CAN1_BUFFER_SIZE;
		pcan1_buffer_t->count -=1;
		return 0;
	}else{
		return -1;
	}
}


void USB_LP_CAN1_RX0_IRQHandler(void)  //CAN�����ж�
{
	printf("@1");
	//if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!=RESET){
		CAN_Receive(CAN1, CAN_FIFO0, &can1_buffer_t.buffer[can1_buffer_t.header]);
		can1_buffer_t.header=(can1_buffer_t.header+1)%CAN1_BUFFER_SIZE;
		can1_buffer_t.count++;
		if(can1_buffer_t.count>=CAN1_BUFFER_SIZE){
			can1_buffer_t.count = 0;
			can1_buffer_t.header = 0;
			can1_buffer_t.tailer= 0;
			printf("@1");
		}
		//CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
	//}
}


