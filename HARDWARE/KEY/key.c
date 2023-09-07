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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE4,3

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
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
