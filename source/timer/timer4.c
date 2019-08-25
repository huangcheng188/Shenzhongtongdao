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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 			//时钟使能

    TIM_TimeBaseStructure.TIM_Period = 10000/*1000*/; 						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到500为5ms
    TIM_TimeBaseStructure.TIM_Prescaler =(Prescaler-1);				//设置用来作为TIMx时钟频率除数的预分频值  0.1MHZ的计数频率 10US计数器加1
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					//设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 						//定时器中断配置
#if 1
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  				//
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  		//先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  			//从优先级级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
#endif
    TIM_Cmd(TIM4, ENABLE); 											//使能TIMx外设

}
void TIM4_IRQHandler(void)   //TIM4 2ms中断一次
{

	static u8 timer1_10ms=0;
	static u8 timer1_100ms=0;
	static u8 timer1_1000ms=0;
	static u8 flag =0;


	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)		 //检查指定的TIM中断发生与否:TIM 中断源
	{
		if(++timer1_10ms>0){								//100ms out
			timer1_10ms=0;
			iot_time(&iot_data_t);
		}
		if(++timer1_100ms>=10){							//1000ms out
			timer1_100ms=0;
			//printf("1");
			laser_time(&laser_t);
			if(iot_data_t.miplupdate>0)					//设备更新时间
				iot_data_t.miplupdate--;
			if(iot_data_t.connect_time_limit>0)				//设备连接时间
				iot_data_t.connect_time_limit--;
			if(iot_data_t.request_time>0)
				iot_data_t.request_time--;
		}

		if(++timer1_1000ms>=100){						//10000ms out 10s
			timer1_1000ms=0;
			//sensor_t.time_func(&sensor_t);
			
			
		}


	    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  			//清除TIMx的中断待处理位:TIM 中断源
	}
}



