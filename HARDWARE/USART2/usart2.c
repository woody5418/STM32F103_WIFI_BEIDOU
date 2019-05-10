//********************************************************************
//	 
//      ���빦�ܣ�����2��ʼ������	����ESP8266
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

#if 0
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE {
    int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART2->SR&0X40)==0);//ѭ������,ֱ���������
    USART2->DR = (u8) ch;
    return ch;
}
#endif

u16 USART2_RX_STA=0;  //����״̬���

//���ڽ��ջ�����
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN];//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�



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
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

    //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���
}

//����2�жϽ���
void USART2_IRQHandler(void)                	//����2�жϷ������
{
    u8 res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) { //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
        res =USART_ReceiveData(USART2);
        if((USART2_RX_STA&(1<<15))==0) {		//�������һ������,��û�б�����,���ٽ�����������
            if(USART2_RX_STA<USART2_MAX_RECV_LEN) {		//�����Խ�������
                TIM_SetCounter(TIM2,0);			//���������
                if(USART2_RX_STA==0)
                    TIM_Cmd(TIM2, ENABLE);  	//ʹ�ܶ�ʱ��2
                FLAG_APPS_CJSON_SEND=0;
                USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ
            } else {
                USART2_RX_STA|=1<<15;		//ǿ�Ʊ�ǽ������
            }
        }
    }
}


//����3,printf ����
//ȷ��һ�η������ݲ����� USART3_MAX_SEND_LEN �ֽ�
void u2_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART2_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART2_TX_BUF);																								//�˴η������ݵĳ���
    for(j=0; j<i; j++) {																																		//ѭ����������
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);													//�ȴ��ϴδ������
        USART_SendData(USART2,(uint8_t)USART2_TX_BUF[j]);																	//�������ݵ�����3
    }
}



