#ifndef __AIRKISS_H
#define __AIRKISS_H
#include "stm32f10x.h"

extern uint8_t Dis_Number;   //��ʼ���Լ��������̿��Ʊ���
extern uint16_t NetNumber;   //����Airkiss�������̱���
extern uint8_t Control_Flag_Mode;

void Platform_Init(void);
void DataControlMode(void);
void Distribution_Network(void);
void Configuration_Network(void);
void PlatformProcessControl(void);


#endif





