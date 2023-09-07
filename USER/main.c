#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "beep.h"
#include "usart3.h"
#include "lora.h"
#include "usart2.h"
#include "esp8266.h"
#include "fun.h"
#include "lf125k.h"
#include "systick.h"
#include "timer.h"
#include "rtc.h"

const char *devSubtopic[] = {"th"};//mqtt���ĵ�����
const char *devSubtopic_2[] = {"state"};//mqtt���ĵ�����

const char *devPubtopic = "machine" ;//mqtt���ĵ�����
const char *devPubtopic_2 = "machine_data" ;//mqtt���ĵ�����
int  status = 0;


 int main(void)
 {		
//    long i;
//    unsigned short timeCount = 0;	//���ͼ������
//    uint8_t flag=1;
//    unsigned short timetest = 0; 
    
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ� 
	BEEP_Init();		 	//��ʼ��������IO
	EXTI4_Init();           //��ʼ���ⲿ�ж�����
    EXTI3_Init();
    
     
    RTC_Init();
     
    
    usart3_init(115200); //lora����
    usart3_rx(1);//ʹ��
     
    usart2_init(115200);//esp8266���� 
    usart2_rx(1);
    LED0=0;					//�ȵ������
     
    
    //SYSTICK_init();
	LF125K_init();
    	
    TIM2_Int_Init(99,7199);
    ESP8266_Init();
    esp8266_Subscribe(devSubtopic);
    esp8266_Subscribe(devSubtopic_2);
    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
//    printf("��Ƶ������\r\n");
//	flag=LF125K_card_init();
//	if(flag)
//		printf("����ʼ�ɹ�\r\n");
//	else
//		printf("����ʼʧ��\r\n");
    
	BEEP=1;
    delay_ms(250);
    BEEP=0;
    
	while(1)
	{	
        if(status == 0 )
        {
            Reset();
            BEEP=1;
            delay_ms(250);
            BEEP=0;
        }
        if(status == 1) 
        {
            Running();
        }
        if(status == 2) 
        {
            Pause();
            BEEP=1;
            delay_ms(250);
            BEEP=0;
        }
//        printf("Time:%d-%d-%d %d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//�������ʱ��

        delay_ms(1000);
    }
}
