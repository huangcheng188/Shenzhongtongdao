#ifndef _IOT_H
#define _IOT_H
#include "stm32f10x.h"
#define		IOT_BUF_LEN		(4096*10)//2048

#define MIPLUPDATE 570 //9min30s
struct iot_struct {
	u32 header;
	u32 tailer;
	u32 count;
	u8 buffer_temp[200];
	u8 buffer[IOT_BUF_LEN];
	void (*handler)(struct iot_struct * );
};
extern struct iot_struct iot_t;

typedef void(*function_ptr)(u8 *str1,u8 *str2);

enum{
	IOT_AT=0,
	IOT_AT_WATI,
	
	IOT_AT_CFUN,
	IOT_AT_CFUN_WATI,

	IOT_AT_IMEI,		//�豸��
	IOT_AT_IMEI_WAIT,

	IOT_AT_IMSI,
	IOT_AT_IMSI_WAIT,

	IOT_AT_CSQ,
	IOT_AT_CSQ_WAIT,

	IOT_AT_CGATT,		//ȷ�����缤��״̬,1���Ѽ��0��δ����
	IOT_AT_CGATT_WAIT,

	IOT_AT_CEREG,
	IOT_AT_CEREG_WAIT,

	IOT_AT_MIPLCREATE,	//ģ����豸����
	IOT_AT_MIPLCREATE_WAIT,

	IOT_AT_MIPLADDOBJ,
	IOT_AT_MIPLADDOBJ_WAIT,

	IOT_AT_MIPLNOTIFY,
	IOT_AT_MIPLNOTIFY_WAIT,
	
	IOT_AT_MIPLOPEN,
	IOT_AT_MIPLOPEN_WAIT,

	IOT_AT_NEXT,//ok
};
struct iot_data_struct {
	u8 state;		//״̬
	u8 time;		//��ʱ��������
	u16 time_heart_updata;//������������
	u16 time_heart_set;//��ȡƽ̨���õ�ʱ��(����)
	u8 iot_flag;	//������־������־
	volatile u16  miplupdate;//�豸���ʱ�����
	volatile u16  connect_time_limit;//�豸����ʱ��3min ֮�ڣ�����������wif
	vu8 wifi_connect_flag;//0 error !0 success
	vu8 request_time;
	vu8 request_flag;
};
extern struct iot_data_struct iot_data_t;
extern void iot_data_struct_init(void);

extern void iot_buffer_receiver(struct iot_struct *piot_t,u8 data);
extern int iot_handler(struct iot_struct *piot_t,function_ptr func_ptr);
extern void iot_send_cmd(u8 * cmd);
extern void iot_send_cmd_data_sprintf(const char *format, ...);	//���������������

//extern u8 iot_send_cmd_length(u8 len);
//extern u8 iot_send_nmgs(u8 len, u8 *str);
void iot_commad_analysis_respond(struct iot_struct *piot_t);
extern void iot_analysis(u8 *str1,u8 *str2,struct iot_data_struct *piot_data_t);
extern void iot_init_handler(struct iot_data_struct *piot_data_t);
extern void iot_time(struct iot_data_struct *piot_data_t);
#endif //_IOT_H

