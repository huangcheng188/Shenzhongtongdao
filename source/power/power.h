#ifndef _POWER_H
#define _POWER_H
#include "stm32f10x.h"

enum{
	OFF=0,		//¹Ø
	ON			//¿ª
};

enum{
	POWER_SLEEP=0,
	POWER_NORMAL
};
typedef struct {
	vu8 power_mode;
}power_struct;
extern power_struct power_t;

void power_wifi_init(void);
void power_wifi_enable(u8 state);
void power_iot_init(void);
void power_iot_enable(u8 state);
void power_laser_init(void);
void power_laser_enable(u8 state);

void power_init(void);
void power_on_all(void);
void power_off_all(void);
#endif 
