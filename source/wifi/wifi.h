
#ifndef _WIFI_H
#define _WIFI_H
#include "stm32f10x.h"
#define		WIFI_BUF_LEN		1024

struct wifi_struct {
	vu16 header;
	vu16 tailer;
	vu16 count;
	vu8 buffer_temp[100];
	vu8 buffer[WIFI_BUF_LEN];
};
extern struct wifi_struct wifi_t;
struct wifi_data_struct{
	vu8 wifi_connect_flag;//0 error !0 success
};
extern struct wifi_data_struct wifi_data_t;
void wifi_init(void);
extern  void wifi_receive_callback(u8 data,struct wifi_struct * pwifi_t);
extern int wifi_receive_analysis(struct wifi_struct * pwifi_t);

#endif //_WIFI_H

