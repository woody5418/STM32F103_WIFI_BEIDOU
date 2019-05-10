#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"


#define USART2_MAX_RECV_LEN		1024					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		1024					//����ͻ����ֽ���

extern u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];
extern u16 USART2_RX_STA;


void uart2_init(u32 baud);
void u2_printf(char* fmt,...);

#endif


