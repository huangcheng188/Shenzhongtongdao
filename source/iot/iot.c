#include "string.h"
#include "stdarg.h"
#include "usart2.h"
#include "delay.h"
#include "iot.h"
#include "led.h"
#include "rj45.h"
static int global_cnt=0;//注册MIPLADDOBJ 

struct iot_struct iot_t={0,0,0};
struct iot_data_struct iot_data_t={0,5,50,5,0,10,/*30*/3,0,3,0};

extern void iot_data_struct_init(void)
{
	iot_data_t.state = 0;		//状态
	iot_data_t.time = 5;		//计时链接网络
	iot_data_t.time_heart_updata = 50;//更新心跳数据
	iot_data_t.time_heart_set = 5;//获取平台设置的时间(心跳)
	iot_data_t.iot_flag = 0;	//开机标志联网标志
	iot_data_t.miplupdate = 10;//设备存活时间更新
	iot_data_t.connect_time_limit = 180;//设备联网时间3min 之内，否则数据走wif
	iot_data_t.wifi_connect_flag = 0;//0 error !0 success
	iot_data_t.request_time = 3;
	iot_data_t.request_flag = 0;
	iot_t.header = 0;
	iot_t.tailer= 0;
	iot_t.count= 0;
}
extern void iot_buffer_receiver(struct iot_struct * piot_t, u8 data)
{
	piot_t->buffer[piot_t->header] = data;
	piot_t->header = (piot_t->header+1)%IOT_BUF_LEN;
	piot_t->count++;
	if(piot_t->count >= IOT_BUF_LEN){
		printf("$=%d",piot_t->count);
		piot_t->count = 0;
		piot_t->header = 0;
		piot_t->tailer = 0;
	}
	return ;
}
void iot_conversion_can_data(struct iot_struct *piot_t)
{
	u8 i;
	CanTxMsg can_tx_msg;
	u8 data[9];
	u8 *str;
	//if(piot_t->buffer_temp)
	if(piot_t->buffer_temp[0]=='9' && piot_t->buffer_temp[1]==','&&iot_data_t.iot_flag == SUCCESS){
		str = piot_t->buffer_temp+2;
		for(i=0; i<9;i++){
			data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
		}
		/*printf("\r\n");
		for(i=0; i<9;i++){
			printf("%d ",data[i]);
		}*/

		can_tx_msg.StdId = (uint32_t)data[0];
		can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
		can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
		can_tx_msg.DLC = 8;
		for(i = 0; i < can_tx_msg.DLC; i++)
	        	can_tx_msg.Data[i] = data[i+1];
		CAN_Transmit(CAN1, &can_tx_msg);
		}
		
}
extern int iot_handler(struct iot_struct *piot_t,function_ptr func_ptr)
{
	
	//function_ptr func;
#define LEN_MIN 6
#define		HEADER1		'\r'
#define		HEADER2		'\n'
	static u8 header_flag=0;
	static u32 tailer_flag=0;		//第一次收到0D 0A 后tailer 的位置
	static u32 count_flag= 0;
	static u32 cnt=0;

	u32 i;
	u32 header,tailer,count;
	header = piot_t->header;
	tailer = piot_t->tailer;
	count = piot_t->count;

	/*if(count >= LEN_MIN)*/{
		//if(count >= LEN_MIN-2)
		if(header_flag==1){
			if(piot_t->buffer[(tailer+0)%IOT_BUF_LEN] == HEADER1\
				&&piot_t->buffer[(tailer+1)%IOT_BUF_LEN] == HEADER2){
				header_flag = 0;		//重新判断新的开始
				//新的tailer 位置和记录的位置
				//printf("\r\nlen=%d",count_flag);
				//printf("\r\n22222tailer=%d",(int)piot_t->tailer);
				for(i=0; i< count_flag; i++){
					piot_t->buffer_temp[i] = piot_t->buffer[(tailer_flag+i)%IOT_BUF_LEN];
				}
				printf("\r\nR=%s",piot_t->buffer_temp);
				//printf("\r\nheader=%d",(int)piot_t->header);
				//printf("\r\ntailer=%d",(int)piot_t->tailer);
				//printf("\r\ncount=%d",(int)piot_t->count);
				iot_analysis(piot_t->buffer_temp,NULL,&iot_data_t);
				iot_commad_analysis_respond(piot_t);

				for(i=0; i< 200; i++)
					piot_t->buffer_temp[i]  = 0x00;

				piot_t->count =piot_t->count-2;
				piot_t->tailer = (piot_t->tailer+2)%IOT_BUF_LEN;
				count_flag=0;


			}else{
				count_flag++;		//记录个数(有效数据)
				//goto wait;
				piot_t->count -=1;
				piot_t->tailer = (piot_t->tailer+1)%IOT_BUF_LEN;

			}
		}
		if(count >= LEN_MIN)
			if(header_flag==0)
				if(/*header_flag==0 && */(piot_t->buffer[(tailer+0)%IOT_BUF_LEN] == HEADER1)\
					&& (piot_t->buffer[(tailer+1)%IOT_BUF_LEN] == HEADER2)){		//第一次接收到到0D 0A

					//printf("\r\nheader=%d",(int)piot_t->header);
					//printf("\r\ntailer=%d",(int)piot_t->tailer);
					//printf("\r\ncount=%d",(int)piot_t->count);

					count_flag=0;

					header_flag = 1;		//第一次接收到头
					piot_t->count -=2;
					piot_t->tailer = (piot_t->tailer+2)%IOT_BUF_LEN;
					tailer_flag = piot_t->tailer%IOT_BUF_LEN;		//记录第一位置澹(不包含头)
					//printf("\r\n-----------tailer_flag=%d",tailer_flag);
					delay_ms(100);//避免一帧数据没有接收完整

				}else{
					//printf("%c",piot_t->buffer[piot_t->tailer]);
					wait:
					piot_t->count -=1;
					piot_t->tailer = (piot_t->tailer+1)%IOT_BUF_LEN;
					return -2;
				}
	}/*else{
		return -1;
	}*/
}

/*brief :iot send cmd
 *param :u8 *
 *retuen:none
 */
extern void iot_send_cmd(u8 * cmd)
{
	while(*cmd != '\0'){
		uart5_putc(*cmd);
		cmd ++;
	}
	uart5_putc(0x0D);
	uart5_putc(0x0A);
}

/*brief :iot send cmd & data throght sprintf
 *param :char *
 *retuen:none
 */
extern void iot_send_cmd_data_sprintf(const char *format,...)	//传输带参数的
{
	u8 buf[256]={0};
	va_list va;
	va_start(va,format);
	vsprintf(buf, format, va);
	va_end(va);
	uart5_send_string(buf);		//send cmd & data
}
//return -1 num return 0-5 abcdef
static int string_is_num(char num)
{
	int i;
	char ascii[6] = {'a','b','c','d','e','f'};
	if(num>=97){
		for(i=0;i<6;i++){
			if(ascii[i]==num)
				return i;
		}
	}else
		return -1;//是一个字符数字
}
char ascii_num[6] = {10,11,12,13,14,15};
static void iot_can_conversion(char *str)
{
	u8 i;
	CanTxMsg can_tx_msg;
	u8 data[9];
	
	if(iot_data_t.iot_flag == SUCCESS){//连接网络成功
		for(i=0; i<9;i++){
			//data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
			if(string_is_num(str[2*i])<0){		//num -- num char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = (str[2*i]-'0')*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}else if(string_is_num(str[2*i])>=0){//mun --char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}
		}
		printf("\r\n");
		for(i=0; i<9;i++){
			printf("%d ",data[i]);
		}

		can_tx_msg.StdId = (uint32_t)data[0];
		can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
		can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
		can_tx_msg.DLC = 8;
		for(i = 0; i < can_tx_msg.DLC; i++)
	        	can_tx_msg.Data[i] = data[i+1];
		CAN_Transmit(CAN1, &can_tx_msg);
	}
}
static void iot_can_conversion_1(char *str)
{
	u8 i;
	CanTxMsg can_tx_msg;
	u8 data[12];
	
	if(iot_data_t.iot_flag == SUCCESS){//连接网络成功
		for(i=0; i<12;i++){
			//data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
			if(string_is_num(str[2*i])<0){		//num -- num char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = (str[2*i]-'0')*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}else if(string_is_num(str[2*i])>=0){//mun --char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}
		}
		printf("\r\n");
		for(i=0; i<12;i++){
			printf("%d ",data[i]);
		}

		can_tx_msg.StdId = (uint32_t)((data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3]);
		can_tx_msg.IDE = CAN_Id_Standard; // 标准帧
		can_tx_msg.RTR = CAN_RTR_Data;  // 数据帧
		can_tx_msg.DLC = 8;
		for(i = 0; i < can_tx_msg.DLC; i++)
	        	can_tx_msg.Data[i] = data[i+4];
		CAN_Transmit(CAN1, &can_tx_msg);
		printf("\r\n三一重工ID=%d",can_tx_msg.StdId);
		printf("\r\ndata=");
		for(i=0; i<8;i++){
			printf("%d ",data[4+i]);
		}
	}
}


static void iot_client_write_tcp_data(char * str)
{
	u8 i;
	u8 data[44];	//str[88] 44byte str[0]str[1] = 1byte
	int write_buf[22]={0};//往下写44 byte
	printf("\r\nval=%s",str);
	if(iot_data_t.iot_flag == SUCCESS){//连接网络成功收到数据再发送
		for(i=0; i<44;i++){
			if(string_is_num(str[2*i])<0){		//num -- num char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = (str[2*i]-'0')*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}else if(string_is_num(str[2*i])>=0){//mun --char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}
		}
		printf("\r\n");
		for(i=0; i<44;i++){
			printf("0x%02x-",data[i]);
		}
		for(i=0;i<22;i++){
			write_buf[i] = data[2*i]<<8;
			write_buf[i] |= data[2*i+1];
		}
			
		rj45_modbus_write(0x0000, 22, 1,write_buf);
	}


	
	return;
}
// 44 字节分两次发送0-9(short) 字节 发送10-21(short)另外发送
static void iot_client_write_tcp_data_1(char * str,u8 num)
{
	u8 i,j;
	u8 data[44];	//str[88] 44byte str[0]str[1] = 1byte
	int write_buf[22]={0};//往下写44 byte
	printf("\r\nval=%s",str);
	j=num;//
	//if(j!=40 && j!=44)
		//return ;
	if(iot_data_t.iot_flag == SUCCESS){//连接网络成功收到数据再发送
		for(i=0; i<j;i++){
			if(string_is_num(str[2*i])<0){		//num -- num char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = (str[2*i]-'0')*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}else if(string_is_num(str[2*i])>=0){//mun --char
				if(string_is_num(str[2*i+1])<0){//num -- num char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){//mun --char
					data[i] = ascii_num[string_is_num(str[2*i])]*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}
		}
		printf("\r\n");
		for(i=0; i<j;i++){
			printf("0x%02x-",data[i]);
		}
		for(i=0;i<(j/2);i++){
			write_buf[i] = data[2*i]<<8;
			write_buf[i] |= data[2*i+1];
		}
		if(j==40)
			rj45_modbus_write(0x0000, 10, 1,write_buf);
		else
			//rj45_modbus_write(0x000A, 12, 1,write_buf);
			rj45_modbus_write(0x0000, 12, 1,write_buf);

		//rj45_modbus_write(0x0000, 22, 1,write_buf);
	}


	
	return;
}
static void iot_laser_distanc_send_to_rj45(char * str)
{
	u8 i;
	u8 data[4];
	int write_buf[2]={0,0};
	
	
	
	if(iot_data_t.iot_flag == SUCCESS){//连接网络成功收到数据再发送
		for(i=0; i<4;i++){
			if(string_is_num(str[2*i])<0){		//num -- num char
				if(string_is_num(str[2*i+1])<0){
					data[i] = (str[2*i]-'0')*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){
					data[i] = (str[2*i]-'0')*16+ascii_num[string_is_num(str[2*i+1])];
				}
			}else if(string_is_num(str[2*i])>=0){
				if(string_is_num(str[2*i+1])<0){
					data[i] = ascii_num[string_is_num(str[2*i])]*16+(str[2*i+1]-'0');
				}else if(string_is_num(str[2*i+1])>=0){
					data[i] = ascii_num[string_is_num(str[2*i])]*16+ascii_num[string_is_num(str[2*i])];
				}
			}
		}
		printf("\r\n");
		for(i=0; i<4;i++){
			printf("0x%x ",data[i]);
		}
	
		//send distance to rj45
		write_buf[0] = data[0]<<8;
		write_buf[0] |= data[1];
		write_buf[1]=data[2]<<8;
		write_buf[1]|=data[3];
		printf("\r\nlaser=0x%04x  0x%04x\r\n",write_buf[0],write_buf[1]);
		rj45_modbus_write(0x0000, 0x0002, 1,write_buf );
	}
}
void iot_commad_analysis_respond(struct iot_struct *piot_t)
{
	//piot_t->buffer_temp 数据
	int write_buf[1];
	
	char temp[100] = {0};
	char *str_test = "\"word\"";
	char *str;
	char *event;//获取消息命令
	char *ref=NULL;char *mid=NULL;char *objid=NULL;char *insid=NULL;char *resid=NULL;//read 指令参数
	char *type;char *len;char *value;char *flag;// write 指令参数同时还需要read指令参数
	str=(char *)strstr((const char *)piot_t->buffer_temp,"+MIPLREAD:");// AT+MIPLREADRSP
	if(str){
		event = strtok(str,":");
		ref = strtok(NULL,",");
		mid = strtok(NULL,",");
		objid = strtok(NULL,",");
		insid = strtok(NULL,",");
		resid = strtok(NULL,",");
		printf("\r\nstrtok=%s %s %s %s %s",ref,mid,objid,insid,resid);
		/*sprintf(temp,"AT+MIPLREADRSP=%s,%s,%d,%s,%d,%s,%d,%d,%s,%d,%d\r\n",ref,mid,1,objid,0,resid,1,4,str_test,0,0);
		printf("\r\ntemp=%s!!!",temp);	*/		
		if(strstr(objid,"3200")){//read heart data
			iot_send_cmd_data_sprintf("AT+MIPLREADRSP=%s,%s,%d,%s,%d,%s,%d,%d,%s,%d,%d\r\n"\
			,ref,mid,1,objid,0,resid,1,1,"\"A\"",0,0);
		}else
			iot_send_cmd_data_sprintf("AT+MIPLREADRSP=%s,%s,%d,%s,%d,%s,%d,%d,%s,%d,%d\r\n"\
			,ref,mid,1,objid,0,resid,1,4,str_test,0,0);//test
	}
	str=(char *)strstr((const char *)piot_t->buffer_temp,"+MIPLWRITE:");// AT++MIPLWRITERSP
	if(str){
		event = strtok(str,":");
		ref = strtok(NULL,",");
		mid = strtok(NULL,",");
		objid = strtok(NULL,",");
		insid = strtok(NULL,",");
		resid = strtok(NULL,",");
		type= strtok(NULL,",");
		len= strtok(NULL,",");
		value= strtok(NULL,",");
		flag= strtok(NULL,",");
		printf("\r\nstrtok=%s %s %s %s %s %s %s %s %s",ref,mid,objid,insid,resid,type,len,value,flag);
		/*sprintf(temp,"AT+MIPLWRITERSP=0,%s,2",mid);
		printf("\r\temp=n%s",temp);*/
		iot_send_cmd_data_sprintf("AT+MIPLWRITERSP=0,%s,2\r\n",mid);//write rsp 
		if(strstr(objid,"3200")){//设置心跳时间
			iot_data_t.time_heart_set = atoi(value);
			printf("\r\nset heart time=%d",atoi(value));
		}else if(strstr(objid,"3201")){//can 数据下发到泵机
			if(strstr(len,"18")){//必须18 位9个字节ID+Data(8)
				iot_can_conversion(value);
			}else if(strstr(len,"24")){//add 20190818 三一重工
				iot_can_conversion_1(value);
			}
		}else if(strstr(objid,"3330")){
			/*if(strstr(len,"8"))
				iot_laser_distanc_send_to_rj45(value);*/
			if(strstr(len,"2")){//开关0开 1 关
				/*if(strstr(value,"01")){	
					write_buf[0]=0x0001;
				}else if(strstr(value,"00")){
					write_buf[0]=0x0000;
				}
				//rj45_modbus_write(0x0000, 0x0001, 1,write_buf ); //暂时没用
				printf("\r\non of");*/
			}
			if(strstr(len,"24")){//rj45 收到client 端请求数据
				//if(strstr(value,"000000ffaa0601030000000e"))
					//rj45_modbus_read(0x0000, 14/*22*/, 1, NULL);
				if(strstr(value,"000000ffaa06010300000010")){
					iot_data_t.request_flag = 1;
					rj45_modbus_read(0x0000, 16/*22*/, 1, NULL);
					printf("\r\nrequst rj45 start");
				}else if(strstr(value,"000000ffaa06010300000000")){
					//rj45_modbus_read(0x0000, 16/*22*/, 1, NULL);
					iot_data_t.request_flag = 0;
					printf("\r\nrequst rj45 end");
				}
			}
			if(strstr(len,"88")){//rj45 写服务器端
				iot_client_write_tcp_data(value);
			}
			if(strstr(len,"40")){//rj45 激光数据(包括校验)
				//printf("\r\nlaser data");
				//iot_client_write_tcp_data_1(value, 40);
			}
			if(strstr(len,"48")){//其他数据
				printf("\r\nothers data");
				iot_client_write_tcp_data_1(value, 44);

			}
			
		}
	}
}
extern void iot_analysis(u8 *str1,u8 *str2,struct iot_data_struct *piot_data_t)
{
	u8 *str;
	u8 ptr[10];

	switch(piot_data_t->state){
		case IOT_AT_WATI:						//AT 指令测试
			str=(u8 *)strstr((char *)str1,"OK");
			if(str){
				piot_data_t->state = IOT_AT_CFUN;	//测试成功进入下一项
				printf("\r\nAT OK");
			}
			break;
		case IOT_AT_CFUN_WATI:					//查询相关的指令忽略执行返回值OK
			str=(u8 *)strstr((char *)str1,"+CFUN:");
			if(str){
				if(str[6] == '1'){					//射频打开
					printf("\r\nCFUN=1  radio frequency open");
					piot_data_t->state = IOT_AT_IMEI;
					printf("\r\nAT_CFUN? OK");
				}
				else
					printf("\r\nCFUN=0  radio frequency close!!!");
			}
			break;
		case IOT_AT_IMEI_WAIT:
			str=(u8 *)strstr((char *)str1,"+CGSN:");
			if(str){
				piot_data_t->state = IOT_AT_IMSI;
				printf("\r\nAT_IMEI(设备号) OK");
			}
			break;
		case IOT_AT_IMSI_WAIT:
			str=(u8 *)strstr((char *)str1,"4");		//需要修改判断0-9之间认为ok
			if(str){
				piot_data_t->state = IOT_AT_CSQ;
				printf("\r\nAT_IMSI (卡号)OK");
			}
			break;

		case IOT_AT_CSQ_WAIT:
			str=(u8 *)strstr((char *)str1,"+CSQ:");
			if(str){
				printf("\r\nCSQ=%s",&str[5]);
				piot_data_t->state = IOT_AT_CGATT;
				printf("\r\nAT_CSQ OK");
			}
			break;
			
		case IOT_AT_CGATT_WAIT:
			str=(u8 *)strstr((char *)str1,"+CGATT:1");
			if(str){
				piot_data_t->state = IOT_AT_CEREG;
				printf("\r\nAT_CGATT OK");
				
			}
			break;

		case IOT_AT_CEREG_WAIT:
			str=(u8 *)strstr((char *)str1,"+CEREG:0,1");
			if(str){
				piot_data_t->state = IOT_AT_MIPLCREATE;
				printf("\r\nAT_CEREG OK");
			}
			break;
		case  IOT_AT_MIPLCREATE_WAIT:
			str=(u8 *)strstr((char *)str1,"+MIPLCREATE");
			if(str){
				piot_data_t->state = IOT_AT_MIPLADDOBJ;
				printf("\r\nAT_MIPLCREATE OK");
			}
			str=(u8 *)strstr((char *)str1,"ERROR");		//已经注册返回error 重启设备和mcu
			if(str){
				iot_send_cmd("AT+NRB");
				piot_data_t->state = IOT_AT;//需要更新有些参数!!!
				printf("\r\nAT+NRB reboot nb-iot !!!");
				delay_ms(500);
			}
			break;
		case  IOT_AT_MIPLADDOBJ_WAIT:
			str=(u8 *)strstr((char *)str1,"OK");
			
			if(str){
				global_cnt--;
				if(global_cnt == 0){//接受ok 数量对应
					piot_data_t->state = IOT_AT_MIPLNOTIFY;
					printf("\r\nAT_MIPLADDOBJ OK");
				}
			}
			break;
		case  IOT_AT_MIPLNOTIFY_WAIT:
			str=(u8 *)strstr((char *)str1,"OK");
			if(str){
				global_cnt--;
				if(global_cnt == 0){//接受ok 数量对应
					piot_data_t->state = IOT_AT_MIPLOPEN;
					printf("\r\nAT_MIPLNOTIFY OK");
				}
			}
			break;
		case  IOT_AT_MIPLOPEN_WAIT:
			str=(u8 *)strstr((char *)str1,"+MIPLEVENT:0,6");
			if(str){
				piot_data_t->state = IOT_AT_NEXT;
				piot_data_t->iot_flag = SUCCESS;
				piot_data_t->miplupdate = MIPLUPDATE;// 60s x 10 = 600s-30s = 9.5min 提前30s  更新
				printf("\r\nAT_MIPLOPEN OK");
			}
			break;
		case IOT_AT_NEXT:
			break;
		default:
			printf("\r\niot_analysis error!!!");
	}
	
	str=(u8 *)strstr((char *)str1,"+MIPLEVENT:0,11");
	if(str){
		printf("\r\n存活时间更新成功");
	}
	str=(u8 *)strstr((char *)str1,"+MIPLEVENT:0,15");
	if(str){
		printf("\r\nRESET MCU!!!");
		__set_FAULTMASK(1);//关闭总中断
    		NVIC_SystemReset();//请求单片机重启
	}
}
extern int get_iot_message_length(u8 *str)
{
	int data=0;
	data=str[0]-'0';
	if(str[1]==','){
		printf("\r\n111data=%d",data);
		return data;
	}
	else{
		data = data*10+(str[1]-'0');
		printf("\r\n222data=%d",data);
	}
	return data;
}

extern void iot_init_handler(struct iot_data_struct *piot_data_t)
{
	switch(piot_data_t->state){
		case  IOT_AT:							//测试AT指令模式模块是否存在
			iot_send_cmd("AT");		
			printf("\r\nAT test");
			piot_data_t->state=IOT_AT_WATI;	//进入等待模式
			piot_data_t->time = 10;			//等待时间10*100ms
			break;
		case  IOT_AT_WATI:					//AT 指令的等待模式
			if(piot_data_t->time==0){			//超时再次进入测试模式
				piot_data_t->state=IOT_AT;
				printf("\r\nAT test again");
			}
			break;
		case IOT_AT_CFUN:
			iot_send_cmd("AT+CFUN?");		//
			printf("\r\nAT+CFUN?");
			piot_data_t->state=IOT_AT_CFUN_WATI;
			piot_data_t->time = 10;
			break;
		case IOT_AT_CFUN_WATI:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_CFUN;
				printf("\r\nAT+CFUN? test again");
			}
			break;
		case IOT_AT_IMEI:					//设备号
			iot_send_cmd("AT+CGSN=1");
			printf("\r\nAT+CGSN=1");
			piot_data_t->state=IOT_AT_IMEI_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_IMEI_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_IMEI;
				printf("\r\nAT+CGSN=1 test again");
			}
			break;
		case IOT_AT_IMSI:
			iot_send_cmd("AT+CIMI");			//查询IMSI 码(卡号)
			printf("\r\nAT+CIMI");
			piot_data_t->state=IOT_AT_IMSI_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_IMSI_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_IMSI;
				printf("\r\nAT+CIMI test again");
			}
			break;
		case IOT_AT_CSQ:
			iot_send_cmd("AT+CSQ");			//测试AT指令
			printf("\r\nAT+CSQ");
			piot_data_t->state=IOT_AT_CSQ_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_CSQ_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_CSQ;
				printf("\r\nAT+CSQ test again");
			}
			break;
		case IOT_AT_CGATT:
			iot_send_cmd("AT+CGATT?");		//测试网络激活状态
			printf("\r\nAT+CGATT?");
			piot_data_t->state=IOT_AT_CGATT_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_CGATT_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_CGATT;
				printf("\r\nAT+CGATT? test again");
			}
			break;
		case IOT_AT_CEREG:
			iot_send_cmd("AT+CEREG?");		//测试确认网络注册成功
			printf("\r\nAT+CEREG?");
			piot_data_t->state=IOT_AT_CEREG_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_CEREG_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_CEREG;
				printf("\r\nAT+CEREG? test again");
			}
			break;
		case IOT_AT_MIPLCREATE:				//模组侧设备创建
			iot_send_cmd("AT+MIPLCREATE=49,130031F10003F2002304001100000000000000123138332E3233302E34302E34303A35363833000131F30008C000000000,0,49,0");
			printf("\r\nAT+MIPLCREATE=49,130......");
			piot_data_t->state=IOT_AT_MIPLCREATE_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_MIPLCREATE_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_MIPLCREATE;
				printf("\r\nAT+MIPLCREATE=49,130...... test again");
			}
			break;
		case IOT_AT_MIPLADDOBJ:
			//iot_send_cmd("AT+MIPLADDOBJ=0,3303,1,\"1\",1,0");//test 3303 对应温度
			iot_send_cmd("AT+MIPLADDOBJ=0,3200,1,\"1\",1,0");//heart
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLADDOBJ=0,3201,1,\"2\",1,0");//can
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLADDOBJ=0,3202,1,\"3\",1,0");//rj45
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLADDOBJ=0,3330,1,\"4\",1,0");//laser
			delay_ms(100);
			global_cnt++;
			printf("\r\nAT+MIPLADDOBJ");
			piot_data_t->state=IOT_AT_MIPLADDOBJ_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_MIPLADDOBJ_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_MIPLADDOBJ;
				printf("\r\nAT+MIPLADDOBJ test again");
			}
			break;
		case IOT_AT_MIPLNOTIFY:
			//iot_send_cmd("AT+MIPLNOTIFY=0,0,3303,0,5700,4,4,\"19.8\",0,0");// 浮点类型

			//iot_send_cmd("AT+MIPLNOTIFY=0,0,3200,0,5750,1,4,\"zyzt\",0,0");
			//心跳object 3200(Digital input) resource 5526(mode) 应用可以设置心跳时间默认5ms 心跳数据为mode数据
			iot_send_cmd("AT+MIPLNOTIFY=0,0,3200,0,5750,1,5,\"heart\",0,0");
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLNOTIFY=0,0,3201,0,5750,1,3,\"can\",0,0");
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLNOTIFY=0,0,3202,0,5750,1,4,\"rj45\",0,0");
			delay_ms(100);
			global_cnt++;
			iot_send_cmd("AT+MIPLNOTIFY=0,0,3330,0,5750,1,5,\"laser\",0,0");
			delay_ms(100);
			global_cnt++;
			printf("\r\nAT+MIPLNOTIFY");
			piot_data_t->state=IOT_AT_MIPLNOTIFY_WAIT;
			piot_data_t->time = 10;	
			break;
		case IOT_AT_MIPLNOTIFY_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_MIPLNOTIFY;
				printf("\r\nAT+MIPLNOTIFY test again");
			}
			break;
		case IOT_AT_MIPLOPEN:
			//iot_send_cmd("AT+MIPLOPEN=0,3000,30");//由于注册时间为3000S,如需长期在线，需要定时更新在线时间
			iot_send_cmd("AT+MIPLOPEN=0,600,30");
			printf("\r\nAT+MIPLOPEN");
			//delay_ms(1000);
			piot_data_t->state=IOT_AT_MIPLOPEN_WAIT;
			piot_data_t->time = 10;
			break;
		case IOT_AT_MIPLOPEN_WAIT:
			if(piot_data_t->time==0){
				piot_data_t->state=IOT_AT_MIPLOPEN;
				printf("\r\nAT+MIPLOPEN test again");
			}
			break;
		
		case IOT_AT_NEXT:
			if(piot_data_t->miplupdate)
				return;
			piot_data_t->miplupdate = MIPLUPDATE;
			iot_send_cmd("AT+MIPLUPDATE=0,600,1");
			break;
		default:
			printf("piot_data_t->state error!!!");
			break;
	}
}

extern void iot_time(struct iot_data_struct *piot_data_t)
{
	if(piot_data_t->time>0)
		piot_data_t->time--;
	if(piot_data_t->time_heart_updata>0)
		piot_data_t->time_heart_updata--;
}

