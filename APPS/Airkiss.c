//********************************************************************
//
//      代码功能：Airkiss配网流程
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

uint8_t Dis_Number = 0;       //初始化以及配网流程控制变量
uint16_t NetNumber = 0;       //进入Airkiss控制流程变量
uint8_t Control_Flag_Mode=0;  // 控制模块模式切换


//********************************
//   进入Airkiss模式
//********************************
void Configuration_Network(void)
{
    switch(NetNumber) {
    case 0: { // 退出TCP模式
        LED_Hint_Red=1;
        if(Quit_Order_TT() ==true)
            NetNumber=1;
        else
            NetNumber=0;
        break;
    }
    case 1: { //断开AP
        Send_CLOSE_TCP_UDP();
        NetNumber=2;
        break;
    }
    case 2: { //进入Airkiss模式
        Control_Flag_Mode=0;
        Send_Stop_Airkiss();
        Dis_Number = 3;  //进入Airkiss模式
        break;
    }
    }
}

//********************************
//   连接配置WIFI 选择Airkiss
//********************************
void Distribution_Network(void)
{
    switch(Dis_Number) {
    case 0: { //初始化 先退出透传然后发送重启命令
        Send_Order_RST();
        for(uint8_t m=100; m>0; m--) { //等待10s连接wifi
            LED_Hint_Red=1;
            delay_ms(50);
            LED_Hint_Red=0;
            delay_ms(50);
        }
        LED_Hint_Red=0;
        Dis_Number = 1; //重启成功
        break;
    }
    case 1: { //检测是否有AP连接
        if(Send_Search_Connect_AP() == true)
            Dis_Number = 5; //有连接
        else
            Dis_Number=2; //没有连接
        break;
    }
    case 2: { //选择station模式
        Send_WiFi_Mode();
        Dis_Number=3; //进入Airkiss模式
        break;
    }
    case 3: { //开启airkiss
        LED_Hint_Red=1;
		LED_Normal_Operation_Green=1;
        if (Send_Start_Airkiss() == true)
            Dis_Number=5; //创建TCP
        else{
				Dis_Number = 11;//停止airkiss
				Send_Stop_Airkiss();
			}
        break;
    }
    case 4: { //停止Airkiss
        if (Send_Stop_Airkiss() == true)
            Dis_Number=3; //开启Airkiss
        else
            Dis_Number = 4;//停止airkiss
        break;
    }
    case 5: { //创建TCP
      LED_Hint_Red=0;
		LED_Normal_Operation_Green=0;
        if (Send_Set_Connect_TCP() == true)
            Dis_Number=7; ////查询MAC地址
        else
            Dis_Number = 6;//关闭TCP
        break;
    }
    case 6: { //关闭TCP
        if (Send_CLOSE_TCP_UDP() == true)
            Dis_Number=5; //创建TCP
        else
            Dis_Number = 3;//开启airkiss
        break;
    }
    case 7: { //查询MAC地址
        if (Send_Search_Local_MAC() == true)
            Dis_Number=8;
        else
            Dis_Number = 7;
        break;
    }
    case 8: { //单连接
        Send_Passthrough();
        Dis_Number=9;
        break;
    }
    case 9: { //进入透传
        Send_Data_Passthrough();
        Dis_Number=10; //连接成功
        break;
    }
    case 10: { //发送请求头成功
        Send_Request_header();
        USART2_RX_STA=0;
        TIM_Cmd(TIM4, ENABLE);
        Control_Flag_Mode=1;
        break;
    }
    case 11: {//Airkiss等待失败
        TIM_Cmd(TIM4, ENABLE);
        USART2_RX_STA=0;
        Control_Flag_Mode=1;
    }
    }
}



