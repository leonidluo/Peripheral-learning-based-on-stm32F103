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

//设备工作模式(用于记录设备状态)
u8 Lora_mode=0;//0:配置模式 

extern u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;


//LORA模块发送数据
void LoRa_SendData(void)
{    
    	u8 temp[256] = "Hello Lora!!! -- computer";
            //printf("%s\r\n",temp);
			u3_printf("%s\r\n",temp);
			
}


//Lora模块接收数据
void LoRa_ReceData(void)
{
    u16 len=0;  
    unsigned char *dataPtr;
    if(USART3_RX_STA&0x8000)
	{
       
        //printf("2");
		len = USART3_RX_STA&0X7FFF;
		USART3_RX_BUF[len]=0;//添加结束符
		USART3_RX_STA=0;
        
        //printf("OneNet_Publish\r\n");
        //esp8266_Publish(devPubtopic,(char *)USART3_RX_BUF);
        //OneNet_Publish(devPubtopic, (char *)USART3_RX_BUF);
		printf("接收到的数据为\r\n");
		printf("%s\r\n",USART3_RX_BUF);
        
        dataPtr = USER_GetJsonValue(USART3_RX_BUF,"temp");
        if(dataPtr != NULL)
        tempr = (dataPtr[0]-'0')*10+(dataPtr[1]-'0');
        
        dataPtr = USER_GetJsonValue(USART3_RX_BUF,"hum");
        if(dataPtr != NULL)
        hum = (dataPtr[0]-'0')*10+(dataPtr[1]-'0');
        
  }

}



//发送和接收数据处理进程


void Lora_Test(void)
{   
	  
}
