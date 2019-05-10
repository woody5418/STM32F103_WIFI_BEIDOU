//********************************************************************
//	 
//      代码功能：串口2初始化代码	用于ESP8266
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

#if 0
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE {
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
    USART2->DR = (u8) ch;
    return ch;
}
#endif

u16 USART2_RX_STA=0;  //接收状态标记

//串口接收缓存区
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];//发送缓冲,最大USART3_MAX_SEND_LEN字节



void uart2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //USART1_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART1_RX	PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口
}

//串口2中断接收
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
    u8 res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) { //接收中断(接收到的数据必须是0x0d 0x0a结尾)
        res =USART_ReceiveData(USART2);
        if((USART2_RX_STA&(1<<15))==0) {		//接收完的一批数据,还没有被处理,则不再接收其他数据
            if(USART2_RX_STA<USART2_MAX_RECV_LEN) {		//还可以接收数据
                TIM_SetCounter(TIM2,0);			//计数器清空
                if(USART2_RX_STA==0)
                    TIM_Cmd(TIM2, ENABLE);  	//使能定时器2
                FLAG_APPS_CJSON_SEND=0;
                USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值
            } else {
                USART2_RX_STA|=1<<15;		//强制标记接收完成
            }
        }
    }
}


//串口3,printf 函数
//确保一次发送数据不超过 USART3_MAX_SEND_LEN 字节
void u2_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART2_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART2_TX_BUF);																								//此次发送数据的长度
    for(j=0; j<i; j++) {																																		//循环发送数据
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);													//等待上次传输完成
        USART_SendData(USART2,(uint8_t)USART2_TX_BUF[j]);																	//发送数据到串口3
    }
}



