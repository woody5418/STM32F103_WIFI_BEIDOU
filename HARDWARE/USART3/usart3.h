#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////
extern u8 USART3_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符
extern u16 USART3_RX_STA;         //接收状态标记

void uart3_init(u32 bound);
uint16_t crc16(char* puchMsg,uint8_t usDataLen);
#endif




