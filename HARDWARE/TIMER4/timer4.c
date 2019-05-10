//********************************************************************
//	 
//      ���빦�ܣ���ʱ��4��ʼ������	���������շ�����
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"


uint8_t FLAG_delay_s = 0; //�����ϴ����ݱ���
uint16_t Send_data_to_SD=0;  // �������ݵ�SD��

static uint8_t FLAG_LED_delay = 0; //����LEDʱ�����ʱ����
static uint8_t Num_delay_ms = 0; //��ȡʱ��������Ʊ�����
static uint8_t Start_Send_Sd_num = 0; // ��¼SD��˵��
static uint8_t delay_times = 0;  // ���ͼ�ʱ��������

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM4_Configuration_set(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = 500; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������500Ϊ50ms
    TIM_TimeBaseStructure.TIM_Prescaler =(7200-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
        TIM4, //TIM2
        TIM_IT_Update  |  //TIM �ж�Դ
        TIM_IT_Trigger,   //TIM �����ж�Դ
        ENABLE  //ʹ��
    );
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
}
/* z * ��ʱ��4��ʼ������
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
            if(delay_times == 40) {   //2S *20=ֵ
                delay_times = 0;
                FLAG_APPS_CJSON_SEND=1;
            }
        }
        if(FLAG_DATA_READ == 0) {
            Num_delay_ms++;
            if(Num_delay_ms == 20) { //1s ��ȡһ������
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













