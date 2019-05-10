#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////////
extern u8 USART3_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART3_RX_STA;         //����״̬���

void uart3_init(u32 bound);
uint16_t crc16(char* puchMsg,uint8_t usDataLen);
#endif




