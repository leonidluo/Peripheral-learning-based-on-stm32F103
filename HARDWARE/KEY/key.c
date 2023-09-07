#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "led.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include "esp8266.h"
#include "rtc.h"
#include "fun.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE4,3

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}
extern char machine[255];
int volume = 500;
void KEY_Process()
{
    int i;
    char time[30];
    i=KEY_Scan(0);
    if(i==0)return ;
    else if(i==1)
    {
        memset(machine, 0, sizeof(machine));
        printf("temp:%d hum:%d\r\n",tempr,hum);
        if(status == 0)sprintf(machine,"{\\\"id\\\":\\\"%s\\\"\\,\\\"location\\\":\\\"library\\\"\\,\\\"state\\\":\\\"reset\\\"}",id);
		else if(status == 1)
        {
            sprintf(time, "2023-8-1 %02d:%02d:%02d", calendar.hour,calendar.min,calendar.sec);
            
            sprintf(machine,"[{\\\"id\\\":\\\"%s\\\"\\,\\\"cardNumber\\\":\\\"3\\\"\\,\\\"volume\\\":%d\\,\\\"time\\\":\\\"%s\\\"}]",id,volume,time);
        }
        else if(status == 2)sprintf(machine,"{\\\"id\\\":\\\"%s\\\"\\,\\\"location\\\":\\\"library\\\"\\,\\\"state\\\":\\\"pause\\\"}",id);
        esp8266_Publish(devPubtopic_2,machine);
        
    }
    else if(i==2)
    {
        memset(machine, 0, sizeof(machine));
        status = 2;
        sprintf(time, "2023-8-1 %02d:%02d:%02d", calendar.hour,calendar.min,calendar.sec);
        sprintf(machine,"{\\\"id\\\":\\\"%s\\\"\\,\\\"location\\\":\\\"library\\\"\\,\\\"state\\\":\\\"error\\\"\\,\\\"updateTime\\\":\\\"%s\\\"\\,\\\"th\\\":%d\\,\\\"tds\\\":111}",\
    id,time,set_th);
    esp8266_Publish(devPubtopic,machine);
    
    }
    printf("55\r\n");
}
