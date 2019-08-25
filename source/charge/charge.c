#include "charge.h"
#include "led.h"
#include "stdio.h"
//����߼�
//���ʱIO(PA7) �ߵ�ƽ / �����IO(PA7) �͵�ƽ
//���ʱred green ������green ��

charge_struct charge_t={CHARGE_ON};
void charge_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   	//72M/6=12,ADC���ʱ�䲻�ܳ���14M
	//adc 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1; 
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE); 

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	//interrupt
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING/*GPIO_Mode_IPD*/;//�����ػ���mcu
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;         		 //�ⲿ�ж�ͨ��11
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      //�ⲿ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;    //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                   //ʹ��
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;   //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;                  //�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
}
//2580 ��Ӧ8.4 v ��Ϊ����
//1850 ��Ӧ6.0 v ��Ϊ�͵�
u16 charge_battay_adc_value_get(void)
{
	u16 TempValue;
 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5 );
	ADC_Cmd(ADC1, ENABLE);    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	delay_us(20);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	TempValue = ADC_GetConversionValue(ADC1); 
	ADC_TempSensorVrefintCmd(DISABLE);
	ADC_Cmd(ADC1, DISABLE);   
	//TempValue = TempValue >> 4;
	return TempValue;
}

u8 charge_handler( charge_struct *pcharge_t)
{
	u16 adc_val;
	static u8 flag=0;
	switch(pcharge_t->charge_mode){
		case CHARGE_ON://PA7 
			if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 1){
				delay_ms(200);
				if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 1){
					pcharge_t->charge_mode = CHARGE_ON_WAIT;
				}
			}
			break;
		case CHARGE_ON_WAIT:
			if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 0){
				pcharge_t->charge_mode = CHARGE_ON;
			}else{
				pcharge_t->charge_mode = CHARGE_ON_OK;
			}
			break;
		case CHARGE_ON_OK:
			pcharge_t->charge_mode = CHARGE_OFF;
			printf("\r\ncharge on...");
			led_contrl(LED_ALL_ON);//���״̬��+  ��(���������߻��ǿ���״̬)
			break;
		case CHARGE_OFF:
			if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 0){
				delay_ms(500);
				if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 0){
					pcharge_t->charge_mode = CHARGE_OFF_WAIT;
				}
			}
			adc_val = charge_battay_adc_value_get();
			if(flag ==0 && adc_val >= (2580-20)){		//��Ϊ����
				flag = 1;
				led_contrl(LED_DERRN);//�̵�
				printf("\r\nadc=%d",adc_val);
			}else if(flag ==0 && adc_val <= (1850-20)){
				flag = 0;
				led_contrl(LED_ALL_ON);//�̵�
				printf("\r\nadc=%d",adc_val);
			}
			break;
		case CHARGE_OFF_WAIT:
			if(GPIO_ReadInputDataBit(GPIOA,  GPIO_Pin_7) == 1){
				pcharge_t->charge_mode = CHARGE_OFF;
			}else{
				pcharge_t->charge_mode = CHARGE_OFF_OK;
			}
			break;
		case CHARGE_OFF_OK:
			pcharge_t->charge_mode = CHARGE_ON;
			printf("\r\ncharge off");
			delay_ms(100);
			// 1.û�г����ֶ��ε�2. �����Զ��ϵ� ֻ�ðε�������
			if(GPIO_ReadInputDataBit(GPIOC,  GPIO_Pin_4) == 0){//����״̬��
				led_contrl(LED_RED);
				printf("\r\ncharge on key push down");
			}else{//ֱ������
				led_contrl(LED_ALL_OFF);
				printf("\r\ncharge off enter sleep!!!");
				PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);//stop mode 
				SystemInit();//SetSysClockTo72();
				printf("\r\nmcu wake up form charge!!!");
			}
			
			break;
	}	
}

void EXTI9_5_IRQHandler(void) /*�жϻ���*/
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET){
	    printf("\r\nCharge!!!");
	    EXTI_ClearITPendingBit(EXTI_Line7);  /*����жϱ��*/
	}
}
