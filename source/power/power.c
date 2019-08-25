#include "power.h"

power_struct power_t;

void power_wifi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	power_wifi_enable(ON);
}
void power_wifi_enable(u8 state)
{
	if(state == ON)
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
	else if(state == OFF)
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
}

void power_iot_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//power enable
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
}
void power_iot_enable(u8 state)
{
	if(state == ON)
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
	else
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}
void power_laser_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //laser power
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//∏¥”√Õ∆ÕÏ ‰≥ˆ
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}
void power_laser_enable(u8 state)
{
	if(state == ON)
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}


void power_init(void)
{
	power_wifi_init();
	power_iot_init();
	power_laser_init();
}
void power_on_all(void)
{
	power_wifi_enable(ON);
	power_iot_enable(ON);
	power_laser_enable(ON);
}
void power_off_all(void)
{
	power_wifi_enable(OFF);
	power_iot_enable(OFF);
	power_laser_enable(OFF);
}
