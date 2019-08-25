#ifndef _UART4_H
#define _UART4_H
#include "stm32f10x.h"

extern void uart4_init(u32 baudrate);
extern void uart4_send_string(u8 *str);

#endif //_UART4_H

