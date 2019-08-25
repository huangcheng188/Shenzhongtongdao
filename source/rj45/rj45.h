#ifndef _RJ45_H
#define _RJ45_H
#include "stm32f10x.h"
#define HEARD0 	0x00
#define HEARD1	0xFF
#define HEARD2	0xAA

#define RJ45_BUF_LEN	1024

struct modbus_struct{
	u8 client_write_flag;		//client ¶ÁÐ´±êÊ¶
	u8 client_read_flag;
};
struct rj45_struct {
	u16 header;
	u16 tailer;
	u16 count;
	u8 buffer_temp[100];
	u8 buffer[RJ45_BUF_LEN];//»º³åÇø

	struct modbus_struct modbus_t;
};
extern struct rj45_struct rj45_t;

int rj45_modbus_read(short reg, short num,int mask,u8 *buf);
int rj45_modbus_write(short reg, short num,int mask,int *buf);

void rj45_receive_callback(u8 data,struct rj45_struct *prj45_t);
extern int rj45_receive_analysis(struct rj45_struct *prj45_t);
extern void rj45_handler(struct rj45_struct *prj45_t);

#endif
