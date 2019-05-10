#ifndef __SHT2x_H__
#define __SHT2x_H__
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif

//IO��������
#define SHT2x_SDA_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=0x00008000;}
#define SHT2x_SDA_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=0x00003000;}

//IO��������
#define SHT2x_IIC_SCL    PAout(15) //SCL
#define SHT2x_IIC_SDA    PBout(3) //SDA
#define SHT2x_READ_SDA   PBin(3)  //SDA

#define  SHT2x_I2C_ADR_W 0x80  //SHT2X IIC ����ַ
#define  SHT2x_I2C_ADR_R 0x81  //SHT2X IIC д��ַ

typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3, // ���� �¶� ���� ����������   ����
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5, // ���� ʪ�� ���� ����������   ����
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3, // ���� �¶� ���� ���Ǳ������� ����
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5, // ���� ʪ�� ���� ���Ǳ������� ����
    USER_REG_W                 = 0xE6, // д�Ĵ�������
    USER_REG_R                 = 0xE7, // ���Ĵ�������
    SOFT_RESET                 = 0xFE  // �����λ����
} SHT2xCommand;

typedef enum {
    SHT2x_RES_12_14BIT         = 0x00, //RH=12bit, T=14bit ����Ĭ�ϵ�ֵ   ���ǲ����޸�
    SHT2x_RES_8_12BIT          = 0x01, //RH= 8bit, T=12bit
    SHT2x_RES_10_13BIT         = 0x80, //RH=10bit, T=13bit
    SHT2x_RES_11_11BIT         = 0x81, //RH=11bit, T=11bit
    SHT2x_RES_MASK             = 0x81  //Mask for res. bits (7,0) in user reg.
} SHT2xResolution;

typedef enum {
    SHT2x_HEATER_ON            = 0x04, //heater on
    SHT2x_HEATER_OFF           = 0x00, //heater off
    SHT2x_HEATER_MASK          = 0x04  //Mask for Heater bit(2) in user reg.
} SHT2xHeater;

typedef struct {
    float TEMP_POLL;
    float HUMI_POLL;
} SHT2x_data;

extern SHT2x_data SHT20;

u8 SHT2x_Init(void);
u8 SHT2x_SoftReset(void);  //SHT20�����λ

float SHT2x_GetTempPoll(void);//��ȡSHT20 �¶�
float SHT2x_GetHumiPoll(void);//��ȡSHT20 ʪ��

//IIC���в�������
void SHT2x_IIC_Init(void);                //��ʼ��IIC��IO��
void SHT2x_IIC_Start(void);				//����IIC��ʼ�ź�
void SHT2x_IIC_Stop(void);	  			//����IICֹͣ�ź�
void SHT2x_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 SHT2x_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 SHT2x_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void SHT2x_IIC_Ack(void);					//IIC����ACK�ź�
void SHT2x_IIC_NAck(void);				//IIC������ACK�ź�

void SHT2x_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 SHT2x_IIC_Read_One_Byte(u8 daddr,u8 addr);


#ifdef __cplusplus
}
#endif

#endif



