#include <string.h>
#include "fun.h"
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
#include "lf125k.h"
#include "rtc.h"


int tempr; //保存c8t6温湿度数据
int hum;

const char * id = "4";
static u16 syear=2023;
static u8 smon=0,sday=0,hour=0,min=0, sec=0;

void Machine_Process(void)
{
        unsigned char *dataPtr = NULL;
        int timetest = 5;
        do
        {
            if(ESP8266_WaitRecive() == REV_OK)
            {
                printf("1\r\n");
//                dataPtr = USER_GetJsonValue(esp8266_buf,"running");
//                if(dataPtr != NULL)
//                {
//                    if(*dataPtr-'0'==0)break;
//                    else if(*dataPtr-'0'==1)status=1;
//                }
                
                dataPtr = USER_GetJsonValue(esp8266_buf,"id");
                if(dataPtr != NULL)
                {
                    //printf("555\r\n");
                    if(strcmp((const char *)dataPtr,id)==0)
                    {
                        //printf("666\r\n");
                        dataPtr = USER_GetJsonValue(esp8266_buf,"state");
                        if(dataPtr != NULL)
                        {   
                            if(strcmp((const char *)dataPtr,"running")==0)
                            {
                                status=1;
                            }
                            else if(strcmp((const char *)dataPtr,"pause")==0)
                            {
                                status=2;
                            }
                            else if(strcmp((const char *)dataPtr,"reset")==0)
                            {
                                status=0;
                            }
                        }
                        
                    }
                }
                
                dataPtr = USER_GetJsonValue(esp8266_buf,"error");
                if(dataPtr != NULL)
                {
                    if(*dataPtr-'0'==0)break;
                    else if(*dataPtr-'0'==1)status=2;
                }
                dataPtr = USER_GetJsonValue(esp8266_buf,"reset");
                if(dataPtr != NULL)
                {
                    if(*dataPtr-'0'==0)break;
                    else if(*dataPtr-'0'==1)status=0;
                }
            }
            delay_ms(5);
        }while(timetest--);
}

int  set_th=30;
char machine[255],timereq[20];
void Reset(void)
{
    
    unsigned char *dataPtr = NULL;
    //unsigned short timetest = 0; 
    int flag =1;
    int timetest = 3;
    sprintf(timereq,"2023-8-1 %02d:%02d:%02d", calendar.hour,calendar.min,calendar.sec);
    
    sprintf(machine,"{\\\"id\\\":\\\"%s\\\"\\,\\\"location\\\":\\\"library\\\"\\,\\\"state\\\":\\\"reset\\\"\\,\\\"updateTime\\\":\\\"%s\\\"\\,\\\"th\\\":%d\\,\\\"tds\\\":111}",\
    id,timereq,set_th);
    
    esp8266_Publish(devPubtopic,machine);
    
    while(status == 0)
    {
        //printf("Time:%d-%d-%d %d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间
        
        if(flag)
        {
            //esp8266_Publish(devPubtopic,machine);
            Machine_Process();
        }
        else
        {
            //esp8266_Publish(devPubtopic,timereq);
            timetest = 5;
            do
            {
                if(ESP8266_WaitRecive() == REV_OK)
                {
                    dataPtr = USER_GetJsonValue(esp8266_buf,"yearreq");
                    if(dataPtr != NULL)
                    {
                        printf("%s\r\n",dataPtr);
                        if(*dataPtr-'0'==2)
                        {
                            TimeYearSet((char *)dataPtr);
                            
                            
                        }     
                    }
                    dataPtr = USER_GetJsonValue(esp8266_buf,"dayreq");
                    if(dataPtr != NULL)
                    {
                        printf("%s\r\n",dataPtr);
                        if(*dataPtr-'0'>=0)
                        {
                            TimeDaySet((char *)dataPtr);
                            flag=1;
                            
                        }
                        
                    }
                }
               delay_ms(5);
             }while(timetest--);
            Machine_Process();
         }
        delay_ms(1000);
     }
        
        
        
        
    
}






void Pause(void)
{
	int ret;
	//int ret1;
    uint8_t buf[8];
	//uint8_t buf1[8];
	memset(buf, 0, sizeof(buf));
    
    while(status == 2)
    {
        printf("低频卡测试\r\n");
        ret=LF125K_read_1_page(buf);
    
        if(ret == 0)
        {
         printf("低频卡获取成功！");
         printf("id card:%X %X %X %X %X %X %X %X\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
         status = 1 ;
        }
        else
        {
         printf("低频卡获取失败！");
        }
        
        delay_ms(1000);
        Machine_Process();
    }
	

}

void Running(void)
{
    unsigned short timeCount = 0;	//发送间隔变量
    unsigned char *dataPtr = NULL;
    unsigned short timetest = 0; 
    while(status == 1)
    {
        if(timeCount%4==0) //   1000 / 25 = 40  1s运行一次
        {
            KEY_Process();	
            
            
        }
        if(timeCount%20==0) //   1000 / 25 = 40  1s运行一次
        {
            //LED0=~LED0;	
            if(USART_RX_STA&0x8000)
            {
               SendReceiveData();
               
               USART_RX_STA=0;
            }
            
        }
         /*******************LoRa***********************/
        if(timeCount%200==0)
        {
            //LoRa_SendData();
            LoRa_ReceData(); //接收lora信息，并且发送给服务器
            ESP8266_Clear();
        }
            
            
          /*****************************************/ 
        /***********************WIFI*******************/

        if(timeCount >= 800)									//发送间隔5s
        {
            timeCount=0;
//            LoRa_SendData();
//            LoRa_ReceData();
            //timeCount = 0;
//            status = 2;
            
        }
        timetest = 3;
        do
        {
            
            if(ESP8266_WaitRecive() == REV_OK)
            {
                timeCount = 0;
                printf("收到服务器信息：%s\r\n",esp8266_buf);
                dataPtr = USER_GetJsonValue(esp8266_buf,"LED_SW");
                printf("%c\r\n",*dataPtr);
                if(dataPtr != NULL)
                {
                    if(*dataPtr-'0'==0)LED0=1;
                    else if(*dataPtr-'0'==1)LED0=0;
                }
//                dataPtr = USER_GetJsonValue(esp8266_buf,"FAN_SW");
//                if(dataPtr != NULL)
//                {
//                    if(*dataPtr-'0'==0)u3_printf("FAN_OFF\r\n");
//                    else if(*dataPtr-'0'==1)u3_printf("FAN_ON1\r\n");
//                    else if(*dataPtr-'0'==2)u3_printf("FAN_ON2\r\n");
//                    else if(*dataPtr-'0'==3)u3_printf("FAN_ON3\r\n");
//                }
                
                
                
            }
            delay_ms(5);   
        }while(timetest--);
        Machine_Process();
     /*****************************************/       
        timeCount++; 
        delay_ms(10);
    }     	 
}
void TimeYearSet(char *dataPtr)
{
    int i;
    
    for(i=0;i<4;i++)
    {
        syear*=10;
        syear+=dataPtr[i]-'0';
    }
    
    
    
//x    printf("%d %d %d %d %d %d ",syear,smon,sday,hour,min,sec);
//    RTC_Set(syear,smon,sday,hour,min,sec);
}


void TimeDaySet(char *dataPtr)
{
    smon=(dataPtr[0]-'0')*10+(dataPtr[1]-'0');
    sday=(dataPtr[2]-'0')*10+(dataPtr[3]-'0');
    hour=(dataPtr[4]-'0')*10+(dataPtr[5]-'0');
    min =(dataPtr[6]-'0')*10+(dataPtr[7]-'0');
    sec =(dataPtr[8]-'0')*10+(dataPtr[9]-'0');
    RTC_Set(2023,smon,sday,hour,min,sec);
    return ;
}
