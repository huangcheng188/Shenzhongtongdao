#ifndef __USART3_H
#define __USART3_H

#include "stdio.h"	
#include "stm32f10x.h"

void usart3_init(u32 bound);
extern void usart3_putc(u8 chr);
extern void usart3_send_string(u8 *str);

#endif
