/****************************************************************************
*       Copyright (C), 北京四维拓智教育科技有限公司
*
*       WIFI模块
*       V0.1
*       2017-06-27
*       Woody
*< STM32F130C8T6 >
			FLASH SIZE:64Kbytes
			RAM SIZE:20Kbytes
			Package:LQFP481
* 已用
			FLASH: Kbytes
			RAM:   Kbytes
*****************************************************************************/

#include "includes.h"

int main(void)
{
    SystemInit();//系统时钟等初始化
    delay_init(72);	//延时初始化
    NVIC_Configuration();//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    Platform_Init();
    while(1) {
        KEY_SCAN();                   //判断进入Airkiss模式
        GET_MPU9250_DATA();           // 读取MPU9250数据
        DataControlMode();            //数据控制
        PlatformProcessControl();     //流程控制函数
    }
}
//********************************
//   平台初始化函数
//********************************
void Platform_Init(void)
{
    TIM2_Int_Init(1000,7199);     //10Khz的计数频率，计数到1000为100ms 用于ESP8266
    TIM4_Configuration_set();      //定時器控制发送数据.
    PWM_Init(199,7199);           //PWM初始化
    uart1_init(9600);             //串口初始化为9600  北斗模块
    uart2_init(115200);           //esp8266通讯
    uart3_init(115200);           //SD卡
    Mpu9250_Init();                //9轴初始化
    Init_BH1750();                 //光敏
    Init_BMP180();                 //气压传感器初始化
    SHT2x_Init();                 //SHT2x初始化
    Beep_Init();                  //蜂鸣器初始化
    Adc_Init();                   //ADC初始化
    KeyInit();                    //按键初始化
    LED_Init();                   //LED的效果
    LED_RGB_Init();               //LED初始化
    servo_status_value=7;
    TIM_SetCompare2(TIM3,servo_status_value);
    sprintf(js_servo_status_value,"%d",servo_status_value);
    LED_Blink_times(5);   //闪烁五次进去系统
}

//********************************
//   流程控制函数
//********************************
void PlatformProcessControl(void)
{
    switch(Control_Flag_Mode) {
    case 0: { //首次进入
        FLAG_delay_s=0;
        TIM_Cmd(TIM4, DISABLE);
        Distribution_Network();  //上电配网
        break;
    }
    case 1: { //数据接收发送
        FLAG_delay_s=1;
        Apps_Analysis_Data();  //数据接受发送处理
        break;
    }
    case 2: {
        FLAG_delay_s=0;
        TIM_Cmd(TIM4, DISABLE);
        Configuration_Network();  //Airkiss流程
        break;
    }
    }
}
//********************************
//   数据控制函数
//********************************
void DataControlMode()
{
    if(FLAG_delay_s == 1) {
        GET_ublox_data();  // 获取北斗数据
        if(FLAG_APPS_CJSON_SEND == 1) {   //发送json到服务器
            FLAG_APPS_CJSON_SEND=0;
            SendDataServer();
        }
        if(FLAG_DATA_READ == 1) {         // 传感器数据读取
            FLAG_DATA_READ = 0;
            Data_Read();
        }
        if(Send_data_to_SD == 1)  {       // 发送带格式的数据到SD卡
            Send_data_to_SD=0;
            SendDataToSd();
        }
    }
}
/*****END OF FILE****/

