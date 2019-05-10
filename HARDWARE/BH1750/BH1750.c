//********************************************************************
//	 
//      代码功能：光敏BH1750初始化代码	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

float result_lx = 0;
u8 BUF[2] = {0};
u16 result = 0;


///初始化
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
 返回0表示成功，返回1表示失败
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


/*********************  模拟IIC  的基础函数  *************************/
//初始化IIC  光敏传感器
//PB14   SDA
//PB15   SCL
void BH1750_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//产生IIC起始信号
void BH1750_IIC_Start(void)
{
    BH1750_SDA_OUT();     //sda线输出
    BH1750_IIC_SDA = 1;
    BH1750_IIC_SCL = 1;
    delay_us(14);
    BH1750_IIC_SDA = 0; //开始：当SCL为高时，数据从高到低变化
    delay_us(14);
    BH1750_IIC_SCL = 0; //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void BH1750_IIC_Stop(void)
{
    BH1750_SDA_OUT();//sda线输出
    BH1750_IIC_SCL = 0;
    BH1750_IIC_SDA = 0;//停止:当SCL为高时，数据从低到高变化
    delay_us(14);
    BH1750_IIC_SCL = 1;
    delay_us(14);
    BH1750_IIC_SDA = 1;//发送I2C总线结束信号

}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 BH1750_IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    BH1750_IIC_SCL = 0;
    BH1750_IIC_SDA = 1;
    BH1750_SDA_IN();      //SDA设置为输入
    delay_us(14);
    BH1750_IIC_SCL = 1;
    delay_us(14);
    while(BH1750_READ_SDA)//等到SDA变低
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            BH1750_IIC_Stop();
            return 1;
        }
        delay_us(1);
    }
    BH1750_IIC_SCL = 0; //时钟输出0
    delay_us(10);
    return 0;
}
//产生ACK应答
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
//产生非ACK应答
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void BH1750_IIC_Send_Byte(u8 txd)
{
    u8 t;
    BH1750_SDA_OUT();
    BH1750_IIC_SCL = 0; //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        BH1750_IIC_SCL = 0;
        if((txd & 0x80) >> 7)
            BH1750_IIC_SDA = 1;
        else
            BH1750_IIC_SDA = 0;
        txd <<= 1;
        delay_us(15);   //延时
        BH1750_IIC_SCL = 1;
        delay_us(15);
    }
    BH1750_IIC_SCL = 0;
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 BH1750_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;

    BH1750_SDA_IN();//SDA设置为输入
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
        BH1750_IIC_NAck();//发送nACK
    else
        BH1750_IIC_Ack(); //发送ACK
    return receive;
}







