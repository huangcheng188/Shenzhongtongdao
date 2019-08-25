//E103-W02 wifi 
/*
	如果iot  3  分钟没有连接上
	切换成wifi  进行数据传输(wifi 默认打开)
	测试激光192.168.1.100-180使用
	can                 192.168.1.181-190
	rj45			 192.168.1.191-200
*/
#include "wifi.h"
#include "usart2.h"
#include "can.h"
#include "iot.h"
struct wifi_struct wifi_t={0,0,0};
struct wifi_data_struct wifi_data_t={0};

void wifi_init(void)
{
	usart2_init(115200);
}
extern  void wifi_receive_callback(u8 data,struct wifi_struct * pwifi_t)
{
	pwifi_t->buffer[pwifi_t->header] = data;
	pwifi_t->header = (pwifi_t->header+1)%WIFI_BUF_LEN;
	pwifi_t->count++;
	if(pwifi_t->count >= WIFI_BUF_LEN){
		printf("#");
		pwifi_t->count = 0;
		pwifi_t->header = 0;
		pwifi_t->tailer = 0;
	}
}
static void wifi_data_copy(struct wifi_struct * pwifi_t,u8 len)
{	
	u8 i;
	for(i=0;i<len;i++){
		pwifi_t->buffer_temp[i]=pwifi_t->buffer[(pwifi_t->tailer+i)%WIFI_BUF_LEN];
		//printf("-0x%02x",pwifi_t->buffer_temp[i]);
	}
}
static void wifi_data_send_to_can_devices(struct wifi_struct * pwifi_t)
{
	u8 i;
	u8 data[9];
	CanTxMsg can_tx_msg;

	if(pwifi_t->buffer_temp[2] == 9){
		can_tx_msg.StdId = (uint32_t)pwifi_t->buffer_temp[3];
		can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
		can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
		can_tx_msg.DLC = 8;
		for(i = 0; i < 8; i++)
	        	can_tx_msg.Data[i] = pwifi_t->buffer_temp[i+4];
		CAN_Transmit(CAN1, &can_tx_msg);
		printf("\r\ncan set ");
	}
}
static void wifi_data_request_rj45_data(struct wifi_struct * pwifi_t)
{
	//aa550c0000000ffaa06010300000010
	if(pwifi_t->buffer_temp[2] == 12 && pwifi_t->buffer_temp[14] == 0x10){
		iot_data_t.request_flag = 1;
		rj45_modbus_read(0x0000, 16/*22*/, 1, NULL);
		printf("\r\nrequst rj45 start");
	}else if(pwifi_t->buffer_temp[2] == 12 && pwifi_t->buffer_temp[14] == 0x00){
		//aa550f000000ffaa06010300000000"))
		iot_data_t.request_flag = 0;
		printf("\r\nrequst rj45 stop");
	}
}
static void wifi_data_set_rj45(struct wifi_struct * pwifi_t)
{
	u8 i,j;
	u8 data[24];
	int write_buf[12]={0};
	
	if(pwifi_t->buffer_temp[2] == 24){
		for(i=0; i<24;i++)
			data[i] = pwifi_t->buffer_temp[3+i];
	}
#if 0		
	printf("\r\nRJ45 set");
	for(i=0; i<24;i++){
		printf("0x%02x-",data[i]);
	}
#endif
	for(i=0;i<(12);i++){
		write_buf[i] = data[2*i]<<8;
		write_buf[i] |= data[2*i+1];
	}
	rj45_modbus_write(0x0000, 12, 1,write_buf);

	return;	
}

extern int wifi_receive_analysis(struct wifi_struct * pwifi_t)
{
#define LEN 12
	u8 i;
	u16 count =pwifi_t->count;
	u16 header=pwifi_t->header;
	u16 tailer=pwifi_t->tailer;
	u8	length=0;
//	if(pwifi_t->count>=1){
//		printf(" %d 0x%02x",pwifi_t->buffer[(pwifi_t->tailer)],pwifi_t->buffer[(pwifi_t->tailer)]);

//		pwifi_t->count = pwifi_t->count-1;
//		pwifi_t->tailer = (pwifi_t->tailer+1)%WIFI_BUF_LEN;
//	}
//	return 0;
	if(count >= LEN	){		//
		if(pwifi_t->buffer[(tailer+0)%WIFI_BUF_LEN] == 0xAA &&
			pwifi_t->buffer[(tailer+1)%WIFI_BUF_LEN] == 0x55){
				length = pwifi_t->buffer[(tailer+2)%WIFI_BUF_LEN];
				if(count >= length+3){
					wifi_data_copy(pwifi_t, length+3);

					printf("\r\nwifi receive=");
					for(i=0;i<length+3;i++)
						printf("-0x%02x",pwifi_t->buffer_temp[i]);
					
					if(iot_data_t.wifi_connect_flag == SUCCESS){
						wifi_data_send_to_can_devices(pwifi_t);
						wifi_data_request_rj45_data(pwifi_t);
						wifi_data_set_rj45(pwifi_t);
					}
					for(i=0;i<100;i++)
						pwifi_t->buffer_temp[i] = 0;
					pwifi_t->count = pwifi_t->count-length-3;
					pwifi_t->tailer = (pwifi_t->tailer+length+3)%WIFI_BUF_LEN;
					return 0;
				}else{
					return 1;
				}
		}else{
			pwifi_t->count = pwifi_t->count-1;
			pwifi_t->tailer = (pwifi_t->tailer+1)%WIFI_BUF_LEN;
			return -1;
		}
	}else
		return -1;
}
