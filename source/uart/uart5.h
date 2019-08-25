#ifndef _UART5_H
#define _UART5_H
#include "stm32f10x.h"
extern void uart5_init(u32 baudrate);
extern void uart5_send_string(u8 *str);

#endif //_UART5_H

