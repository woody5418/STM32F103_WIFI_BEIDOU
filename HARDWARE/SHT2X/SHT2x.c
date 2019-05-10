//********************************************************************
//	 
//      代码功能：温湿度初始化代码	   
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

u8 SHT2x_SoftReset(void)   //SHT20软件复位
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
    SHT2x_IIC_Start();				//发送IIC开始信号
    SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_W);			//IIC发送一个字节
    ack = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
    ack = SHT2x_IIC_Wait_Ack();

    do {
        delay_ms(100);
        SHT2x_IIC_Start();				//发送IIC开始信号
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

    SHT2x_IIC_Start();				//发送IIC开始信号
    SHT2x_IIC_Send_Byte(SHT2x_I2C_ADR_W);			//IIC发送一个字节
    ack = SHT2x_IIC_Wait_Ack();
    SHT2x_IIC_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = SHT2x_IIC_Wait_Ack();
    do {
        delay_ms(100);
        SHT2x_IIC_Start();				//发送IIC开始信号
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
/*************************** IIC  部分 **************************/
//初始化IIC
void SHT2x_IIC_Init(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//
//    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE );//
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试

//    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4|GPIO_Pin_5);
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	/////
    SHT2x_IIC_SCL=1;
    SHT2x_IIC_SDA=1;
}


//产生IIC起始信号
void SHT2x_IIC_Start(void)
{
    SHT2x_SDA_OUT();     //sda线输出
    SHT2x_IIC_SDA=1;
    SHT2x_IIC_SCL=1;
    delay_us(4);
    SHT2x_IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    SHT2x_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
    delay_us(4);
}
//产生IIC停止信号
void SHT2x_IIC_Stop(void)
{
    SHT2x_SDA_OUT();//sda线输出
    SHT2x_IIC_SCL=0;
    SHT2x_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    SHT2x_IIC_SCL=1;
    delay_us(1);
    SHT2x_IIC_SDA=1;//发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 SHT2x_IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SHT2x_SDA_IN();      //SDA设置为输入
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
    SHT2x_IIC_SCL=0;//时钟输出0
    return 0;
}
//产生ACK应答
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
//不产生ACK应答
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void SHT2x_IIC_Send_Byte(u8 txd)
{
    u8 t;
    SHT2x_SDA_OUT();
    SHT2x_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0; t<8; t++) {
        SHT2x_IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        delay_us(5);   //对TEA5767这三个延时都是必须的
        SHT2x_IIC_SCL=1;
        delay_us(5);
        SHT2x_IIC_SCL=0;
        delay_us(5);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 SHT2x_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SHT2x_SDA_IN();//SDA设置为输入
    for(i=0; i<8; i++ ) {
        SHT2x_IIC_SCL=0;
        delay_us(5);
        SHT2x_IIC_SCL=1;
        receive<<=1;
        if(SHT2x_READ_SDA)receive++;
        delay_us(5);
    }
    if (!ack)
        SHT2x_IIC_NAck();//发送nACK
    else
        SHT2x_IIC_Ack(); //发送ACK
    return receive;
}













