//********************************************************************
//	 
//      ���빦�ܣ�������ʼ������	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

static uint8_t key_num_flag = 0; //������������



//PA8����
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //GPIO_Pin_1 | GPIO_Pin_2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// ����PA0 ��������   ����ȥ3S
void KEY_SCAN(void)
{
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == Bit_RESET) { // ==1
        delay_ms(500); // ȥ����
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == Bit_RESET) {
            key_num_flag++;
            if(key_num_flag == 6) {
                key_num_flag=0;
                LED_Hint_Red=1;
                delay_ms(500);
                LED_Hint_Red=0;
                delay_ms(500);
				//ѡ��Airkissģʽ
                Control_Flag_Mode = 2;  //����Airkissģʽ
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





