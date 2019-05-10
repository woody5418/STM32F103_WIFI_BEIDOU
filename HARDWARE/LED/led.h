#ifndef __LED_H
#define __LED_H
#include "sys.h"


//LED 普通灯控制
#define LED_Breating_light_1        PBout(5)
//#define LED_Breating_light_2        PBout(8)
//#define LED_Breating_light_3        PBout(9)


//LED端口定义
#define LED_Hint_Red                PAout(5)
#define LED_Normal_Operation_Green  PBout(12)

void LED_Init(void);//初始化
void LED_RGB_Init(void);
void LED_Blink_times(uint8_t tims);


#endif

















