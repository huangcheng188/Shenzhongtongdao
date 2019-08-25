/*brief :beep_init
 *param :none
 *return:none
 */
#include "led.h"
#include "delay.h"


extern void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_15); //e810(rj45)   iot  rest   
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	//GPIO_SetBits(GPIOB,GPIO_Pin_15);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5;//red green
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);
	led_contrl(LED_RED);
	//led_contrl(LED_ALL_ON);
}
/*brief :beep contrl
 *param :u8 channel
 *return:u8 adc value(12bit->8bit)
 */
extern void led_contrl(unsigned char cnt)
{
	switch(cnt){
		case LED_RED:
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
		break;
		case LED_DERRN:
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
		break;
		case LED_ALL_OFF:
			GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);
		break;
		case LED_ALL_ON:
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	}
}
