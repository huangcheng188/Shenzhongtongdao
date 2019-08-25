#include "stdio.h"
#include "key.h"
#include "led.h"
#include "system_stm32f10x.h"
#include "power.h"
#include "iot.h"
enum{
	KEY_UP=0,		//初始值
	KEY_UP_WAIT,
	KEY_UP_OK,
	KEY_DOWM,
	KEY_DOWN_WAIT,
	KEY_DOWN_OK,
};

key_struct key_t={KEY_UP};

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;						//pb0 acc check(input push)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU/*GPIO_Mode_IPD*/;	//input down/*up*/
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling/*EXTI_Trigger_Rising*/;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
int key_handler(key_struct * pkey_t)
{
	switch(pkey_t->key_mun){
		case KEY_DOWM://pc4=1
			if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 0){
				delay_ms(1000);
				if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 0){
					pkey_t->key_mun = KEY_DOWN_WAIT;
				}
			}
			break;
		case KEY_DOWN_WAIT:
			if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 1){
				pkey_t->key_mun = KEY_DOWM;
			}else{
				pkey_t->key_mun = KEY_DOWN_OK;
				
			}
			break;
		case KEY_DOWN_OK:
			pkey_t->key_mun = KEY_UP;
			printf("\r\nkey down");//wake up mcu
			iot_data_struct_init();
			power_on_all();
			printf("\r\nmcu wake up !!!");
			led_contrl(LED_RED);
			break;

		case KEY_UP:
			if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 1){
				delay_ms(1000);
				if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 1){
					pkey_t->key_mun = KEY_UP_WAIT;
				}
			}
			break;
		case KEY_UP_WAIT:
			if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 0){
				pkey_t->key_mun = KEY_UP;
			}else{
				pkey_t->key_mun = KEY_UP_OK;
			}
			break;
		case KEY_UP_OK:
			pkey_t->key_mun = KEY_DOWM;
			power_off_all();
			printf("\r\nkey up");//mcu enter sleep mode 一开始上电进入休眠
			if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 1){
				printf("\r\ncharge now!!!");
				led_contrl(LED_ALL_ON);
			}else{
				printf("\r\nmcu enter sleep mode!!!");
				led_contrl(LED_ALL_OFF);
				PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);//stop mode 
				SystemInit();//SetSysClockTo72();
				printf("\r\nmcu wake up form key!!!");
			}
			break;
	}

	
}
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET){
		printf("\r\nI");
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

