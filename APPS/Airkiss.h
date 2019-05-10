#ifndef __AIRKISS_H
#define __AIRKISS_H
#include "stm32f10x.h"

extern uint8_t Dis_Number;   //初始化以及配网流程控制变量
extern uint16_t NetNumber;   //进入Airkiss控制流程变量
extern uint8_t Control_Flag_Mode;

void Platform_Init(void);
void DataControlMode(void);
void Distribution_Network(void);
void Configuration_Network(void);
void PlatformProcessControl(void);


#endif





