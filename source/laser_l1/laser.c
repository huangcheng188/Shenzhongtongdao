#include "laser.h"
#include "usart3.h"
#include "iot.h"
#include "wifi.h"
laser_struct laser_t ={0,0,0};

extern void laser_get_distance(laser_struct * plaser_t)		//¶ÁÈ¡distance
{
	
	u8 i;
	u8 str[6];
	if(plaser_t->time_updata)
		return;
	plaser_t->time_updata = 1;
	str[0] = 0xA5;
	str[1] = 0x5A;
	str[2] = 0x02;
	str[3] = 0x00;
	str[4] = 0xFD;
	for(i=0; i<5; i++){
		//printf("-0x%02x",str[i]);
		usart3_putc(str[i]);
	}
}
extern void laser_receive_callback(u8 data,laser_struct * plaser_t)		//for uart3
{
	plaser_t->buffer[plaser_t->header] = data;
	plaser_t->header = (plaser_t->header+1)%LASER_BUF_SIZE;
	plaser_t->count++;
	if(plaser_t->count >= LASER_BUF_SIZE){
		printf("*");
		plaser_t->count = 0;
		plaser_t->header = 0;
		plaser_t->tailer = 0;
	}
}

extern u32  laser_distance(u8 *str)
{
	u32 distance;
	if(str[2]&0x80){
		printf("\r\nlaser distance errror");
		distance = 0;
	}else{
		distance = (u32)((str[3]<<32)+(str[4]<<16)+(str[5]<<8)+(str[6]<<0));
		printf("\r\ndistance=%d",distance);
	}
	return distance;
}
static void laser_copy(laser_struct * plaser_t,u8 *str)
{
	u8 i;
	for(i=0; i<8; i++){
		str[i] = plaser_t->buffer[(plaser_t->tailer+i)%LASER_BUF_SIZE];
	}
}
static int laser_check(u8 *str)
{
	u8 i;
	u8 check = 0;
	for(i=0; i<7; i++){
		check ^= str[i];
	}
	if(str[7] ==check)
		return 0;
	else 
		return -1;
}
extern void laser_time(laser_struct *plaser_t)
{
	if(plaser_t->time_updata>0){
		plaser_t->time_updata--;
	}
}

extern int laser_receive_analysis(laser_struct * plaser_t,struct iot_data_struct *piot_data_t)
{
#define MSG_LEN 8
#define HEARD1	0xB4
#define HEARD2	0x69
	u8 i;
	u16 count =plaser_t->count;
	u16 header=plaser_t->header;
	u16 tailer=plaser_t->tailer;
	u8	length=0;
	u8  laser_buf[8];		//laser distance buffer
	char laser_str[10];
	if(count >=MSG_LEN){		//
		if(plaser_t->buffer[(tailer+0)%LASER_BUF_SIZE] == HEARD1 &&
			plaser_t->buffer[(tailer+1)%LASER_BUF_SIZE] == HEARD2){
				laser_copy(plaser_t,laser_buf);
				if(laser_check(laser_buf)){//Ð£Ñé
					printf("\r\ncheck error");
					printf("\r\n");
					for(i=0;i<8;i++)
						printf("0x%x---",(int)(plaser_t->buffer[i]));
					printf("\r\n");
				}else{
					printf("\r\n");
					for(i=0;i<8;i++)
						printf("0x%x---",laser_buf[i]);
					printf("\r\n");
						//laser_distance(laser_buf);
						if(laser_distance(laser_buf))
						if(piot_data_t->iot_flag==SUCCESS){
							iot_send_cmd_data_sprintf("AT+MIPLNOTIFY=0,0,3330,0,5750,1,8,\"%02x%02x%02x%02x\",0,0\r\n",\
							laser_buf[3],laser_buf[4],laser_buf[5],laser_buf[6]);
						}
						else if(piot_data_t->wifi_connect_flag == SUCCESS){
							usart2_putc(laser_buf[3]);
							usart2_putc(laser_buf[4]);
							usart2_putc(laser_buf[5]);
							usart2_putc(laser_buf[6]);
						}
				}
				for(i=0;i<8;i++)
					laser_buf[i] = 0;		//clear buffer
				plaser_t->count = plaser_t->count-MSG_LEN;
				plaser_t->tailer = (plaser_t->tailer+MSG_LEN)%LASER_BUF_SIZE;
		}else{
			plaser_t->count = plaser_t->count-1;
			plaser_t->tailer = (plaser_t->tailer+1)%LASER_BUF_SIZE;
		}
	}else
		return -1;
	return 0;
}

