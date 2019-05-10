#ifndef __ESP8266_H_
#define __ESP8266_H_
#include "stm32f10x.h"
#include "stdbool.h"


/********************函数部分*********************/
bool Send_Order_RST(void);
bool Quit_Order_TT(void);
bool Send_Search_Connect_AP(void);
bool Send_WiFi_Mode(void);
bool Send_Start_Airkiss(void);
bool Send_Stop_Airkiss(void);
bool Send_Set_Connect_TCP(void);
bool Send_CLOSE_TCP_UDP(void);
bool Send_Passthrough(void);
bool Send_Data_Passthrough(void);
bool Send_Request_header(void);
bool Send_Search_Local_MAC(void);
/********************功能部分********************/
void sendCMD(char *cmd);
char* checkCMD(char *str);
char* Get_Search_Local_MAC(char *cmd,u16 waittime);
bool esp8266SendCMD(char *cmd,char *ack,u16 waittime);


#endif



