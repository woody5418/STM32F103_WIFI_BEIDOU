#ifndef _BH1750_H_
#define _BH1750_H_

#include "stm32f10x.h"
#include "sys.h"


//IO�������� PB0->SDA  PB1->SCL
#define BH1750_SDA_IN()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x08000000;}//����������
#define BH1750_SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x03000000;}//ͨ���������

//IO��������
#define BH1750_IIC_SCL    PBout(15) //SCL���
#define BH1750_IIC_SDA    PBout(14) //SDA���
#define BH1750_READ_SDA   PBin(14)  //SDA����



#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07

void Init_BH1750(void);
void Start_BH1750(void);
u8 Read_BH1750(void);
void Convert_BH1750(void);
float send_bh1750_data(void);

//IIC���в�������
void BH1750_IIC_Init(void);                //��ʼ��IIC��IO��
void BH1750_IIC_Start(void);				//����IIC��ʼ�ź�
void BH1750_IIC_Stop(void);	  			//����IICֹͣ�ź�
void BH1750_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 BH1750_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 BH1750_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void BH1750_IIC_Ack(void);					//IIC����ACK�ź�
void BH1750_IIC_NAck(void);				//IIC������ACK�ź�



#endif	//_DHT12_H_





