//********************************************************************
//	 
//      代码功能：九轴MPU9150初始化代码	   
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

unsigned char Mpu9250_BUF[10];       //接收数据缓存区
short Mpu9250_T_X,Mpu9250_T_Y,Mpu9250_T_Z;

bool Mpu9250_Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char Mpu9250_Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);
bool Mpu9250_I2C_Start(void);
void Mpu9250_I2C_Stop(void);
void Mpu9250_I2C_Ack(void);
void Mpu9250_I2C_NoAck(void);
bool Mpu9250_I2C_WaitAck(void);
void Mpu9250_I2C_SendByte(u8 SendByte);
unsigned char Mpu9250_I2C_RadeByte(void);
void Mpu9250_I2C_delay(void);
void READ_ACCEL_MPU9250(void);
void READ_GYRO_MPU9250(void);
void READ_MAG_MPU9250(void);
void mpu9250CreateData(uchar *s,short temp_data);

bool Mpu9250_Init(void) {
	u8 errcode=0;
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟	
	
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    delay_ms(10);				 //延时-顾玉龙

    if(Mpu9250_Single_Write(GYRO_ADDRESS,PWR_MGMT_1, 0x00) == true)	//解除休眠状态
		errcode+=1;
    if(Mpu9250_Single_Write(GYRO_ADDRESS,SMPLRT_DIV, 0x07)== true)	
		errcode+=1;
    if(Mpu9250_Single_Write(GYRO_ADDRESS,CONFIG, 0x06)== true)	
		errcode+=1;
    if(Mpu9250_Single_Write(GYRO_ADDRESS,GYRO_CONFIG, 0x18)== true)	
		errcode+=1;
    if(Mpu9250_Single_Write(GYRO_ADDRESS,ACCEL_CONFIG, 0x01)== true)
		errcode+=1;	
	if(errcode == 5)
		return true;
	else
		return false;
}

void read_Mpu9250_data(u8 *ax,u8 * ay,u8 * az,u8 * gx,u8 * gy,u8 * gz,u8 * mx,u8 * my,u8 * mz) {
	
    READ_ACCEL_MPU9250();   //加速度
    mpu9250CreateData(ax,Mpu9250_T_X);//转换X轴数据到数组
    mpu9250CreateData(ay,Mpu9250_T_Y);//转换Y轴数据到数组
    mpu9250CreateData(az,Mpu9250_T_Z);//转换Z轴数据到数组

    READ_GYRO_MPU9250();    //陀螺
    mpu9250CreateData(gx,Mpu9250_T_X);//转换X轴数据到数组
    mpu9250CreateData(gy,Mpu9250_T_Y);//转换Y轴数据到数组
    mpu9250CreateData(gz,Mpu9250_T_Z);//转换Z轴数据到数组
	
	READ_MAG_MPU9250();	    //磁场
    mpu9250CreateData(mx,Mpu9250_T_X);//转换X轴数据到数组
    mpu9250CreateData(my,Mpu9250_T_Y);//转换Y轴数据到数组
    mpu9250CreateData(mz,Mpu9250_T_Z);//转换Z轴数据到数组
}
/**
 *创建数据
 */
void mpu9250CreateData(u8 *s,short temp_data) {
    if(temp_data<0) {
        temp_data=-temp_data;
        *s='-';
    } else *s=' ';
    *++s =temp_data/100+0x30;
    temp_data=temp_data%100;     //取余运算
    *++s =temp_data/10+0x30;
    temp_data=temp_data%10;      //取余运算
    *++s =temp_data+0x30;
}

//******读取MPU9250数据****************************************
void READ_ACCEL_MPU9250(void) {
    Mpu9250_BUF[0]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_L);
    Mpu9250_BUF[1]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_H);
    Mpu9250_T_X=	(Mpu9250_BUF[1]<<8)|Mpu9250_BUF[0];
    Mpu9250_T_X/=164; 						   //读取计算X轴数据

    Mpu9250_BUF[2]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_L);
    Mpu9250_BUF[3]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_H);
    Mpu9250_T_Y=	(Mpu9250_BUF[3]<<8)|Mpu9250_BUF[2];
    Mpu9250_T_Y/=164; 						   //读取计算Y轴数据
    Mpu9250_BUF[4]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_L);
    Mpu9250_BUF[5]=Mpu9250_Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_H);
    Mpu9250_T_Z=	(Mpu9250_BUF[5]<<8)|Mpu9250_BUF[4];
    Mpu9250_T_Z/=164; 					       //读取计算Z轴数据
}

void READ_GYRO_MPU9250(void) {
    Mpu9250_BUF[0]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_XOUT_L);
    Mpu9250_BUF[1]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_XOUT_H);
    Mpu9250_T_X=	(Mpu9250_BUF[1]<<8)|Mpu9250_BUF[0];
    Mpu9250_T_X/=16.4; 						   //读取计算X轴数据

    Mpu9250_BUF[2]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_YOUT_L);
    Mpu9250_BUF[3]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_YOUT_H);
    Mpu9250_T_Y=	(Mpu9250_BUF[3]<<8)|Mpu9250_BUF[2];
    Mpu9250_T_Y/=16.4; 						   //读取计算Y轴数据
    Mpu9250_BUF[4]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_ZOUT_L);
    Mpu9250_BUF[5]=Mpu9250_Single_Read(GYRO_ADDRESS,GYRO_ZOUT_H);
    Mpu9250_T_Z=	(Mpu9250_BUF[5]<<8)|Mpu9250_BUF[4];
    Mpu9250_T_Z/=16.4; 					       //读取计算Z轴数据
}

void READ_MAG_MPU9250(void) {
    Mpu9250_Single_Write(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode
    delay_ms(10);	//-顾玉龙
    Mpu9250_Single_Write(MAG_ADDRESS,0x0A,0x01);
    delay_ms(10);	//-顾玉龙
    Mpu9250_BUF[0]=Mpu9250_Single_Read (MAG_ADDRESS,MAG_XOUT_L);
    Mpu9250_BUF[1]=Mpu9250_Single_Read (MAG_ADDRESS,MAG_XOUT_H);
    Mpu9250_T_X=(Mpu9250_BUF[1]<<8)|Mpu9250_BUF[0];

    Mpu9250_BUF[2]=Mpu9250_Single_Read(MAG_ADDRESS,MAG_YOUT_L);
    Mpu9250_BUF[3]=Mpu9250_Single_Read(MAG_ADDRESS,MAG_YOUT_H);
    Mpu9250_T_Y=	(Mpu9250_BUF[3]<<8)|Mpu9250_BUF[2];
    //读取计算Y轴数据

    Mpu9250_BUF[4]=Mpu9250_Single_Read(MAG_ADDRESS,MAG_ZOUT_L);
    Mpu9250_BUF[5]=Mpu9250_Single_Read(MAG_ADDRESS,MAG_ZOUT_H);
    Mpu9250_T_Z=	(Mpu9250_BUF[5]<<8)|Mpu9250_BUF[4];
    //读取计算Z轴数据
}

//ZRX
//单字节写入*******************************************
bool Mpu9250_Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data) {	     //void
    if(!Mpu9250_I2C_Start())return false;
    Mpu9250_I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//Mpu9250_I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址
    if(!Mpu9250_I2C_WaitAck()) {
        Mpu9250_I2C_Stop();
        return false;
    }
    Mpu9250_I2C_SendByte(REG_Address);   //设置低起始地址
    Mpu9250_I2C_WaitAck();
    Mpu9250_I2C_SendByte(REG_data);
    Mpu9250_I2C_WaitAck();
    Mpu9250_I2C_Stop();
    delay_ms(5);	//-顾玉龙
    return true;
}
//单字节读取*****************************************
unsigned char Mpu9250_Single_Read(unsigned char SlaveAddress,unsigned char REG_Address) {
    unsigned char REG_data;
    if(!Mpu9250_I2C_Start())
        return false;
    Mpu9250_I2C_SendByte(SlaveAddress); //Mpu9250_I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址
    if(!Mpu9250_I2C_WaitAck()) {
        Mpu9250_I2C_Stop();
        return false;
    }
    Mpu9250_I2C_SendByte((u8) REG_Address);   //设置低起始地址
    Mpu9250_I2C_WaitAck();
    Mpu9250_I2C_Start();
    Mpu9250_I2C_SendByte(SlaveAddress+1);
    Mpu9250_I2C_WaitAck();

    REG_data= Mpu9250_I2C_RadeByte();
    Mpu9250_I2C_NoAck();
    Mpu9250_I2C_Stop();
    return REG_data;
}

/*******************************************************************************
* Function Name  : Mpu9250_I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
bool Mpu9250_I2C_Start(void) {
    Mpu9250_SDA_H;
    Mpu9250_SCL_H;
    Mpu9250_I2C_delay();
    if(!Mpu9250_SDA_read)return false;	//SDA线为低电平则总线忙,退出
    Mpu9250_SDA_L;
    Mpu9250_I2C_delay();
    if(Mpu9250_SDA_read) return false;	//SDA线为高电平则总线出错,退出
    Mpu9250_SDA_L;
    Mpu9250_I2C_delay();
    return true;
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void Mpu9250_I2C_Stop(void) {
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
    Mpu9250_SDA_L;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_H;
    Mpu9250_I2C_delay();
    Mpu9250_SDA_H;
    Mpu9250_I2C_delay();
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void Mpu9250_I2C_Ack(void) {
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
    Mpu9250_SDA_L;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_H;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void Mpu9250_I2C_NoAck(void) {
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
    Mpu9250_SDA_H;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_H;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
bool Mpu9250_I2C_WaitAck(void) { //返回为:=1有ACK,=0无ACK
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
    Mpu9250_SDA_H;
    Mpu9250_I2C_delay();
    Mpu9250_SCL_H;
    Mpu9250_I2C_delay();
    if(Mpu9250_SDA_read) {
        Mpu9250_SCL_L;
        Mpu9250_I2C_delay();
        return false;
    }
    Mpu9250_SCL_L;
    Mpu9250_I2C_delay();
    return true;
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void Mpu9250_I2C_SendByte(u8 SendByte) { //数据从高位到低位//
    u8 i=8;
    while(i--) {
        Mpu9250_SCL_L;
        Mpu9250_I2C_delay();
        if(SendByte&0x80)
            Mpu9250_SDA_H;
        else
            Mpu9250_SDA_L;
        SendByte<<=1;
        Mpu9250_I2C_delay();
        Mpu9250_SCL_H;
        Mpu9250_I2C_delay();
    }
    Mpu9250_SCL_L;
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave
****************************************************************************** */
unsigned char Mpu9250_I2C_RadeByte(void) { //数据从高位到低位//
    u8 i=8;
    u8 ReceiveByte=0;

    Mpu9250_SDA_H;
    while(i--) {
        ReceiveByte<<=1;
        Mpu9250_SCL_L;
        Mpu9250_I2C_delay();
        Mpu9250_SCL_H;
        Mpu9250_I2C_delay();
        if(Mpu9250_SDA_read) {
            ReceiveByte|=0x01;
        }
    }
    Mpu9250_SCL_L;
    return ReceiveByte;
}
/*******************************************************************************
* Function Name  : Mpu9250_I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void Mpu9250_I2C_delay(void) {
    u8 i=15; //这里可以优化速度	，经测试最低到5还能写入
    while(i) {
        i--;
    }
}






