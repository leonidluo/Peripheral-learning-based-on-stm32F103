#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include <string.h>
#include "esp8266.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
void EXTI4_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStrue;
    
    NVIC_InitTypeDef NVIC_InitStrue;
    
    KEY_Init();
    
   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
    
    EXTI_InitStrue.EXTI_Line= EXTI_Line4 ;
    EXTI_InitStrue.EXTI_LineCmd= ENABLE;
    EXTI_InitStrue.EXTI_Mode = EXTI_Mode_Interrupt ;
    EXTI_InitStrue.EXTI_Trigger=EXTI_Trigger_Falling ;
    
    
    EXTI_Init(&EXTI_InitStrue);
    
    NVIC_InitStrue.NVIC_IRQChannel= EXTI4_IRQn ;
    NVIC_InitStrue.NVIC_IRQChannelCmd= ENABLE ;
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority= 1;
    NVIC_InitStrue.NVIC_IRQChannelSubPriority= 1 ;
    
    
    NVIC_Init(&NVIC_InitStrue);
}

void EXTI3_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStrue;
    
    NVIC_InitTypeDef NVIC_InitStrue;
    
//    KEY_Init();
    
   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
    
    EXTI_InitStrue.EXTI_Line= EXTI_Line3 ;
    EXTI_InitStrue.EXTI_LineCmd= ENABLE;
    EXTI_InitStrue.EXTI_Mode = EXTI_Mode_Interrupt ;
    EXTI_InitStrue.EXTI_Trigger=EXTI_Trigger_Falling ;
    
    
    EXTI_Init(&EXTI_InitStrue);
    
    NVIC_InitStrue.NVIC_IRQChannel= EXTI3_IRQn ;
    NVIC_InitStrue.NVIC_IRQChannelCmd= ENABLE ;
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority= 1;
    NVIC_InitStrue.NVIC_IRQChannelSubPriority= 1 ;
    
    
    NVIC_Init(&NVIC_InitStrue);
}

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(WK_UP==1)	 	 //WK_UP����
	{				 
		BEEP=!BEEP;	
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}
 

//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY1==0)	 //����KEY1
	{				 
		LED0=0;
        LED1=1;
	}		 
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY0==0)	 //����KEY0
	{
        
        
        LED0=1;
        LED1=0; 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}
 
