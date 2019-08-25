#include "rj45.h"
#include "stdio.h"
#include "delay.h"
#include "uart4.h"
#include "iot.h"
//note 每次都是22 字节读写
struct rj45_struct rj45_t;

int  rj45_modbus_read(short reg, short num,int mask,u8 *buf)//buf = NULL
{
	int i;
	short datalen;
	u8 cmd_len;
	u8 temp_buf[50];
	cmd_len = 0x06;
	temp_buf[0] = HEARD0;
	temp_buf[1] = HEARD0;
	temp_buf[2] = HEARD0;
	temp_buf[3] = HEARD1;
	temp_buf[4] = HEARD2;
	temp_buf[5] = cmd_len;
	temp_buf[6] =0x01;
	temp_buf[7] =0x03;
	temp_buf[8] =(u8)(reg>>8);	//寄存器
	temp_buf[9] =(u8)(reg&0xff);
	temp_buf[10] =(u8)(num>>8);	//寄存器数量
	temp_buf[11] =(u8)(num&0xff);
	for(i=0; i<12; i++){
		uart4_putc(temp_buf[i]);
	}
	rj45_t.modbus_t.client_read_flag = 1;
	while(++i<=50000){
		if(rj45_t.modbus_t.client_read_flag==0){
			//处理读返回的数据
			break;
		}
	}
	
	//receive data for server and handler
#if 1
	printf("\r\nread=");
	for(i=0; i<12; i++){
		printf("-0x%x",temp_buf[i]);
	}
#endif
}
int rj45_modbus_write(short reg, short num,int mask,int *buf)//buf = NULL
{
	u8 i;
	u8 data_len;
	u8 cmd_len;
	u8 temp_buf[100];

	data_len = 2*num;
	cmd_len = data_len+7;

	temp_buf[0] = HEARD0;
	temp_buf[1] = HEARD0;
	temp_buf[2] = HEARD0;
	temp_buf[3] = HEARD1;
	temp_buf[4] = HEARD2;
	temp_buf[5] = cmd_len;//总长度
	temp_buf[6] = 0x01;
	temp_buf[7] = 0x10;
	temp_buf[8] = (u8)(reg>>8);	//寄存器
	temp_buf[9] = (u8)(reg&0xff);
	temp_buf[10] = (u8)(num>>8);	//寄存器数量
	temp_buf[11] = (u8)(num&0xff);
	temp_buf[12] = data_len;
	for(i=0;i<num;i++){
		temp_buf[2*i+14] = buf[i]&0x00ff;//数据的处理
		temp_buf[2*i+13] = (buf[i]&0xff00) >> 8;
	}
	for(i=0;i<cmd_len+6;i++){
		uart4_putc(temp_buf[i]);
	}
	rj45_t.modbus_t.client_write_flag= 1;
	
#if 1
	printf("\r\nwrite=");
	for(i=0;i<cmd_len+6;i++){
		printf("-0x%02x",temp_buf[i]);
	}
	
#endif
}

void rj45_receive_callback(u8 data,struct rj45_struct *prj45_t)
{
	prj45_t->buffer[prj45_t->header] = data;
	prj45_t->header = (prj45_t->header+1)%RJ45_BUF_LEN;
	prj45_t->count++;
	if(prj45_t->count >= RJ45_BUF_LEN){
		printf("&");
		prj45_t->count = 0;
		prj45_t->header = 0;
		prj45_t->tailer = 0;
	}
}
extern int rj45_receive_analysis(struct rj45_struct *prj45_t)
{
#define MSG_LEN 10

	u8 i;
	u16 count =prj45_t->count;
	u16 header=prj45_t->header;
	u16 tailer=prj45_t->tailer;
	u8 length=0;
	
	if(count >=MSG_LEN){
		if(prj45_t->buffer[(tailer+0)%RJ45_BUF_LEN] == HEARD0 &&
			prj45_t->buffer[(tailer+1)%RJ45_BUF_LEN] == HEARD0 &&
			prj45_t->buffer[(tailer+2)%RJ45_BUF_LEN] == HEARD0 &&
			prj45_t->buffer[(tailer+3)%RJ45_BUF_LEN] == HEARD1 &&
			prj45_t->buffer[(tailer+4)%RJ45_BUF_LEN] == HEARD2){

			length = prj45_t->buffer[(tailer+5)%RJ45_BUF_LEN];//不包括自身剩余的数位
			length +=6;//头(5) + 自身(1)
			if(count>=length){		//a frame is ok
				for(i=0;i<length;i++){
					prj45_t->buffer_temp[i] = prj45_t->buffer[(prj45_t->tailer+i)%RJ45_BUF_LEN];
				}
#if 1
				printf("\r\nrj45=");
				for(i=0;i<length;i++)
						printf("0x%x---",(int)(prj45_t->buffer_temp[i]));
					
#endif
				rj45_handler(prj45_t);
				prj45_t->count=prj45_t->count-length;
				prj45_t->tailer=(prj45_t->tailer+length)%RJ45_BUF_LEN;
			}else{
				return 1;//wait for a frame
			}
			
		}else{
			prj45_t->count = prj45_t->count-1;
			prj45_t->tailer = (prj45_t->tailer+1)%RJ45_BUF_LEN;
		}
	}else{
		return -1;
	}
	return 0;
}
extern void rj45_handler(struct rj45_struct *prj45_t)
{
	//prj45_t->buffer_temp[] 
	//来自server 数据的读写处理 同时还有处理自己client 数据读写后server回复
	u8 i;
	u8 length = prj45_t->buffer_temp[8/*5*/] ;//获取长度 =0x2c  44个字节按协议拼接并
	u8 *temp=prj45_t->buffer_temp;
	if(prj45_t->buffer_temp[6] == 0x01 && prj45_t->buffer_temp[7] == 0x03){
		//if(length!= 0x1C/*0x2c*/)
			//return ;
		//实际应用打包上报数据格式如下
		printf("\r\n123");
		/*iot_send_cmd_data_sprintf("AT+MIPLNOTIFY=0,0,3330,0,5750,1,88,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\",0,0\r\n",\
						temp[9],temp[10],temp[11],temp[12],
						temp[13],temp[14],temp[15],temp[16],
						temp[17],temp[18],temp[19],temp[20],
						temp[21],temp[22],temp[23],temp[24],
						temp[25],temp[26],temp[27],temp[28],
						temp[29],temp[30],temp[31],temp[32],
						temp[33],temp[34],temp[35],temp[36],
						temp[37],temp[38],temp[39],temp[40],
						temp[41],temp[42],temp[43],temp[44],
						temp[45],temp[46],temp[47],temp[48],
						temp[49],temp[50],temp[51],temp[52]);*/
		#if 0
		/*iot_send_cmd_data_sprintf("AT+MIPLNOTIFY=0,0,3330,0,5750,1,56,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\",0,0\r\n",\
						temp[9],temp[10],temp[11],temp[12],
						temp[13],temp[14],temp[15],temp[16],
						temp[17],temp[18],temp[19],temp[20],
						temp[21],temp[22],temp[23],temp[24],
						temp[25],temp[26],temp[27],temp[28],
						temp[29],temp[30],temp[31],temp[32],
						temp[33],temp[34],temp[35],temp[36]
						/*temp[37],temp[38],temp[39],temp[40],
						temp[41],temp[42],temp[43],temp[44],
						temp[45],temp[46],temp[47],temp[48],
						temp[49],temp[50],temp[51],temp[52]*/);*/
		#endif
		if(iot_data_t.iot_flag == SUCCESS )
		iot_send_cmd_data_sprintf("AT+MIPLNOTIFY=0,0,3330,0,5750,1,64,\"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\",0,0\r\n",\
						temp[9],temp[10],temp[11],temp[12],
						temp[13],temp[14],temp[15],temp[16],
						temp[17],temp[18],temp[19],temp[20],
						temp[21],temp[22],temp[23],temp[24],
						temp[25],temp[26],temp[27],temp[28],
						temp[29],temp[30],temp[31],temp[32],
						temp[33],temp[34],temp[35],temp[36],
						temp[37],temp[38],temp[39],temp[40]
						/*temp[41],temp[42],temp[43],temp[44]
						temp[45],temp[46],temp[47],temp[48],
						temp[49],temp[50],temp[51],temp[52]*/);
		else if( iot_data_t.wifi_connect_flag == SUCCESS){
			for(i=0;i<32;i++)
				usart2_putc(prj45_t->buffer_temp[9+i]);
		}
		
	}
}

