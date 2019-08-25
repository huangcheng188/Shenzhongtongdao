#include"iot_io.h"
#include"iot_config.h"
#include"delay.h"

void iot_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IOT_GPIO_PWR_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IOT_GPIO_PWR_EN_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(IOT_GPIO_PWR_EN_PORT, IOT_GPIO_PWR_EN_PIN);
	delay_ms(100);
	GPIO_SetBits(IOT_GPIO_PWR_EN_PORT, IOT_GPIO_PWR_EN_PIN);
	delay_ms(500);
	
	iot_io_reboot();
}
void iot_io_reboot(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IOT_GPIO_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IOT_GPIO_RESET_PORT, &GPIO_InitStructure);

	GPIO_SetBits(IOT_GPIO_RESET_PORT, IOT_GPIO_RESET_PIN);
	delay_ms(500);
	GPIO_ResetBits(IOT_GPIO_RESET_PORT, IOT_GPIO_RESET_PIN);
	delay_ms(1000);
}
