#ifndef __BMP180_H__
#define __BMP180_H__
#include "stm32f10x.h"
#include "sys.h"

#define	 BMP180_SlaveAddress     0xee	//��ѹ������������ַ

#define OSS 0	 //	BMP085ʹ��
#define REG_Pressure 0x34 // oss=0 4.5

//IO��������
#define Bmp180_SDA_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x00000008;}
#define Bmp180_SDA_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x00000003;}

//IO��������
#define Bmp180_IIC_SCL    PBout(1) //SCL
#define Bmp180_IIC_SDA    PBout(0) //SDA	 
#define Bmp180_READ_SDA   PBin(0)  //����SDA 

u16 Init_BMP180(void);//��ʼ��IO   ��ȡУ׼����
uint32_t bmp180Convert(void);	//У׼�¶Ⱥ���ѹ ��������ֱ���� result_UP result_UT
/************************* ģ��IIC���� **********************/
//IIC���в�������
void Bmp180_IIC_Init(void);                //��ʼ��IIC��IO��
void Bmp180_IIC_Start(void);				//����IIC��ʼ�ź�
void Bmp180_IIC_Stop(void);	  			//����IICֹͣ�ź�
void Bmp180_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t Bmp180_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t Bmp180_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void Bmp180_IIC_Ack(void);					//IIC����ACK�ź�
void Bmp180_IIC_NAck(void);				//IIC������ACK�ź�

void Bmp180_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t Bmp180_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);


#endif

