//********************************************************************
//	 
//      ���빦�ܣ�����BH1750��ʼ������	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

float result_lx = 0;
u8 BUF[2] = {0};
u16 result = 0;


///��ʼ��
void Init_BH1750(void)
{
    BH1750_IIC_Init();
}

void Cmd_Write_BH1750(u8 cmd)
{
    u8 errTime = 0;
    BH1750_IIC_Start();
    BH1750_IIC_Send_Byte(BH1750_Addr + 0);
    while(BH1750_IIC_Wait_Ack())
    {
        errTime++;
        if(errTime == 255) break;
    }
    errTime = 0;
    BH1750_IIC_Send_Byte(cmd);
    while(BH1750_IIC_Wait_Ack())
    {
        errTime++;
        if(errTime == 255) break;
    }
    BH1750_IIC_Stop();
    delay_ms(5);
}
void Start_BH1750(void)
{
    Cmd_Write_BH1750(BH1750_ON);	 //power on
    //Cmd_Write_BH1750(BH1750_RSET);	//clear
    Cmd_Write_BH1750(BH1750_ONE);  //H,120ms
}
/*
 ����0��ʾ�ɹ�������1��ʾʧ��
*/
u8 Read_BH1750(void)
{
    u8 errTime = 0;
    BH1750_IIC_Start();
    BH1750_IIC_Send_Byte(BH1750_Addr + 1);
    while(BH1750_IIC_Wait_Ack())
    {
        errTime++;
        if(errTime == 255) return 1;
    }
    BUF[0] = BH1750_IIC_Read_Byte(1); //ACK
    BUF[1] = BH1750_IIC_Read_Byte(0); //NACK

    BH1750_IIC_Stop();
    delay_ms(5);
    return 0;
}
void Convert_BH1750(void)
{

    result = BUF[0];
    result = (result << 8) + BUF[1];
    result_lx = (float)result / 1.2;
}


float send_bh1750_data(void)
{

    Start_BH1750();
    delay_ms(180);
    if(Read_BH1750())
    {
        return result_lx = 0;
    }
    Convert_BH1750();
    return result_lx;
}


/*********************  ģ��IIC  �Ļ�������  *************************/
//��ʼ��IIC  ����������
//PB14   SDA
//PB15   SCL
void BH1750_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//����IIC��ʼ�ź�
void BH1750_IIC_Start(void)
{
    BH1750_SDA_OUT();     //sda�����
    BH1750_IIC_SDA = 1;
    BH1750_IIC_SCL = 1;
    delay_us(14);
    BH1750_IIC_SDA = 0; //��ʼ����SCLΪ��ʱ�����ݴӸߵ��ͱ仯
    delay_us(14);
    BH1750_IIC_SCL = 0; //ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void BH1750_IIC_Stop(void)
{
    BH1750_SDA_OUT();//sda�����
    BH1750_IIC_SCL = 0;
    BH1750_IIC_SDA = 0;//ֹͣ:��SCLΪ��ʱ�����ݴӵ͵��߱仯
    delay_us(14);
    BH1750_IIC_SCL = 1;
    delay_us(14);
    BH1750_IIC_SDA = 1;//����I2C���߽����ź�

}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 BH1750_IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    BH1750_IIC_SCL = 0;
    BH1750_IIC_SDA = 1;
    BH1750_SDA_IN();      //SDA����Ϊ����
    delay_us(14);
    BH1750_IIC_SCL = 1;
    delay_us(14);
    while(BH1750_READ_SDA)//�ȵ�SDA���
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            BH1750_IIC_Stop();
            return 1;
        }
        delay_us(1);
    }
    BH1750_IIC_SCL = 0; //ʱ�����0
    delay_us(10);
    return 0;
}
//����ACKӦ��
void BH1750_IIC_Ack(void)
{
    BH1750_IIC_SCL = 0;
    BH1750_SDA_OUT();
    BH1750_IIC_SDA = 0;
    delay_us(15);
    BH1750_IIC_SCL = 1;
    delay_us(15);
    BH1750_IIC_SCL = 0;
    BH1750_IIC_SDA = 1;
}
//������ACKӦ��
void BH1750_IIC_NAck(void)
{

    BH1750_SDA_OUT();
    BH1750_IIC_SDA = 1;
    BH1750_IIC_SCL = 0;
    delay_us(15);
    BH1750_IIC_SCL = 1;
    delay_us(15);
    BH1750_IIC_SDA = 1;
    BH1750_IIC_SCL = 0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void BH1750_IIC_Send_Byte(u8 txd)
{
    u8 t;
    BH1750_SDA_OUT();
    BH1750_IIC_SCL = 0; //����ʱ�ӿ�ʼ���ݴ���
    for(t = 0; t < 8; t++)
    {
        BH1750_IIC_SCL = 0;
        if((txd & 0x80) >> 7)
            BH1750_IIC_SDA = 1;
        else
            BH1750_IIC_SDA = 0;
        txd <<= 1;
        delay_us(15);   //��ʱ
        BH1750_IIC_SCL = 1;
        delay_us(15);
    }
    BH1750_IIC_SCL = 0;
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 BH1750_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;

    BH1750_SDA_IN();//SDA����Ϊ����
    for(i = 0; i < 8; i++ )
    {
        BH1750_IIC_SCL = 0;
        delay_us(50);
        BH1750_IIC_SCL = 1;
        delay_us(50);
        receive <<= 1;
        if(BH1750_READ_SDA)receive++;
    }
    BH1750_IIC_SCL = 0;
    if (!ack)
        BH1750_IIC_NAck();//����nACK
    else
        BH1750_IIC_Ack(); //����ACK
    return receive;
}







