//project stm32f103 + IOT(M5310-A) +WIFI(E103-W02)
//data 2019/04/23
//Huangcheng tel:18824867964 qq:278269949
//

#include <stdio.h>
#include <string.h>
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart5.h"
#include "delay.h"
#include "can.h"
#include "laser.h"
#include "timer4.h"
#include "iot.h"
#include "led.h"
#include "key.h"
#include "rj45.h"
#include "charge.h"
#include "wifi.h"

int flag_ttt=0;
u8 uart4_data;
void bsp_init(void);
CanTxMsg can_tx_msg;
/*	can_tx_msg.StdId = 0x011;
	can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
	can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
	can_tx_msg.DLC = 4;
	for(i = 0; i < can_tx_msg.DLC; i++)
        	can_tx_msg.Data[i] = i;
	ret = CAN_Transmit(CAN1, &can_tx_msg);
	printf("%d",ret);
	delay_ms(1000);
	ret = CAN_Transmit(CAN1, &can_tx_msg);
	printf("%d",ret);
	delay_ms(1000);*/
int main(void)
{	static u8 wifi_flag=0;
	int i,ret,j;
	u8 read_data[15]={0};
	int write_buf[]={0x0,0x0,0x0,0xff,0xaa,0x1f,0x1,0x3,0x1c,0x3,0xe8,0x7,0xd0,0xb,0xb8,0xf,0xa0,0x0,0x1,0x0,0x2,0x0,0x0,0x13,0x88,0x0,0x0,0x0,0x64,0x0,0x0,0x7,0xd0,0x0,0x0,0x3,0xe8};
	bsp_init();

	while(1){
		if(iot_data_t.iot_flag == SUCCESS){//net connect ok 
			if(iot_data_t.time_heart_updata==0){
				iot_data_t.time_heart_updata = iot_data_t.time_heart_set*10;//心跳时间
				//iot_send_cmd("AT+MIPLNOTIFY=0,0,3200,0,5700,1,1,\"H\",0,0");//heart 数据上报
				iot_send_cmd_data_sprintf("AT+MIPLNOTIFY=0,0,3330,0,5750,1,2,\"%02x\",0,0\r\n",\
							0XFF);
			}
			
			//laser_get_distance(&laser_t);//单次测量
		}
		if(iot_data_t.iot_flag == SUCCESS || iot_data_t.connect_time_limit){//iot connect ok or time not over 3minute, iot translate data
			iot_init_handler(&iot_data_t);
			iot_handler(&iot_t,NULL);
		}
		if(iot_data_t.iot_flag != SUCCESS && iot_data_t.connect_time_limit==0){//iot connect error and time over 3 minute,wifi translate data
			if(!wifi_flag){
				iot_data_t.wifi_connect_flag = SUCCESS;//wifi 联网成功标志
				printf("\r\nwifi");
				wifi_flag = 1;
			}
		}
		if(iot_data_t.iot_flag == SUCCESS || iot_data_t.wifi_connect_flag == SUCCESS){
			laser_get_distance(&laser_t);//单次测量
			//add 
			if(iot_data_t.request_time == 0 && iot_data_t.request_flag == 1){
				iot_data_t.request_time = 3;
				rj45_modbus_read(0x0000, 16/*22*/, 1, NULL);
				
			}
		}
		can_analysis(&can1_buffer_t);			
		laser_receive_analysis(&laser_t,&iot_data_t);
		//key_handler(&key_t);
		rj45_receive_analysis(&rj45_t);
		//charge_handler(&charge_t);
		wifi_receive_analysis(&wifi_t);
	if(flag_ttt){
		flag_ttt = 0;
		//printf("\r\n%d   %c",uart4_data,uart4_data);
	}
		if(i++>=500000){
			i=-500000;
			for(i=0;i<10;i++){
				usart2_putc(i);
			}
			//i=0;
			//rj45_modbus_read(0x0000, 14, 1, NULL);
			//printf("\r\nj++=%d",j++);
			//printf("\r\nadc=%d",charge_battay_adc_value_get());
			/*int write_buf[2];
			write_buf[0]=0x0001;
			write_buf[1]=0x0202;
			write_buf[21]=0x0002;
			rj45_modbus_write(0x0002, 2, 1,write_buf );*/

			/*can_tx_msg.StdId = 0x011;
			can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
			can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
			can_tx_msg.DLC = 4;
			for(i = 0; i < can_tx_msg.DLC; i++)
		        	can_tx_msg.Data[i] = i;
			ret = CAN_Transmit(CAN1, &can_tx_msg);*/
			//printf("%d",ret);
		}
		/*if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)==0)
			printf("\r\nio 000000000000000000");*/
			/*if(iot_data_t.iot_flag == SUCCESS){
				for(j=0;j<37;j++){
					rj45_t.buffer_temp[j]=(u8)write_buf[j];
				}
				rj45_handler(&rj45_t);
			}*/
	}
}


void bsp_init(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	usart1_init(115200);		//printf
	//usart2_init(115200);		//wifi
	uart5_init(9600);		//iot 
	uart4_init(115200);	//rj45

	delay_init();
	//iot_io_init();			//iot reset
	can_init();
	usart3_init(38400);		//laser
	timer4_init(720);
	led_init();
	key_init();
	power_init();
	wifi_init();
	charge_init();
}

/*read_data[0] = 0x00;
			read_data[1] = 0x00;
			read_data[2] = 0x00;
			read_data[3] = 0xFF;
			read_data[4] = 0xAA;
			read_data[5] = 0x09;
			read_data[6] = 0x01;
			read_data[7] = 0x10;
			read_data[8] = 0x00;
			read_data[9] = 0x00;
			read_data[10] = 0x00;
			read_data[11] = 0x01;
			read_data[12] = 0x02;
			read_data[13] = 0x00;
			read_data[14] = 0x0c;
			for(j=0; j<15; j++){
				uart4_putc(read_data[j]);
			}*/

			/*read_data[0] = 0x00;
			read_data[1] = 0x00;
			read_data[2] = 0x00;
			read_data[3] = 0xFF;
			read_data[4] = 0xAA;
			read_data[5] = 0x06;
			read_data[6] = 0x01;
			read_data[7] = 0x03;
			read_data[8] = 0x00;
			read_data[9] = 0x00;
			read_data[10] = 0x00;
			read_data[11] = 0x01;
			for(j=0; j<12; j++){
				uart4_putc(read_data[j]);
			}*/

