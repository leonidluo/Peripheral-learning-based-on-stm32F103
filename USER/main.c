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

const char *devSubtopic[] = {"th"};//mqtt订阅的主题
const char *devSubtopic_2[] = {"state"};//mqtt订阅的主题

const char *devPubtopic = "machine" ;//mqtt订阅的主题
const char *devPubtopic_2 = "machine_data" ;//mqtt订阅的主题
int  status = 0;


 int main(void)
 {		
//    long i;
//    unsigned short timeCount = 0;	//发送间隔变量
//    uint8_t flag=1;
//    unsigned short timetest = 0; 
    
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口 
	BEEP_Init();		 	//初始化蜂鸣器IO
	EXTI4_Init();           //初始化外部中断输入
    EXTI3_Init();
    
     
    RTC_Init();
     
    
    usart3_init(115200); //lora串口
    usart3_rx(1);//使能
     
    usart2_init(115200);//esp8266串口 
    usart2_rx(1);
    LED0=0;					//先点亮红灯
     
    
    //SYSTICK_init();
	LF125K_init();
    	
    TIM2_Int_Init(99,7199);
    ESP8266_Init();
    esp8266_Subscribe(devSubtopic);
    esp8266_Subscribe(devSubtopic_2);
    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
//    printf("低频卡测试\r\n");
//	flag=LF125K_card_init();
//	if(flag)
//		printf("卡初始成功\r\n");
//	else
//		printf("卡初始失败\r\n");
    
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
//        printf("Time:%d-%d-%d %d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间

        delay_ms(1000);
    }
}
