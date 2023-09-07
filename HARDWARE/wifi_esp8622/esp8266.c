/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>


#define ESP8266_WIFI_INFO		"AT+CWJAP=\"nova 7 5G\",\"87654321\"\r\n"

//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"192.168.20.247\",1883\r\n"

#define ESP8266_MQTT_USERCFG    "AT+MQTTUSERCFG=0,1,\"1413\",\"lo\",\"66\",0,0,\"\"\r\n"

#define ESP8266_MQTT_CONN       "AT+MQTTCONN=0,\"192.168.43.112\",1883,0\r\n"


unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
unsigned char esp8266_buf[200];

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
            else 
            {
                printf("%s\r\n",esp8266_buf);
            }
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART2, data, len);		//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
        
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
             printf("%s",esp8266_buf);
			ptrIPD = strstr((char *)esp8266_buf, "MQTTSUBRECV,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ��?
		timeOut--;
	} while(timeOut>0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP8266_Init
//
//	�������ܣ�	��ʼ��ESP8266
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//ESP8266��λ����
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-��λ
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
	delay_ms(250);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
	delay_ms(500);
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "0. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "1. RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
	delay_ms(500);
	
	ESP8266_SendCmd("2.AT+CIPCLOSE\r\n", "");
	delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "3. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. AT+CWDHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);
	
//	UsartPrintf(USART_DEBUG, "6. CIPSTART\r\n");
//	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
//		delay_ms(500);
    
    UsartPrintf(USART_DEBUG, "6. MQTTUSERCFG\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_USERCFG, "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "7. MQTT_CONN\r\n");
	while(ESP8266_SendCmd(ESP8266_MQTT_CONN, "OK"))
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "8. ESP8266 Init OK\r\n");

}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����3�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
    

}

void esp8266_Subscribe(const char *topics[])
{
    char cmdBuf[70];
    //UsartPrintf(USART_DEBUG, "sub %s\r\n",topics);
	
	ESP8266_Clear();
    sprintf(cmdBuf, "AT+MQTTSUB=0,\"%s\",0\r\n",*topics);
    
	while(ESP8266_SendCmd(cmdBuf, "OK"))
		delay_ms(500);
}
void esp8266_Publish(const char *topic, const char *msg)
{
    int cnt=5;
    char cmdBuf[500];
   UsartPrintf(USART_DEBUG, "pub %s : %s\r\n",topic,msg);
    
	
	ESP8266_Clear();
    sprintf(cmdBuf, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n",topic,msg);
    
    
    
	while(ESP8266_SendCmd(cmdBuf, "OK")&&cnt>=0)
    {
        cnt--;
		delay_ms(500);
    }
}


/*******************************************************************
*������char *USER_GetJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ�����ȡ����
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ���ַ�����ʽ����ʼ��ַ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
unsigned char *USER_GetJsonValue(unsigned char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[100];
	int8_t i=0;
	//int flagX = 0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
        
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	
	if(*target == '"')// ������ַ���
	{
        
		i = 0;
		target++;
		while(1){
			if(*target != '\\' && *(target + 1) == '"'){
				temp[i] = *target;
				break;
			}
			temp[i] = *target;
			target++;
			i++;
			if(i >= 99)
				break;
		}
	}
	else {
        
		for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
		{
			if((*target<='9')&&(*target>='0'))
			{
				temp[i]=*target;
			}
			else
			{
				break;
			}
		}
	}
	temp[i+1] = '\0';
	printf("��ֵ=%s\r\n",temp);
    
	return (unsigned char *)temp;
}

