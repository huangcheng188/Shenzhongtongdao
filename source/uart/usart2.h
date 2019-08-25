#ifndef __USART2_H
#define __USART2_H

#include "stdio.h"	
#include "stm32f10x.h"

void usart2_init(u32 bound);
extern void usart2_send_string(u8 *str);

#endif


