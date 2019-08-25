#ifndef _LASER_H
#define _LASER_H
#include "stm32f10x.h"
#include "iot.h"
#define LASER_BUF_SIZE	1024

//激光测距数据
typedef struct {
	vu16 header;
	vu16 tailer;
	vu16 count;
	vu8  time_updata;//add hc
	vu8  buffer[LASER_BUF_SIZE];
}laser_struct;
extern laser_struct laser_t;

extern void laser_get_distance(laser_struct * plaser_t);		//读取distance
extern void laser_receive_callback(u8 data,laser_struct * plaser_t);		//for uart3
extern u32  laser_distance(u8 *str);		//
extern void laser_time(laser_struct *plaser_t);
extern int laser_receive_analysis(laser_struct * plaser_t,struct iot_data_struct *piot_data_t);

#endif //_LASER_H
