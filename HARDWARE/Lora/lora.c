#include "lora.h"
#include "sys.h"
#include "delay.h"
#include "usart3.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "led.h"
#include "fun.h"
#include "esp8266.h"

char PUB_BUF[256];

//�豸����ģʽ(���ڼ�¼�豸״̬)
u8 Lora_mode=0;//0:����ģʽ 

extern u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;


//LORAģ�鷢������
void LoRa_SendData(void)
{    
    	u8 temp[256] = "Hello Lora!!! -- computer";
            //printf("%s\r\n",temp);
			u3_printf("%s\r\n",temp);
			
}


//Loraģ���������
void LoRa_ReceData(void)
{
    u16 len=0;  
    unsigned char *dataPtr;
    if(USART3_RX_STA&0x8000)
	{
       
        //printf("2");
		len = USART3_RX_STA&0X7FFF;
		USART3_RX_BUF[len]=0;//��ӽ�����
		USART3_RX_STA=0;
        
        //printf("OneNet_Publish\r\n");
        //esp8266_Publish(devPubtopic,(char *)USART3_RX_BUF);
        //OneNet_Publish(devPubtopic, (char *)USART3_RX_BUF);
		printf("���յ�������Ϊ\r\n");
		printf("%s\r\n",USART3_RX_BUF);
        
        dataPtr = USER_GetJsonValue(USART3_RX_BUF,"temp");
        if(dataPtr != NULL)
        tempr = (dataPtr[0]-'0')*10+(dataPtr[1]-'0');
        
        dataPtr = USER_GetJsonValue(USART3_RX_BUF,"hum");
        if(dataPtr != NULL)
        hum = (dataPtr[0]-'0')*10+(dataPtr[1]-'0');
        
  }

}



//���ͺͽ������ݴ������


void Lora_Test(void)
{   
	  
}
