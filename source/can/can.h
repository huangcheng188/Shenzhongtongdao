#ifndef _CAN_H
#define _CAN_H
#include "stm32f10x.h"
#define 	CAN1_BUFFER_SIZE			20

typedef struct{
	vu8 header;
	vu8 tailer;
	vu8 count;
	CanRxMsg buffer[CAN1_BUFFER_SIZE];
}can_buffer_struct;
extern can_buffer_struct can1_buffer_t;

void can_init(void);
int can_analysis(can_buffer_struct * pcan1_buffer_t);
#endif //_CAN_H


