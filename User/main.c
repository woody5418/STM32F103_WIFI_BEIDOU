/****************************************************************************
*       Copyright (C), ������ά���ǽ����Ƽ����޹�˾
*
*       WIFIģ��
*       V0.1
*       2017-06-27
*       Woody
*< STM32F130C8T6 >
			FLASH SIZE:64Kbytes
			RAM SIZE:20Kbytes
			Package:LQFP481
* ����
			FLASH: Kbytes
			RAM:   Kbytes
*****************************************************************************/

#include "includes.h"

int main(void)
{
    SystemInit();//ϵͳʱ�ӵȳ�ʼ��
    delay_init(72);	//��ʱ��ʼ��
    NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    Platform_Init();
    while(1) {
        KEY_SCAN();                   //�жϽ���Airkissģʽ
        GET_MPU9250_DATA();           // ��ȡMPU9250����
        DataControlMode();            //���ݿ���
        PlatformProcessControl();     //���̿��ƺ���
    }
}
//********************************
//   ƽ̨��ʼ������
//********************************
void Platform_Init(void)
{
    TIM2_Int_Init(1000,7199);     //10Khz�ļ���Ƶ�ʣ�������1000Ϊ100ms ����ESP8266
    TIM4_Configuration_set();      //���r�����Ʒ�������.
    PWM_Init(199,7199);           //PWM��ʼ��
    uart1_init(9600);             //���ڳ�ʼ��Ϊ9600  ����ģ��
    uart2_init(115200);           //esp8266ͨѶ
    uart3_init(115200);           //SD��
    Mpu9250_Init();                //9���ʼ��
    Init_BH1750();                 //����
    Init_BMP180();                 //��ѹ��������ʼ��
    SHT2x_Init();                 //SHT2x��ʼ��
    Beep_Init();                  //��������ʼ��
    Adc_Init();                   //ADC��ʼ��
    KeyInit();                    //������ʼ��
    LED_Init();                   //LED��Ч��
    LED_RGB_Init();               //LED��ʼ��
    servo_status_value=7;
    TIM_SetCompare2(TIM3,servo_status_value);
    sprintf(js_servo_status_value,"%d",servo_status_value);
    LED_Blink_times(5);   //��˸��ν�ȥϵͳ
}

//********************************
//   ���̿��ƺ���
//********************************
void PlatformProcessControl(void)
{
    switch(Control_Flag_Mode) {
    case 0: { //�״ν���
        FLAG_delay_s=0;
        TIM_Cmd(TIM4, DISABLE);
        Distribution_Network();  //�ϵ�����
        break;
    }
    case 1: { //���ݽ��շ���
        FLAG_delay_s=1;
        Apps_Analysis_Data();  //���ݽ��ܷ��ʹ���
        break;
    }
    case 2: {
        FLAG_delay_s=0;
        TIM_Cmd(TIM4, DISABLE);
        Configuration_Network();  //Airkiss����
        break;
    }
    }
}
//********************************
//   ���ݿ��ƺ���
//********************************
void DataControlMode()
{
    if(FLAG_delay_s == 1) {
        GET_ublox_data();  // ��ȡ��������
        if(FLAG_APPS_CJSON_SEND == 1) {   //����json��������
            FLAG_APPS_CJSON_SEND=0;
            SendDataServer();
        }
        if(FLAG_DATA_READ == 1) {         // ���������ݶ�ȡ
            FLAG_DATA_READ = 0;
            Data_Read();
        }
        if(Send_data_to_SD == 1)  {       // ���ʹ���ʽ�����ݵ�SD��
            Send_data_to_SD=0;
            SendDataToSd();
        }
    }
}
/*****END OF FILE****/

