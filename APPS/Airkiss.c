//********************************************************************
//
//      ���빦�ܣ�Airkiss��������
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

uint8_t Dis_Number = 0;       //��ʼ���Լ��������̿��Ʊ���
uint16_t NetNumber = 0;       //����Airkiss�������̱���
uint8_t Control_Flag_Mode=0;  // ����ģ��ģʽ�л�


//********************************
//   ����Airkissģʽ
//********************************
void Configuration_Network(void)
{
    switch(NetNumber) {
    case 0: { // �˳�TCPģʽ
        LED_Hint_Red=1;
        if(Quit_Order_TT() ==true)
            NetNumber=1;
        else
            NetNumber=0;
        break;
    }
    case 1: { //�Ͽ�AP
        Send_CLOSE_TCP_UDP();
        NetNumber=2;
        break;
    }
    case 2: { //����Airkissģʽ
        Control_Flag_Mode=0;
        Send_Stop_Airkiss();
        Dis_Number = 3;  //����Airkissģʽ
        break;
    }
    }
}

//********************************
//   ��������WIFI ѡ��Airkiss
//********************************
void Distribution_Network(void)
{
    switch(Dis_Number) {
    case 0: { //��ʼ�� ���˳�͸��Ȼ������������
        Send_Order_RST();
        for(uint8_t m=100; m>0; m--) { //�ȴ�10s����wifi
            LED_Hint_Red=1;
            delay_ms(50);
            LED_Hint_Red=0;
            delay_ms(50);
        }
        LED_Hint_Red=0;
        Dis_Number = 1; //�����ɹ�
        break;
    }
    case 1: { //����Ƿ���AP����
        if(Send_Search_Connect_AP() == true)
            Dis_Number = 5; //������
        else
            Dis_Number=2; //û������
        break;
    }
    case 2: { //ѡ��stationģʽ
        Send_WiFi_Mode();
        Dis_Number=3; //����Airkissģʽ
        break;
    }
    case 3: { //����airkiss
        LED_Hint_Red=1;
		LED_Normal_Operation_Green=1;
        if (Send_Start_Airkiss() == true)
            Dis_Number=5; //����TCP
        else{
				Dis_Number = 11;//ֹͣairkiss
				Send_Stop_Airkiss();
			}
        break;
    }
    case 4: { //ֹͣAirkiss
        if (Send_Stop_Airkiss() == true)
            Dis_Number=3; //����Airkiss
        else
            Dis_Number = 4;//ֹͣairkiss
        break;
    }
    case 5: { //����TCP
      LED_Hint_Red=0;
		LED_Normal_Operation_Green=0;
        if (Send_Set_Connect_TCP() == true)
            Dis_Number=7; ////��ѯMAC��ַ
        else
            Dis_Number = 6;//�ر�TCP
        break;
    }
    case 6: { //�ر�TCP
        if (Send_CLOSE_TCP_UDP() == true)
            Dis_Number=5; //����TCP
        else
            Dis_Number = 3;//����airkiss
        break;
    }
    case 7: { //��ѯMAC��ַ
        if (Send_Search_Local_MAC() == true)
            Dis_Number=8;
        else
            Dis_Number = 7;
        break;
    }
    case 8: { //������
        Send_Passthrough();
        Dis_Number=9;
        break;
    }
    case 9: { //����͸��
        Send_Data_Passthrough();
        Dis_Number=10; //���ӳɹ�
        break;
    }
    case 10: { //��������ͷ�ɹ�
        Send_Request_header();
        USART2_RX_STA=0;
        TIM_Cmd(TIM4, ENABLE);
        Control_Flag_Mode=1;
        break;
    }
    case 11: {//Airkiss�ȴ�ʧ��
        TIM_Cmd(TIM4, ENABLE);
        USART2_RX_STA=0;
        Control_Flag_Mode=1;
    }
    }
}



