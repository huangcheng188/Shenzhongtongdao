/*file name  :timer4.c
 *description:
 *date		 :2071/05/22
 */

#include "timer4.h"
#include "stm32f10x_tim.h"
#include "iot.h" 
#include "laser.h" 


void timer4_init(u16 Prescaler)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef 		NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 			//ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = 10000/*1000*/; 						//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ     ������500Ϊ5ms
    TIM_TimeBaseStructure.TIM_Prescaler =(Prescaler-1);				//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  0.1MHZ�ļ���Ƶ�� 10US��������1
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					//����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 						//��ʱ���ж�����
#if 1
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  				//
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  		//��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  			//�����ȼ���
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  								//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
#endif
    TIM_Cmd(TIM4, ENABLE); 											//ʹ��TIMx����

}
void TIM4_IRQHandler(void)   //TIM4 2ms�ж�һ��
{

	static u8 timer1_10ms=0;
	static u8 timer1_100ms=0;
	static u8 timer1_1000ms=0;
	static u8 flag =0;


	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)		 //���ָ����TIM�жϷ������:TIM �ж�Դ
	{
		if(++timer1_10ms>0){								//100ms out
			timer1_10ms=0;
			iot_time(&iot_data_t);
		}
		if(++timer1_100ms>=10){							//1000ms out
			timer1_100ms=0;
			//printf("1");
			laser_time(&laser_t);
			if(iot_data_t.miplupdate>0)					//�豸����ʱ��
				iot_data_t.miplupdate--;
			if(iot_data_t.connect_time_limit>0)				//�豸����ʱ��
				iot_data_t.connect_time_limit--;
			if(iot_data_t.request_time>0)
				iot_data_t.request_time--;
		}

		if(++timer1_1000ms>=100){						//10000ms out 10s
			timer1_1000ms=0;
			//sensor_t.time_func(&sensor_t);
			
			
		}


	    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  			//���TIMx���жϴ�����λ:TIM �ж�Դ
	}
}



