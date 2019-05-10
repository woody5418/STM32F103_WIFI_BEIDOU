/********************************************************************
 *@file        adc.h
 *@author      Woody      QQ��2490006131
 *@version     V1.0
 *@date        2016-10-14
 *@brief       ADC ��ʼ��
 *******************************************************************/
#ifndef __ESPJSON_H
#define __ESPJSON_H

#include "stm32f10x.h"
#include <stdint.h>
#include "usart2.h"

extern uint8_t FLAG_APPS_CJSON_SEND;  // ��ʱ������cJson���Ʊ���
extern uint8_t FLAG_DATA_READ; //���ݶ�ȡ����
extern uint16_t servo_status_value;  //���
extern char js_servo_status_value[6];

/*-----------------------CJSON----------------------*/
//JSON У�麯��
char json_check(char *str);
char json_get_value(char *json, char *json_key, char *json_value);
char json_check_value(char *str1, char *str2);
void json_get_ItemObject(char *json, char *json_key, char *json_value);
void json_get_ItemObjectFirst(char *json, char *json_key, char *json_value);
void json_get_obj(char *json, char *json_key,char *json_objectItem);
char* checkJSON(char *str);
/*----------------------- ���� ----------------------*/

void Apps_Analysis_Data(void);
char cJson_Pars_data(char *buffers);
void Data_Read(void);
void GET_ublox_data(void);
void GET_MPU9250_DATA(void);
void SendDataToSd(void);
void SendDataServer(void);
void f_to_string(char *buff_data,float values);
void f_to_string_gps(char *buff_data,float values);
char Received_GPS_DATA(char *weidu_value, char *jingdu_value);
double atof(const char *nptr);

#endif




