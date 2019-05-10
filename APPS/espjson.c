//********************************************************************
//
//      ���빦�ܣ�ESP8266 JSON��������
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************
#include "includes.h"

uint8_t FLAG_APPS_CJSON_SEND = 0;  // ��ʱ������cJson���Ʊ���
uint8_t FLAG_DATA_READ = 0; //���ݶ�ȡ����
uint8_t mess[16]; //У������


char js_beep_status[5] = "off";
char js_light_status[5] = "off";
char js_servo_status_value[6] = "";
char js_gprmc_long_status[14] = "";
char js_gprmc_lati_status[14] = "";

short  magXvalue=0;
short  magYvalue=0;

short  mag_x_max=0;
short  mag_x_min=0;

short  mag_y_max=0;
short  mag_y_min=0;

short mag_x_middle = 0;
short mag_y_middle = 0;

uint8_t  js_accel_x_status[5]= "000";
uint8_t  js_accel_y_status[5]= "000";
uint8_t  js_accel_z_status[5]= "000";
uint8_t  js_gyro_x_status[5]= "000";
uint8_t  js_gyro_y_status[5]= "000";
uint8_t  js_gyro_z_status[5]= "000";
uint8_t  js_mag_x_status[5]= "000";
uint8_t  js_mag_y_status[5]= "000";
uint8_t  js_mag_z_status[5]= "000";
uint16_t servo_status_value;      //���

float harmfulgas_status_value;    //MQ135����
float ultraviolet_status_value;   //������
float temperature_status_value;   //�¶�
float humidity_status_value;      //ʪ��
float photoconductor_status_value;//����
float pressure_status_value;      //��ѹ

double  gprmc_long_status;        //����
double  gprmc_lati_status;        //γ�� 
static  char check_errwebsocket[5] = {0X37,0XFA,0X21,0X3D,0X00};
char   json_get_temp[1024] = "";  //Json�洢 ����
uint8_t PACK[1024];
/**
 * ���յ�����2��ESPP8266����
 * �ж��Ƿ�ΪJSON������ִ����Ӧ�Ĺ���
 */
// ��������
void Apps_Analysis_Data(void)
{
    uint16_t len_ana=0;

    if (USART2_RX_STA & 0x8000) {
        if(cJson_Pars_data((char *)USART2_RX_BUF) == 0x01) { //���ݽ���  ����
            json_get_ItemObjectFirst((char *)USART2_RX_BUF,"welcome",json_get_temp);
            len_ana = USART2_RX_STA & 0x3FFF;
            memset(USART2_RX_BUF,0,len_ana);
        }
        USART2_RX_STA=0;
    }
}
/**
 * ���յ�����2��ESPP8266����
 * �ж��Ƿ�ΪJSON������ִ����Ӧ�Ĺ���
*/
char cJson_Pars_data(char *buffers)
{
    memset((char *)json_get_temp,0,1024);//������� д0 ��С1024Byte
    //��ȡ��json
    json_get_obj((char *) buffers, "services", (char *)json_get_temp);

    if ( json_check((char *)json_get_temp)  == 1) { //�Ϸ� Ϊ1  ���Ϸ�Ϊ0
        // beep ���Լ�ֵ
        json_get_value((char *) json_get_temp, "beep",(char *)js_beep_status);
        json_get_value((char *) json_get_temp, "light",(char *)js_light_status);
        json_get_value((char *) json_get_temp, "servo",(char *)js_servo_status_value);

        servo_status_value = (uint16_t)(atof(js_servo_status_value)/12+5);
        if(servo_status_value == 5) servo_status_value= 7;
        if(servo_status_value >= 25)  servo_status_value=20;
        TIM_SetCompare2(TIM3,servo_status_value);

        if(json_check_value((char *)js_beep_status,"on") == 1)
            Beep = 1;
        else
            Beep = 0;
        if(json_check_value((char *)js_light_status,"on") == 1)
            LED_Breating_light_1 = 1;
        else
            LED_Breating_light_1 = 0;
        //��������Ժ�ֱ�ӷ������ݵ�������
        SendDataServer();  //����json��������
        return 0;
    } else {
        if(checkJSON("Connection: close")) {
            TIM_Cmd(TIM4, DISABLE);//���������
            Control_Flag_Mode=0;
            Dis_Number = 10;  //��������ͷ
			LED_Blink_times(2);
        }else if(checkJSON("ets Jan")){
			TIM_Cmd(TIM4, DISABLE);//���������
			Control_Flag_Mode=0;
			Dis_Number=0;
			LED_Blink_times(3);
        }else  if(checkJSON(check_errwebsocket))
		{
			TIM_Cmd(TIM4, DISABLE);//���������
			Control_Flag_Mode=0;
			Dis_Number=0;
			LED_Blink_times(4);
		}else
			LED_Blink_times(5);
		return 1;
    }
}

/**
 * ���ݶ�ȡ
 */
void Data_Read(void)
{
    if((json_check_value((char *)js_beep_status,"on") != 1)&&(json_check_value((char *)js_beep_status,"off") != 1))
        sprintf((char *)js_beep_status,"%s"," off");
    if((json_check_value((char *)js_light_status,"on") != 1)&&(json_check_value((char *)js_light_status,"off") != 1))
        sprintf((char *)js_light_status,"%s"," off");

    harmfulgas_status_value = 100-(100*Get_Adc(ADC_Channel_0)/4095);   //(float)(Get_Adc(ADC_Channel_0)*(100.0/4096));//MQ135
    ultraviolet_status_value = (float)(Get_Adc(ADC_Channel_1)*(1.1/4096))*1000.0;  //������
	if(ultraviolet_status_value>=1000) ultraviolet_status_value = 1000;
	if(SHT2x_Init() == 0)
	{
		temperature_status_value = SHT2x_GetTempPoll();//��ȡSHT20 �¶�
		if(temperature_status_value>120)
			temperature_status_value=0;
		humidity_status_value = SHT2x_GetHumiPoll();//��ȡSHT20 ʪ��
		if(humidity_status_value>110)
			humidity_status_value=0;
	}
	//Init_BH1750();
	photoconductor_status_value = send_bh1750_data();//���ֵ��λlx
    if(Init_BMP180() != 0)
		pressure_status_value = (float)bmp180Convert(); // ѹǿ
}
/**
 * ��ȡMPU9250����
 */
void GET_MPU9250_DATA(void)
{
	if(Mpu9250_Init() == true)
	{
		read_Mpu9250_data(js_accel_x_status,js_accel_y_status,js_accel_z_status,
						  js_gyro_x_status,js_gyro_y_status,js_gyro_z_status,
						  js_mag_x_status,js_mag_y_status,js_mag_z_status);
		magXvalue = atoi((char *)js_mag_x_status);  // ��ȡX��ֵ
		magYvalue = atoi((char *)js_mag_y_status);	// ��ȡY��ֵ
		if(magXvalue>=mag_x_max)  mag_x_max = magXvalue;
		else if(magXvalue<=mag_x_min) mag_x_min = magXvalue;
		if(magYvalue>=mag_y_max)  mag_y_max = magYvalue;
		else if(magYvalue<=mag_y_min) mag_y_min = magYvalue;
		mag_x_middle = magXvalue - (mag_x_max+mag_x_min)/2;
		sprintf((char *)js_mag_x_status,"%3d",mag_x_middle);
		mag_y_middle = magYvalue - (mag_y_max+mag_y_min)/2;
		sprintf((char *)js_mag_y_status,"%3d",mag_y_middle);
	}
}

/**
 * ��ȡ����GPS����
 */
void GET_ublox_data(void)
{
    if(Received_GPS_DATA(js_gprmc_long_status,js_gprmc_lati_status)) {
        gprmc_long_status = atof(js_gprmc_long_status); //ת��Ϊfloat����
        gprmc_lati_status = atof(js_gprmc_lati_status); //ת��Ϊfloat����
    } else
        gprmc_long_status = gprmc_lati_status = 0.0;
}

/**
 * �������ݵ�������
 */
void SendDataServer(void)
{
    char packed_remove[64];
	
    memset((char *)PACK,0,1024);  //������� д0 ��С2048Byte
    strcpy((char *) PACK, "{\"id\":\"SWTZ_BigDipper_");
    strcat((char *) PACK, macbufs);
    strcat((char *) PACK, "\",\"to\":\"user\",");
    strcat((char *) PACK, "\"debug\":{\"variable\":\"");
    sprintf((char *)packed_remove,"%d,%d",NetNumber,Dis_Number);
    strcat((char *) PACK, packed_remove);
    strcat((char *) PACK, "\"},");
    strcat((char *) PACK, "\"services\":{\"beep\":\"");
    strcat((char *) PACK, (char *)js_beep_status);
    strcat((char *) PACK, "\",\"light\":\"");
    strcat((char *) PACK, (char *)js_light_status);
    strcat((char *) PACK, "\",\"servo\":\"");
    strcat((char *) PACK, (char *)js_servo_status_value);

    strcat((char *) PACK, "\",\"harmfulgas\":\"");
    sprintf(packed_remove, "%.1f", harmfulgas_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"ultraviolet\":\"");
    sprintf(packed_remove, "%.1f", ultraviolet_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"temperature\":\"");
    sprintf(packed_remove, "%.1f", temperature_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"humidity\":\"");
    sprintf(packed_remove, "%.1f", humidity_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"photoconductor\":\"");
    sprintf(packed_remove, "%.1f", photoconductor_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"pressure\":\"");
    sprintf(packed_remove, "%.1f", pressure_status_value);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"nineaxis\":{\"gyro_x_status\":\"");
    strcat((char *) PACK, (char *)js_gyro_x_status);
    strcat((char *) PACK, "\",\"gyro_y_status\":\"");
    strcat((char *) PACK, (char *)js_gyro_y_status);
    strcat((char *) PACK, "\",\"gyro_z_status\":\"");
    strcat((char *) PACK, (char *)js_gyro_z_status);

    strcat((char *) PACK, "\",\"accel_x_status\":\"");
    strcat((char *) PACK, (char *)js_accel_x_status);
    strcat((char *) PACK, "\",\"accel_y_status\":\"");
    strcat((char *) PACK, (char *)js_accel_y_status);
    strcat((char *) PACK, "\",\"accel_z_status\":\"");
    strcat((char *) PACK, (char *)js_accel_z_status);

    strcat((char *) PACK, "\",\"mag_x_status\":\"");
    strcat((char *) PACK, (char *)js_mag_x_status);
    strcat((char *) PACK, "\",\"mag_y_status\":\"");
    strcat((char *) PACK, (char *)js_mag_y_status);
    strcat((char *) PACK, "\",\"mag_z_status\":\"");
    strcat((char *) PACK, (char *)js_mag_z_status);

    strcat((char *) PACK, "\"},\"gprmc\":{\"long_status\":\"");
    sprintf(packed_remove, "%.6f", gprmc_long_status);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\",\"lati_status\":\"");
    sprintf(packed_remove, "%.6f", gprmc_long_status);
    strcat((char *) PACK, packed_remove);

    strcat((char *) PACK, "\"}}}");
    sendMessagePacket((char *)PACK);
}
/**
 * �������ݵ�SD��
 */
void SendDataToSd(void)
{
    uint16_t buff_lens=0,m,CRC_16_DATA;
    char sd_data_buffer[32];  // SD����ת������
	uint8_t sd_send_buff[512];        //SD�����ͻ���
	
    //memset((char *)sd_send_buff,0,512);   //������� д0 ��С512Byte
    strcpy((char *)sd_send_buff, (char *)js_beep_status);  // ƴ��beep����
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_light_status);  // ƴ��light����
    strcat((char *)sd_send_buff,",");
    sprintf(js_servo_status_value,"%d",servo_status_value);
    strcat((char *)sd_send_buff, (char *)js_servo_status_value);  // ƴ��servo����
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,harmfulgas_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);   //��������
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,ultraviolet_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // ����������
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,temperature_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // �¶�����
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,humidity_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // ʪ������
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,photoconductor_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // ��������
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,pressure_status_value);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // ѹǿ����
    strcat((char *)sd_send_buff,",");

    strcat((char *)sd_send_buff, (char *)js_gyro_x_status);  // gyro_x_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_gyro_y_status);  // gyro_y_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_gyro_z_status);  // gyro_z_status
    strcat((char *)sd_send_buff,",");

    strcat((char *)sd_send_buff, (char *)js_accel_x_status);  // accel_x_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_accel_y_status);  // accel_y_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_accel_z_status);  // accel_z_status
    strcat((char *)sd_send_buff,",");

    strcat((char *)sd_send_buff, (char *)js_mag_x_status);  // mag_x_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_mag_y_status);  // mag_y_status
    strcat((char *)sd_send_buff,",");
    strcat((char *)sd_send_buff, (char *)js_mag_z_status);  // mag_z_status
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,gprmc_long_status);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  // gprmc_long_status
    strcat((char *)sd_send_buff,",");
    f_to_string(sd_data_buffer,gprmc_lati_status);
    strcat((char *)sd_send_buff, (char *)sd_data_buffer);  //gprmc_lati_status

    buff_lens = strlen((char *)sd_send_buff);
    CRC_16_DATA = crc16((char *)sd_send_buff,buff_lens);
    sd_send_buff[buff_lens] = (CRC_16_DATA>>8)&0xff;
    sd_send_buff[buff_lens+1] = CRC_16_DATA&0xff;
    strcat((char *)sd_send_buff,"\r\n");
    buff_lens = strlen((char *)sd_send_buff);
    for(m=0; m<buff_lens; m++) {
        USART3->DR=sd_send_buff[m];
        while((USART3->SR&0X40)==0);//�ȴ����ͽ���
    }
}
/**
 * float����ת��Ϊ����С�����1λ���ַ���
 */
void f_to_string(char *buff_data,float values)
{
    uint8_t n;
    for(n=0; n<16; n++) {
        if(buff_data[n] != 0) buff_data[n]=0;
        else continue;
    }
    sprintf(buff_data,"%.1f",values);
}
/**
 * float����ת��Ϊ����С�����6λ���ַ���
 */
void f_to_string_gps(char *buff_data,float values)
{
    uint8_t n;
    for(n=0; n<16; n++) {
        if(buff_data[n] != 0) buff_data[n]=0;
        else continue;
    }
    sprintf(buff_data,"%.6f",values);
}


////////////********************** JSON  ���ݴ��� ***************************////////////////
/***************************************
 name:       ��ȡ��γ��
 input:      �ַ���
 output:
 description: �������ݴ���
 ***************************************/

char Received_GPS_DATA(char *weidu_value, char *jingdu_value)
{
    uint8_t i;
    uint16_t len,data_len;

    char *p="FFFFFF.FFFFFF";

    if(USART1_RX_STA&0x8000) {
        len=USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
        USART1_RX_STA=RX1_STATUS=0;
        strtok((char *)USART1_RX_BUF, ",");
        while (p) {
            p = strtok(NULL, ",");
            if (i == 2) {
                data_len =strlen(p);
                strncpy(weidu_value, p, data_len);
                weidu_value[data_len] = '\0';
            } else if (i == 4) {
                data_len =strlen(p);
                strncpy(jingdu_value, p, data_len);
                jingdu_value[data_len] = '\0';
            }
            i++;
        }
        memset(USART1_RX_BUF,0,len);//Clean_Anabuff((char *)Ana_Buffer,USART2_MAX_RECV_LEN);
    }
    if(weidu_value[0] == 0) return 0;
    else if(jingdu_value[0] == 0) return 0;
    else return 1;
}
/***************************************
 name:       JSON У�麯��
 input:      �ַ���
 output:     �Ϸ�JAON ����1 ���Ϸ�JSON ����0
 description:
 ***************************************/
char json_check(char *str)
{
    //�����ַ����ȱ���
    uint16_t str_length;
    //�����ַ�����
    str_length = strlen(str);
    //ͨ����β�������ж��Ƿ�ΪJSON
    if (str[0] == '{' && str[str_length - 1] == '}') {
        //����ַ���Ϊ�Ϸ�JSON ����1
        return 1;
    } else {
        //����ַ���Ϊ�Ϸ�JSON ����0
        return 0;
    }
}

/***************************************
 name:       JSON ��ȡ��ֵ����
 input:      JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ���ַ���
 output:     �����ȡ�ɹ�����1 ��ȡʧ�ܷ���0
 description:
 ***************************************/
char json_get_value(char *json, char *json_key, char *json_value)
{
    //���������ʼ��λ��
    char *json_key_start;
    //�������������λ��
    char *json_key_end;
    //�����������
    char json_key_length;
    //�����ֵ��ʼ��λ��
    char *json_value_start;
    //�����ֵ������λ��
    char *json_value_end;
    //�����ֵ����
    char json_value_length;
    //��ȡ������ʼ��λ��
    json_key_start = strstr(json, json_key);
    //��ȡ�����ĳ���
    json_key_length = strlen(json_key);
    //��ȡ����������λ��
    json_key_end = json_key_start + json_key_length;

    if (json_key_start != 0 && *(json_key_start - 1) == '\"'
        && *(json_key_end) == '\"' && *(json_key_end + 1) == ':'
        && *(json_key_end + 2) == '\"') {
        //��ȡ��ֵ��ʼ��λ��
        json_value_start = json_key_end + 3;
        //��ȡ��ֵ������λ��
        json_value_end = strstr(json_value_start, "\"");
        //��ȡ��ֵ�ĳ���
        json_value_length = json_value_end - json_value_start;
        //����ֵ����ָ������
        strncpy(json_value, json_value_start, json_value_length);
        //ָ�����һλ���ַ��������� \0
        json_value[json_value_length] = '\0';
        //�ɹ���ȡ��ֵ ����1
        return 1;
    } else {
        json_value[0] = '\0';
        //ʧ�ܻ�ȡ��ֵ ����0
        return 0;
    }
}

/***************************************
 name:      JSON ��ȡ��JSON��ֵ��ֵ��һ����ȡ��
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ��ֵ
 output:    ��
 description:
 ***************************************/
void json_get_ItemObject(char *json, char *json_key, char *json_value)
{
    int  json_len;
    char *json_get_Item_obj;
    char *ptr_obj;

    json_get_Item_obj = strstr(json, json_key);
    json_get_Item_obj = strstr(json_get_Item_obj, "{");
    ptr_obj = strchr(json_get_Item_obj, '}');
    json_len = strlen(json_get_Item_obj) - strlen(ptr_obj);
    strncpy(json_value, json_get_Item_obj, json_len);
    json_value[json_len] = '\0';
}

/***************************************
 name:      JSON ��ȡ��JSON��ֵ��ֵ
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ��ֵ
 output:    ��
 description:
 ***************************************/
void json_get_ItemObjectFirst(char *json, char *json_key, char *json_value)
{
    int  json_len_Fir;
    char *json_get_Item_obj_Fir;
    char *ptr_obj_Fir;

    json_get_Item_obj_Fir = strstr(json, json_key);
    json_get_Item_obj_Fir = strstr(json_get_Item_obj_Fir, "{");
    ptr_obj_Fir = strrchr(json_get_Item_obj_Fir, '}');
    json_len_Fir = strlen(json_get_Item_obj_Fir) - strlen(ptr_obj_Fir);
    strncpy(json_value, json_get_Item_obj_Fir, json_len_Fir);
    json_value[json_len_Fir] = '\0';
}

/***************************************
 name:      JSON ��ȡ��JSON(������ȡ)
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��json�Ĵ�
 output:    ��
 description:
 ***************************************/
void json_get_obj(char *json, char *json_key,char *json_objectItem)
{
    int json_len;
    char *json_get_temp_obj;
    char *ptr;

    json_get_temp_obj = strstr(json,json_key);
    json_get_temp_obj = strstr(json_get_temp_obj,"{");
    ptr = strchr(json_get_temp_obj,'}');
    json_len =strlen(json_get_temp_obj)-strlen(ptr)+1;
    strncpy(json_objectItem, json_get_temp_obj, json_len);
    json_objectItem[json_len] = '\0';
}


/***************************************
 name:       JSON ��ֵ�ԱȺ���
 input:      JSON ��ֵ Ҫƥ����ַ�
 output:     ���ƥ��ɹ�����1 ʧ�ܷ���0
 description:
 ***************************************/
char json_check_value(char *str1, char *str2)
{
    if (strcmp(str1, str2) == 0) {
        return 1;//ƥ��ɹ�����1
    } else {
        return 0;//ƥ��ɹ�����0
    }
}
/***************************************
 name:       JSON ��鷵�ص������Ƿ�����Ҫ������
 input:      JSON ��ֵ Ҫƥ����ַ�
 output:     ���ƥ��ɹ�����1 ʧ�ܷ���0
 description:
 ***************************************/
char* checkJSON(char *str)
{
    char *strx=0;
    strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
    return (char*)strx;
}


