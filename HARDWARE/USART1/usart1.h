#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"
#include <stdio.h>


#define USART1_RX_LEN  	80

extern u8 USART1_RX_BUF[USART1_RX_LEN];     //接收缓冲,最大63个字节.末字节为换行符
extern u8 Receive_gps_data_buf[USART1_RX_LEN];
extern u16 USART1_RX_STA;         //接收状态标记
extern u8 RX1_STATUS;
void uart1_init(u32 bound);
#endif


