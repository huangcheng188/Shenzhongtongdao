#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"
typedef struct  {
	vu8 key_mun;
}key_struct;
extern key_struct key_t;
void key_init(void);
int key_handler(key_struct * pkey_t);
#endif //_KEY_H
