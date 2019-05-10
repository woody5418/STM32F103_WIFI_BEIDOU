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

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM5_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
        TIM5, //TIM2
        TIM_IT_Update,
        ENABLE  //ʹ��
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx����

}
short  magXvalue=0;
short  magYvalue=0;

short  mag_x_max=0;
short  mag_x_min=0;

short  mag_y_max=0;
short  mag_y_min=0;

short mag_x_middle = 0;
short mag_y_middle = 0;

void TIM5_IRQHandler(void)   //TIM3�ж�
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) { //���ָ����TIM�жϷ������:TIM �ж�Դ
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
        read_Mpu9250_data(js_accel_x_status,js_accel_y_status,js_accel_z_status,
							js_gyro_x_status,js_gyro_y_status,js_gyro_z_status,
							js_mag_x_status,js_mag_y_status,js_mag_z_status);
		magXvalue = atoi((char *)js_mag_x_status);  // ��ȡX��ֵ
		magYvalue = atoi((char *)js_mag_y_status);	// ��ȡY��ֵ	
		if(magXvalue>=mag_x_max)  mag_x_max = magXvalue;
		else if(magXvalue<=mag_x_min) mag_x_min = magXvalue;
		if(magYvalue>=mag_y_max)  mag_y_max = magYvalue;
		else if(magYvalue<=mag_y_min) mag_y_min = magYvalue;
		
		mag_x_middle = magXvalue - (mag_x_max+mag_x_min)/2;
		mag_y_middle = magYvalue - (mag_y_max+mag_y_min)/2;
    }
}












