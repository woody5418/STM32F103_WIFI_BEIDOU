//********************************************************************
//	 
//      ���빦�ܣ���ʪ�ȳ�ʼ������	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

SHT2x_data SHT20;


u8 SHT2x_Init(void)
{
    u8 err;
    SHT2x_IIC_Init();
    err = SHT2x_SoftReset();
    return err;
}

u8 SHT2x_SoftReset(void)   //SHT20�����λ
{
    u8 err=0;
    SHT2x_IIC_Start();
    SHT2x_IIC_Send_Byte(0x80);
    err = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Send_Byte(0xFE);
    err = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Stop();
    return err;
}
float SHT2x_GetTempPoll(void)
{
    float TEMP;
    u8 ack, tmp1, tmp2;
    u16 ST;
    u16 i=0;
    SHT2x_IIC_Start();				//����IIC��ʼ�ź�
    SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_W);			//IIC����һ���ֽ�
    ack = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
    ack = SHT2x_IIC_Wait_Ack();

    do {
        delay_ms(100);
        SHT2x_IIC_Start();				//����IIC��ʼ�ź�
        SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_R);
        i++;
        ack = SHT2x_IIC_Wait_Ack();
        if(i==10)break;
    } while(ack!=0);
    tmp1 = SHT2x_IIC_Read_Byte(1);
    tmp2 = SHT2x_IIC_Read_Byte(1);
    SHT2x_IIC_Read_Byte(0);
    SHT2x_IIC_Stop();

    ST = (tmp1 << 8) | (tmp2 << 0);

    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    return (TEMP);
}

float SHT2x_GetHumiPoll(void)
{
    float HUMI;
    u8 ack, tmp1, tmp2;
    u16 SRH;
    u16 i=0;

    SHT2x_IIC_Start();				//����IIC��ʼ�ź�
    SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_W);			//IIC����һ���ֽ�
    ack = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = SHT2x_IIC_Wait_Ack();
    do {
        delay_ms(100);
        SHT2x_IIC_Start();				//����IIC��ʼ�ź�
        SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_R);
        i++;
        ack = SHT2x_IIC_Wait_Ack();
        if(i==10)break;
    } while(ack!=0);

    tmp1 = SHT2x_IIC_Read_Byte(1);

    tmp2 = SHT2x_IIC_Read_Byte(1);
    SHT2x_IIC_Read_Byte(0);
    SHT2x_IIC_Stop();

    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}
/*************************** IIC  ���� **************************/
//��ʼ��IIC
void SHT2x_IIC_Init(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//
//    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE );//
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����

//    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4|GPIO_Pin_5);
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	/////
    SHT2x_IIC_SCL=1;
    SHT2x_IIC_SDA=1;
}


//����IIC��ʼ�ź�
void SHT2x_IIC_Start(void)
{
    SHT2x_SDA_OUT();     //sda�����
    SHT2x_IIC_SDA=1;
    SHT2x_IIC_SCL=1;
    delay_us(4);
    SHT2x_IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    SHT2x_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
    delay_us(4);
}
//����IICֹͣ�ź�
void SHT2x_IIC_Stop(void)
{
    SHT2x_SDA_OUT();//sda�����
    SHT2x_IIC_SCL=0;
    SHT2x_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    SHT2x_IIC_SCL=1;
    delay_us(1);
    SHT2x_IIC_SDA=1;//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 SHT2x_IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SHT2x_SDA_IN();      //SDA����Ϊ����
    SHT2x_IIC_SDA=1;
    delay_us(1);
    SHT2x_IIC_SCL=1;
    delay_us(1);
    while(SHT2x_READ_SDA) {
        ucErrTime++;
        //	delay_us(1);
        if(ucErrTime>250) {
            SHT2x_IIC_Stop();
            return 1;
        }
    }
    SHT2x_IIC_SCL=0;//ʱ�����0
    return 0;
}
//����ACKӦ��
void SHT2x_IIC_Ack(void)
{
    SHT2x_IIC_SCL=0;
    SHT2x_SDA_OUT();
    SHT2x_IIC_SDA=0;
    delay_us(20);
    SHT2x_IIC_SCL=1;
    delay_us(2);
    SHT2x_IIC_SCL=0;
}
//������ACKӦ��
void SHT2x_IIC_NAck(void)
{
    SHT2x_IIC_SCL=0;
    SHT2x_SDA_OUT();
    SHT2x_IIC_SDA=1;
    delay_us(5);
    SHT2x_IIC_SCL=1;
    delay_us(5);
    SHT2x_IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void SHT2x_IIC_Send_Byte(u8 txd)
{
    u8 t;
    SHT2x_SDA_OUT();
    SHT2x_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++) {
        SHT2x_IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        delay_us(5);   //��TEA5767��������ʱ���Ǳ����
        SHT2x_IIC_SCL=1;
        delay_us(5);
        SHT2x_IIC_SCL=0;
        delay_us(5);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 SHT2x_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SHT2x_SDA_IN();//SDA����Ϊ����
    for(i=0; i<8; i++ ) {
        SHT2x_IIC_SCL=0;
        delay_us(5);
        SHT2x_IIC_SCL=1;
        receive<<=1;
        if(SHT2x_READ_SDA)receive++;
        delay_us(5);
    }
    if (!ack)
        SHT2x_IIC_NAck();//����nACK
    else
        SHT2x_IIC_Ack(); //����ACK
    return receive;
}













