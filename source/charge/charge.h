#ifndef _CHARGE_H
#define _CHARGE_H
#include "stm32f10x.h"

enum{
	CHARGE_OFF=0,		//上电初始值
	CHARGE_OFF_WAIT,
	CHARGE_OFF_OK,
	CHARGE_ON,
	CHARGE_ON_WAIT,
	CHARGE_ON_OK,
};


typedef struct  {
	vu8 charge_mode;
}charge_struct;
extern charge_struct charge_t;

void charge_init(void);
u16 charge_battay_adc_value_get(void);
u8 charge_handler( charge_struct *pcharge_t);

#endif //_CHARGE_H

