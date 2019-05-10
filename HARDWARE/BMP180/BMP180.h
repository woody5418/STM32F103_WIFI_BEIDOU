#ifndef __BMP180_H__
#define __BMP180_H__
#include "stm32f10x.h"
#include "sys.h"

#define	 BMP180_SlaveAddress     0xee	//气压传感器器件地址

#define OSS 0	 //	BMP085使用
#define REG_Pressure 0x34 // oss=0 4.5

//IO方向设置
#define Bmp180_SDA_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x00000008;}
#define Bmp180_SDA_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x00000003;}

//IO操作函数
#define Bmp180_IIC_SCL    PBout(1) //SCL
#define Bmp180_IIC_SDA    PBout(0) //SDA	 
#define Bmp180_READ_SDA   PBin(0)  //输入SDA 

u16 Init_BMP180(void);//初始化IO   读取校准参数
uint32_t bmp180Convert(void);	//校准温度和气压 ，将结果分别存入 result_UP result_UT
/************************* 模拟IIC函数 **********************/
//IIC所有操作函数
void Bmp180_IIC_Init(void);                //初始化IIC的IO口
void Bmp180_IIC_Start(void);				//发送IIC开始信号
void Bmp180_IIC_Stop(void);	  			//发送IIC停止信号
void Bmp180_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t Bmp180_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t Bmp180_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void Bmp180_IIC_Ack(void);					//IIC发送ACK信号
void Bmp180_IIC_NAck(void);				//IIC不发送ACK信号

void Bmp180_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t Bmp180_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);


#endif

