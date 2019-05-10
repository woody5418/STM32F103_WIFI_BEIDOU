//********************************************************************
//	 
//      代码功能：串口1初始化代码	用于北斗数据接收
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"


u8 USART1_RX_BUF[USART1_RX_LEN];     //接收缓冲,最大64个字节.
u8 Receive_gps_data_buf[USART1_RX_LEN];
u16 USART1_RX_STA=0;       //接收状态标记
u8 RX1_STATUS=0;

void uart1_init(u32 bound)
{
    //GPIO端口设置
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
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    u8 Res;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { //接收中断(接收到的数据必须是0x0d 0x0a结尾)
        Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
        if((RX1_STATUS&0x80) == 0) {
            if(RX1_STATUS&0x40) {
                if(Res == 0X43) {
                    RX1_STATUS|=0x80;  //收到了C
                }
            } else {
                if(Res == 0X4D)
                    RX1_STATUS|=0x40;  //收到了M
                else
                    RX1_STATUS=0;
            }
        } else {
            if((USART1_RX_STA&0x8000)==0) { //接收未完成
                if(USART1_RX_STA&0x4000) { //接收到了0x0d
                    if(Res!=0x0a) {
                        USART1_RX_STA=RX1_STATUS=0;//接收错误,重新开始
                    } else
                        USART1_RX_STA|=0x8000;	//接收完成了
                } else { //还没收到0X0D
                    if(Res==0x0d)
                        USART1_RX_STA|=0x4000;
                    else {
                        USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
                        USART1_RX_STA++;
                        if(USART1_RX_STA>USART1_RX_LEN)
                            USART1_RX_STA=RX1_STATUS=0;//接收数据错误,重新开始接收
                    }
                }
            }
        }
    }
}





