#include "timer5.h"
#include "led.h"
#include "mpu9250.h"
#include "string.h"
#include "stdlib.h"
#include "delay.h"

uint8_t js_accel_x_status[5]= "";
uint8_t js_accel_y_status[5]= "";
uint8_t js_accel_z_status[5]= "";

uint8_t js_gyro_x_status[5]= "";
uint8_t js_gyro_y_status[5]= "";
uint8_t js_gyro_z_status[5]= "";

uint8_t js_mag_x_status[5]= "";
uint8_t js_mag_y_status[5]= "";
uint8_t js_mag_z_status[5]= "";

uint8_t mag_x_status_mix[5]="";
uint8_t mag_x_status_min[5]="";

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM5_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(  //使能或者失能指定的TIM中断
        TIM5, //TIM2
        TIM_IT_Update,
        ENABLE  //使能
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM5, ENABLE);  //使能TIMx外设

}
short  magXvalue=0;
short  magYvalue=0;

short  mag_x_max=0;
short  mag_x_min=0;

short  mag_y_max=0;
short  mag_y_min=0;

short mag_x_middle = 0;
short mag_y_middle = 0;

void TIM5_IRQHandler(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) { //检查指定的TIM中断发生与否:TIM 中断源
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
        read_Mpu9250_data(js_accel_x_status,js_accel_y_status,js_accel_z_status,
							js_gyro_x_status,js_gyro_y_status,js_gyro_z_status,
							js_mag_x_status,js_mag_y_status,js_mag_z_status);
		magXvalue = atoi((char *)js_mag_x_status);  // 读取X的值
		magYvalue = atoi((char *)js_mag_y_status);	// 读取Y的值	
		if(magXvalue>=mag_x_max)  mag_x_max = magXvalue;
		else if(magXvalue<=mag_x_min) mag_x_min = magXvalue;
		if(magYvalue>=mag_y_max)  mag_y_max = magYvalue;
		else if(magYvalue<=mag_y_min) mag_y_min = magYvalue;
		
		mag_x_middle = magXvalue - (mag_x_max+mag_x_min)/2;
		mag_y_middle = magYvalue - (mag_y_max+mag_y_min)/2;
    }
}












