//********************************************************************
//	 
//      ���빦�ܣ�����1��ʼ������	���ڱ������ݽ���
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"


u8 USART1_RX_BUF[USART1_RX_LEN];     //���ջ���,���64���ֽ�.
u8 Receive_gps_data_buf[USART1_RX_LEN];
u16 USART1_RX_STA=0;       //����״̬���
u8 RX1_STATUS=0;

void uart1_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
    u8 Res;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
        Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
        if((RX1_STATUS&0x80) == 0) {
            if(RX1_STATUS&0x40) {
                if(Res == 0X43) {
                    RX1_STATUS|=0x80;  //�յ���C
                }
            } else {
                if(Res == 0X4D)
                    RX1_STATUS|=0x40;  //�յ���M
                else
                    RX1_STATUS=0;
            }
        } else {
            if((USART1_RX_STA&0x8000)==0) { //����δ���
                if(USART1_RX_STA&0x4000) { //���յ���0x0d
                    if(Res!=0x0a) {
                        USART1_RX_STA=RX1_STATUS=0;//���մ���,���¿�ʼ
                    } else
                        USART1_RX_STA|=0x8000;	//���������
                } else { //��û�յ�0X0D
                    if(Res==0x0d)
                        USART1_RX_STA|=0x4000;
                    else {
                        USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
                        USART1_RX_STA++;
                        if(USART1_RX_STA>USART1_RX_LEN)
                            USART1_RX_STA=RX1_STATUS=0;//�������ݴ���,���¿�ʼ����
                    }
                }
            }
        }
    }
}





