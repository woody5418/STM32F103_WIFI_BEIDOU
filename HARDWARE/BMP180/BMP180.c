//********************************************************************
//	 
//      ���빦�ܣ���ѹBMP180��ʼ������	   
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
//����BMP085�ڲ�����,��������
//*********************************************************
u16 Multiple_read(u8 ST_Address)
{
    u8 msb, lsb;
    u8 errTimes = 0;
    u16 _data;
    Bmp180_IIC_Start();                          //��ʼ�ź�
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);    //�����豸��ַ+д�ź�
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }
    Bmp180_IIC_Send_Byte(ST_Address);             //���ʹ洢��Ԫ��ַ
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }
    Bmp180_IIC_Start();                          //��ʼ�ź�
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress + 1);       //�����豸��ַ+���ź�
    errTimes = 0;
    while(Bmp180_IIC_Wait_Ack())
    {
        errTimes++;
        if(errTimes == 255) return  _data = 0;
    }

    msb = Bmp180_IIC_Read_Byte(1);                 //BUF[0]�洢
    lsb = Bmp180_IIC_Read_Byte(0);                //���һ��������Ҫ��NOACK

    Bmp180_IIC_Stop();                           //ֹͣ�ź�
    delay_ms(5);
    _data = msb << 8;
    _data |= lsb;
    return _data;
}
//********************************************************************
u16 bmp180ReadTemp(void)
{
    u8 errTimes = 0;
    Bmp180_IIC_Start();                  //��ʼ�ź�
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);   //�����豸��ַ+д�ź�   BMP180_SlaveAddress  0xee	��ѹ������������ַ
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
    Bmp180_IIC_Stop();                   //����ֹͣ�ź�
    delay_ms(10);	// max time is 4.5ms
    return Multiple_read(0xF6);
}
//*************************************************************
u16 bmp180ReadPressure(void)
{
    //u16 pressure = 0;
    u8 errTimes = 0;
    Bmp180_IIC_Start();                   //��ʼ�ź�
    Bmp180_IIC_Send_Byte(BMP180_SlaveAddress);   //�����豸��ַ+д�ź�
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
    Bmp180_IIC_Stop();                    //����ֹͣ�ź�
    delay_ms(20);    	                  // max time is 4.5ms

    //pressure = Multiple_read(0xF6);
    //pressure &= 0x0FFFF;

    return Multiple_read(0xF6);//pressure;
}



//��ʼ��BMP085��������Ҫ��ο�pdf�����޸�
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

    up = bmp180ReadPressure();  // ��ȡѹǿ    return pressure;
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

/*****************************BMP180 ģ��IIC****************************/
//��ʼ��IIC
void Bmp180_IIC_Init(void)
{

//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��
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
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//ʹ��GPIOB

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PB13,PB14
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //��©���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1); 	//PB13,PB14 �����
    Bmp180_IIC_SCL = 1;
    Bmp180_IIC_SDA = 1;

}
//����IIC��ʼ�ź�
void Bmp180_IIC_Start(void)
{
    Bmp180_SDA_OUT();     //sda�����
    Bmp180_IIC_SDA = 1;
    Bmp180_IIC_SCL = 1;
    delay_us(4);
    Bmp180_IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
    delay_us(4);
    Bmp180_IIC_SCL = 0; //ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void Bmp180_IIC_Stop(void)
{
    Bmp180_SDA_OUT();//sda�����
    Bmp180_IIC_SCL = 0;
    Bmp180_IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
    delay_us(4);
    Bmp180_IIC_SCL = 1;
    Bmp180_IIC_SDA = 1; //����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t Bmp180_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    Bmp180_SDA_IN();      //SDA����Ϊ����
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
    Bmp180_IIC_SCL = 0; //ʱ�����0
    return 0;
}
//����ACKӦ��
void Bmp180_IIC_Ack(void)
{
    Bmp180_IIC_SCL = 0;
    Bmp180_SDA_OUT();//����Ϊ���
    Bmp180_IIC_SDA = 0;
    delay_us(2);
    Bmp180_IIC_SCL = 1;
    delay_us(2);
    Bmp180_IIC_SCL = 0;
}
//������ACKӦ��
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void Bmp180_IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    Bmp180_SDA_OUT(); 	    //����Ϊ�����
    Bmp180_IIC_SCL = 0; //����ʱ�ӿ�ʼ���ݴ���
    for(t = 0; t < 8; t++)
    {
        Bmp180_IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        Bmp180_IIC_SCL = 1;
        delay_us(2);
        Bmp180_IIC_SCL = 0;
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
uint8_t Bmp180_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    Bmp180_SDA_IN();//SDA����Ϊ����
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
        Bmp180_IIC_NAck();//����nACK
    else
        Bmp180_IIC_Ack(); //����ACK
    return receive;
}
















