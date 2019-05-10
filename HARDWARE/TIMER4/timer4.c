//********************************************************************
//	 
//      代码功能：定时器4初始化代码	用于数据收发控制
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"


uint8_t FLAG_delay_s = 0; //控制上传数据变量
uint16_t Send_data_to_SD=0;  // 发送数据到SD卡

static uint8_t FLAG_LED_delay = 0; //控制LED时间的延时变量
static uint8_t Num_delay_ms = 0; //读取时间计数控制变量、
static uint8_t Start_Send_Sd_num = 0; // 纪录SD卡说明
static uint8_t delay_times = 0;  // 发送计时计数函数

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM4_Configuration_set(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = 500; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到500为50ms
    TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM4, //TIM2
        TIM_IT_Update  |  //TIM 中断源
        TIM_IT_Trigger,   //TIM 触发中断源
        ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
}
/* z * 定时器4初始化函数
 */
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
        Start_Send_Sd_num++;
        if(FLAG_delay_s == 1) { 
            delay_times++;
            FLAG_LED_delay++;
            if(FLAG_LED_delay == 20) { //500ms
                FLAG_LED_delay=0;
                LED_Normal_Operation_Green=!LED_Normal_Operation_Green;
            }
            if(delay_times == 40) {   //2S *20=值
                delay_times = 0;
                FLAG_APPS_CJSON_SEND=1;
            }
        }
        if(FLAG_DATA_READ == 0) {
            Num_delay_ms++;
            if(Num_delay_ms == 20) { //1s 读取一次数据
                Num_delay_ms=0;
                FLAG_DATA_READ=1;
            }
        }
        if(Start_Send_Sd_num==100) {
            Start_Send_Sd_num=0;
            Send_data_to_SD=1;
        }
    }
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
}













