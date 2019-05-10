//********************************************************************
//	 
//      代码功能：按键初始化代码	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

static uint8_t key_num_flag = 0; //按键计数函数



//PA8按键
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //GPIO_Pin_1 | GPIO_Pin_2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 按键PA0 上来输入   按下去3S
void KEY_SCAN(void)
{
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == Bit_RESET) { // ==1
        delay_ms(500); // 去抖动
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == Bit_RESET) {
            key_num_flag++;
            if(key_num_flag == 6) {
                key_num_flag=0;
                LED_Hint_Red=1;
                delay_ms(500);
                LED_Hint_Red=0;
                delay_ms(500);
				//选择Airkiss模式
                Control_Flag_Mode = 2;  //进入Airkiss模式
                NetNumber=0;
                LED_Hint_Red=1;
                delay_ms(500);
                LED_Hint_Red=0;
                delay_ms(500);
            }
        }
    }
    key_num_flag=0;
}





