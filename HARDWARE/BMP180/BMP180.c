//********************************************************************
//	 
//      代码功能：气压BMP180初始化代码	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

uint32_t result_UT = 0;
uint32_t result_UP = 0;

short ac1;
short ac2;
short ac3;

unsigned short ac4;
unsigned short ac5;
unsigned short ac6;

short b1;
short b2;
short mb;
short mc;
short md;

//*********************************************************
//读出BMP085内部数据,连续两个
//*********************************************************
u16 Multiple_read(u8 ST_Address)
{
    u8 msb, lsb;
    u8 errTimes = 0;
    u16 _data;
    Bmp180_IIC_Start();                          //起始信号
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);    //发送设备地址+写信号
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }
    Bmp180_IIC_Send_Byte(ST_Address);             //发送存储单元地址
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }
    Bmp180_IIC_Start();                          //起始信号
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress + 1);       //发送设备地址+读信号
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }

    msb = Bmp180_IIC_Read_Byte(1);                 //BUF[0]存储
    lsb = Bmp180_IIC_Read_Byte(0);                //最后一个数据需要回NOACK

    Bmp180_IIC_Stop();                           //停止信号
    delay_ms(5);
    _data = msb << 8;
    _data |= lsb;
    return _data;
}
//********************************************************************
u16 bmp180ReadTemp(void)
{
    u8 errTimes = 0;
    Bmp180_IIC_Start();                  //起始信号
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);   //发送设备地址+写信号   BMP180_SlaveAddress  0xee	气压传感器器件地址
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    errTimes = 0;
    Bmp180_IIC_Send_Byte(0xF4);	          // write register address
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    Bmp180_IIC_Send_Byte(0x2E);       	// write register data for temp
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    Bmp180_IIC_Stop();                   //发送停止信号
    delay_ms(10);	// max time is 4.5ms
    return Multiple_read(0xF6);
}
//*************************************************************
u16 bmp180ReadPressure(void)
{
    //u16 pressure = 0;
    u8 errTimes = 0;
    Bmp180_IIC_Start();                   //起始信号
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);   //发送设备地址+写信号
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    Bmp180_IIC_Send_Byte(0xF4);	          // write register address
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    Bmp180_IIC_Send_Byte(0x34);       	  // write register data for pressure
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return 0;
    }
    Bmp180_IIC_Stop();                    //发送停止信号
    delay_ms(20);    	                  // max time is 4.5ms

    //pressure = Multiple_read(0xF6);
    //pressure &= 0x0FFFF;

    return Multiple_read(0xF6);//pressure;
}



//初始化BMP085，根据需要请参考pdf进行修改
u16 Init_BMP180()
{
	u16 errorcode=0;
    Bmp180_IIC_Init();
    ac1 = Multiple_read(0xAA);
    ac2 = Multiple_read(0xAC);
    ac3 = Multiple_read(0xAE);
    ac4 = Multiple_read(0xB0);
    ac5 = Multiple_read(0xB2);
    ac6 = Multiple_read(0xB4);
    b1 =  Multiple_read(0xB6);
    b2 =  Multiple_read(0xB8);
    mb =  Multiple_read(0xBA);
    mc =  Multiple_read(0xBC);
    md =  Multiple_read(0xBE);
	errorcode = ac1|ac1|ac3|ac4|ac5|ac6|b1|b2|mb|mc|md;
	return errorcode;
}

uint32_t bmp180Convert(void)
{
    //uint32_t  result_UT;
    uint32_t  result_UP;
    unsigned int ut;
    unsigned long up;
    long x1, x2, b5, b6, x3, b3, p;
    unsigned long b4, b7;

    up = bmp180ReadPressure();  // 读取压强    return pressure;
    //*************
    x1 = (((long)ut - (long)ac6) * (long)ac5) >> 15;
    x2 = ((long) mc << 11) / (x1 + md);
    b5 = x1 + x2;
    result_UT = ((b5 + 8) >> 4);
    //*************
    b6 = b5 - 4000;
    // Calculate B3
    x1 = (b2 * (b6 * b6) >> 12) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;
    // Calculate B4
    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

    b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
    if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
    else
        p = (b7 / b4) << 1;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    result_UP = p + ((x1 + x2 + 3791) >> 4);
    return result_UP;
}

/*****************************BMP180 模拟IIC****************************/
//初始化IIC
void Bmp180_IIC_Init(void)
{

//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
//
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//使能GPIOB

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PB13,PB14
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1); 	//PB13,PB14 输出高
    Bmp180_IIC_SCL = 1;
    Bmp180_IIC_SDA = 1;

}
//产生IIC起始信号
void Bmp180_IIC_Start(void)
{
    Bmp180_SDA_OUT();     //sda线输出
    Bmp180_IIC_SDA = 1;
    Bmp180_IIC_SCL = 1;
    delay_us(4);
    Bmp180_IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
    delay_us(4);
    Bmp180_IIC_SCL = 0; //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void Bmp180_IIC_Stop(void)
{
    Bmp180_SDA_OUT();//sda线输出
    Bmp180_IIC_SCL = 0;
    Bmp180_IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
    delay_us(4);
    Bmp180_IIC_SCL = 1;
    Bmp180_IIC_SDA = 1; //发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t Bmp180_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    Bmp180_SDA_IN();      //SDA设置为输入
    Bmp180_IIC_SDA = 1;
    delay_us(1);
    Bmp180_IIC_SCL = 1;
    delay_us(1);
    while(Bmp180_READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            Bmp180_IIC_Stop();
            return 1;
        }
    }
    Bmp180_IIC_SCL = 0; //时钟输出0
    return 0;
}
//产生ACK应答
void Bmp180_IIC_Ack(void)
{
    Bmp180_IIC_SCL = 0;
    Bmp180_SDA_OUT();//设置为输出
    Bmp180_IIC_SDA = 0;
    delay_us(2);
    Bmp180_IIC_SCL = 1;
    delay_us(2);
    Bmp180_IIC_SCL = 0;
}
//不产生ACK应答
void Bmp180_IIC_NAck(void)
{
    Bmp180_IIC_SCL = 0;
    Bmp180_SDA_OUT();
    Bmp180_IIC_SDA = 1;
    delay_us(2);
    Bmp180_IIC_SCL = 1;
    delay_us(2);
    Bmp180_IIC_SCL = 0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void Bmp180_IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    Bmp180_SDA_OUT(); 	    //设置为输出、
    Bmp180_IIC_SCL = 0; //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        Bmp180_IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        Bmp180_IIC_SCL = 1;
        delay_us(2);
        Bmp180_IIC_SCL = 0;
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t Bmp180_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    Bmp180_SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++ )
    {
        Bmp180_IIC_SCL = 0;
        delay_us(2);
        Bmp180_IIC_SCL = 1;
        receive <<= 1;
        if(Bmp180_READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        Bmp180_IIC_NAck();//发送nACK
    else
        Bmp180_IIC_Ack(); //发送ACK
    return receive;
}
















