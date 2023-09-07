/* ==================================================================
#     FileName: lf125k.c
#         Desc: T5557 card
#       Author: Allen
#        Email: 1518859810@qq.com
#      Version: 0.0.1
#   LastChange: 2015-08-13 15:12:56
#      History:
================================================================== */
/* Includes --------------------*/
#include <string.h>
#include "stm32f10x.h"
#include "systick.h"
//#include "stm32f103_config.h"
#include "lf125k.h"
#include "usart.h"
#include "delay.h"
/* Types -----------------------*/
/* Constants -------------------*/
/* Define ----------------------*/
#define REM GPIO_read_lf125k_data()
#define CLK(n) GPIO_set_lf125k_clk(n)
#define	M_OK 0
#define LF_DBR_BASE 
#define LF_1us 72ul
#define LF_720us (720000-LF_1us*720)
#define LF_650us (720000-LF_1us*650)
#define LF_Star (720000-LF_1us*5000)//5ms
#define LF_T557 (720000-LF_1us*350)//350us
#define LF_380us (720000-LF_1us*380)
#define LF_350us (720000-LF_1us*384)//8*48
//#define LF_350us (720000-LF_1us*350)
#define LF_250us (720000-LF_1us*250)
#define LF_200us (720000-LF_1us*200)
#define LF_100us (720000-LF_1us*128)//8*18
//#define LF_100us (720000-LF_1us*100)
#define LF_5ms (720000-LF_1us*5000)//5ms

//低频
#define GPIO_LF125K_DATA GPIOE
#define GPIO_LF125K_DATA_PIN GPIO_Pin_1
#define GPIO_LF125K_CLK GPIOE
#define GPIO_LF125K_CLK_PIN GPIO_Pin_2

typedef enum {
	LF125_PAGE0_BANK0=0x00,
	LF125_PAGE0_BANK1=0x01,
	LF125_PAGE0_BANK2=0x02,
	LF125_PAGE0_BANK3=0x03,
	LF125_PAGE0_BANK4=0x04,
	LF125_PAGE0_BANK5=0x05,
	LF125_PAGE0_BANK6=0x06,
	LF125_PAGE0_BANK7=0x07,
}LF125_PAGE0_BANKx;
/* Variables -------------------*/
/* Functions prototypes --------*/
/* Functions -------------------*/
void GPIO_LF125K_configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_LF125K_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_LF125K_CLK, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_LF125K_DATA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIO_LF125K_DATA, &GPIO_InitStructure);
}
/*
 *@brief	.
 *@param	.
 *@retval	.
*/	
void GPIO_set_lf125k_clk (uint8_t status)
{
	if (status)
		GPIO_WriteBit(GPIO_LF125K_CLK, GPIO_LF125K_CLK_PIN,Bit_SET);
	else
		GPIO_WriteBit(GPIO_LF125K_CLK, GPIO_LF125K_CLK_PIN,Bit_RESET);
}
/*
 *@brief	.
 *@param	.
 *@retval	.
*/	
uint8_t GPIO_read_lf125k_data (void)
{
	return GPIO_ReadInputDataBit(GPIO_LF125K_DATA, GPIO_LF125K_DATA_PIN);
}

uint8_t volue;//volue 为曼侧斯特码译码时的临时运算缓存
uint8_t tap;//tap 为曼侧斯特码发射时的临时运算缓存
uint8_t data_tap[6];//等待发射的数据
void _nop_(void)
{
	uint32_t i;
	i=1;
	while(i--);
}
//打开天线
void LF125K_open_RF(void)
{
	CLK(1);
}
void LF125K_close_RF(void)//
{
	CLK(0);
}
void LF125K_init(void)//
{
	GPIO_LF125K_configuration();
	LF125K_open_RF();
//	DBG_B_INFO("LF125K初始化成功");
}
/*
 *@brief	Manchester process.曼侧斯特
 *@param	.
 *@retval	0:success
 		1:failse.
*/	
int8_t REM_Processing(uint8_t *buf)
{
	uint32_t i,j,delay;
	long tus;
	for(i=0;i<50;i++){//find the first cync head
		volue=0;
		//while(REM==0);
		delay = 7200;
		while(REM == 0 && delay)//wait high level
			delay --;
		if (delay == 0) {
			return 1;//timeout
		}
//		SYSTICK_ClearSystickVal();
		while(REM == 1){
            delay_us(720);
			//if(SYSTICK_GetVal() > LF_720us){
				
			//}else{
				i = 50;
				break;
			//}
		}
   	}
	delay = 7200;
	while(REM == 1 && delay)//wait low level
		delay --;
	if (delay == 0) {
		return 1;
	}
	delay = 72000;
	while(REM == 0 && delay)//wait high level
		delay --;
	if (delay == 0) {
		return 1;
	}
	SYSTICK_ClearSystickVal();
//	tus = LF_650us;
 //   delay_us(650);
    tus = 650;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
//			while(SYSTICK_GetVal() > tus);
            delay_us(tus);
			if(REM==1)
			{
				delay = 7200;
				while(REM==1 && delay){
					delay --;
				}
				if(delay == 0)
					return 1;
				volue>>=1;
				volue|=0x80;
//				tus = LF_T557;
                tus = 350;
				SYSTICK_ClearSystickVal();
			}
			else
			{
				delay = 7200;
				while(REM==0 && delay){
					delay --;
				}
				if(delay == 0)
					return 1;
				volue>>=1;
				volue|=0x00;
//				tus = LF_T557;
                tus = 350 ;
				SYSTICK_ClearSystickVal();
			}
		}
		buf[i]=volue;
		volue=0x00;
	}	   
	return 0;
}
void Data_Processing(void)
{
	if(tap==0x01){
		SYSTICK_ClearSystickVal();
//		while(SYSTICK_GetVal()>LF_350us);
        delay_us(384);
		CLK(0);
	 }else{
		SYSTICK_ClearSystickVal();
//		while(SYSTICK_GetVal()>LF_100us);
         delay_us(128);
		CLK(0);
	 }
	SYSTICK_ClearSystickVal();
//	while(SYSTICK_GetVal()>LF_250us);
     delay_us(250);
	CLK(1);
}
void write_od(uint8_t voled)
{
	SYSTICK_ClearSystickVal();
//	while(SYSTICK_GetVal()>LF_Star);
    delay_ms(5);
	CLK(0);//
	SYSTICK_ClearSystickVal();
//	while(SYSTICK_GetVal()>LF_350us);
    delay_us(384);
	CLK(1);
    
	tap=voled;
	tap&=0x10;
	if(tap==0x10){
		tap=0x01;
		Data_Processing();//
	}else{
		tap=0x00;
		Data_Processing();//
	}
	tap=voled;
	tap&=0x01;
	Data_Processing();//	
}
void  write_lock(uint8_t voled)//
{
	tap=voled;
	tap&=0x01;
	Data_Processing();
}
void write_data()
{
	uint8_t i,j,voled_1;
	for(j=0;j<4;j++)
	{
		voled_1=data_tap[j];
		for(i=0;i<8;i++)
		{ 
			tap=voled_1;
			tap&=0x01;
			Data_Processing();//
			voled_1>>=1;
		}
	}
}
void write_add(uint8_t voled)
{
	tap=voled;
	tap&=0x04;
	if(tap==0x04)
	{
		tap=0x01;
		Data_Processing();//
	}
	else 
	{
		tap=0x00;
		Data_Processing();//
	}
	tap=voled;
	tap&=0x02;
	if(tap==0x02)
	{
		tap=0x01;
		Data_Processing();//
	}
	else 
	{
		tap=0x00;
		Data_Processing();//
	}
	tap=voled;
	tap&=0x01;
	Data_Processing();//	
}
int8_t LF125K_read_1_page(uint8_t* buf)
{
	uint8_t buf1[10];
	write_od(0x11);
//	DBG_B_INFO("test time");
	delay_10ms(1);
//	DBG_B_INFO("test time2");
	if(REM_Processing(buf))
	return 1;
	delay_10ms(5);
	write_od(0x11);
	delay_10ms(1);
	if(REM_Processing(buf1))
		return 1;
	if (memcmp(buf1,buf,8) != 0){
		return 1;
	}
	return 0;
}
void LF125K_demo(void)
{
	int ret;
//	int ret1;
    uint8_t buf[8];
//	uint8_t buf1[8];
	memset(buf, 0, sizeof(buf));
	printf("低频卡测试\r\n");
	ret=LF125K_read_1_page(buf);
//	printf("低频卡测试\r\n");
	//memset(buf1,0,sizeof(buf1));
	//ret1=LF125K_read_0_page(LF125_PAGE0_BANK4,buf1);
//	printf("低频卡测试\r\n");
	if(ret == 0)
	{
	 printf("低频卡获取成功！");
	 printf("id card:%X %X %X %X %X %X %X %X\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	}
	else
	{
	 printf("低频卡获取失败！");
	}
	
}

int8_t LF125K_read_0_page(uint8_t adr,uint8_t* buf)
{
	uint8_t  buf1[10];
	adr &= 0x7;
	write_od(0x10);
	//write_lock(RI_Buffer[3]);
	write_lock(0x00);
	write_add(adr);
	delay_10ms(2);
	REM_Processing(buf1);
	delay_10ms(2);
	write_od(0x10);
	//write_lock(RI_Buffer[3]);
	write_lock(0x00);
	write_add(adr);
	delay_10ms(2);
	REM_Processing(buf);
	if(memcmp(buf1,buf,4)!=0)	
   		return M_OK+1;
	return M_OK;
}
int8_t LF125K_write_0_page(unsigned char adr,unsigned char *buf) 
{
	uint8_t  buf1[10];
	uint8_t lock=0;
	if((adr&0x80) == 0x80)
		lock = 1;
	adr &=0x07;
	if(adr==0x00)//块0 配置区 防止误操作
		return M_OK+1;
	
	write_od(0x10);
	//write_lock(RI_Buffer[7]);
	write_lock(lock);//写1固化
	data_tap[0]=buf[0];
	data_tap[1]=buf[1];
	data_tap[2]=buf[2];
	data_tap[3]=buf[3];
	write_data();
	write_add(adr);
	/*delay_10ms(2);	
	REM_Processing(buf1);*/
	delay_10ms(2);
	
	write_od(0x10);
	write_lock(0x00);
	write_add(adr);
	delay_10ms(2);
	REM_Processing(buf1);
	if(memcmp(buf1,buf,4)!=0)	
   		return M_OK+1;
	return M_OK;
}


/*
 *@brief	固化页0某块数据，固化后不能修改.
 *@param	.
 *@retval	.
*/	
int8_t LF125K_write_0_page_die(char adr,char* buf) 
{
	return 1;
}


/*
 *@brief	带密码写块数据.
 *@param	.
 *@retval	.
*/
int8_t LF125K_write_0_page_protect(uint8_t adr,uint8_t *buf,uint8_t *pwd)
{
	uint8_t lock=0;
	if ((adr&0x80) == 0x80) {
		lock = 1;
	}
	write_od(0x10);
	data_tap[0]=pwd[0];
	data_tap[1]=pwd[1];
	data_tap[2]=pwd[2];
	data_tap[3]=pwd[3];
	write_data();
	write_lock(lock);
	
	data_tap[0]=buf[0];
	data_tap[1]=buf[1];
	data_tap[2]=buf[2];
	data_tap[3]=buf[3];
	write_data();
	write_add(adr);
	delay_10ms(1);
	if(REM_Processing(buf))
		return 1;	
	return 0;
}


/*
 *@brief	带密码读块数据.
 *@param	.
 *@retval	.
*/	
int8_t LF125K_read_0_page_protect(uint8_t adr,uint8_t *buf,uint8_t *pwd)
{
/*	write_od(0x10);
	data_tap[0]=pwd[0];
	data_tap[1]=pwd[1];
	data_tap[2]=pwd[2];
	data_tap[3]=pwd[3];
	write_data();
	delay_10ms(1);
*/
	write_od(0x10);
	data_tap[0]=pwd[0];
	data_tap[1]=pwd[1];
	data_tap[2]=pwd[2];
	data_tap[3]=pwd[3];
	write_data();
	write_lock(0x00);
	write_add(adr);
	delay_10ms(1);
	if(REM_Processing(buf))
		return 1;	
	return 0;
}
//无密码
int8_t Card_initialization(void)
{
	uint8_t  buf[10];
	write_od(0x10);
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;//0x28;
	data_tap[2]=0x01;
	data_tap[3]=0x17;
	write_data();
	write_add(0x00);
	delay_10ms(100);
	REM_Processing(buf);
	return (LF125K_read_1_page(buf));
}
void LF125K_card_init1(void)
{
	write_od(0x10);
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;//数据率
	data_tap[2]=0x01;
	data_tap[3]=0x17;
	write_data();
	write_add(0x00);
}
/*
 *@brief	卡片初始化.
 *@param	.
 *@retval	0:success
 		1:false.
*/	
int8_t LF125K_card_init(void)
{
	uint8_t  buf[10];
	write_od(0x10);
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;//0x28;
	data_tap[2]=0x01;
	data_tap[3]=0x17;
	write_data();
	write_add(0x00);
	delay_10ms(100);
	REM_Processing(buf);
	return (LF125K_read_1_page(buf));
}
/*
 *@brief	加密卡片.
 *@param	.
 *@retval	.
*/	
void LF125K_set_password (void)
{
	write_od(0x10);
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;
	data_tap[2]=0x41;
	data_tap[3]=0x1f;
	write_data();
	write_add(0x00);
}
/*
 *@brief	.
 *@param	.
 *@retval	.
*/	
int8_t LF125K_set_pwd (uint8_t *pwd)
{
	if(LF125K_write_0_page(7,pwd) != 0)
		return 1;
	write_od(0x10);
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;
	data_tap[2]=0x41;
	data_tap[3]=0x1f;
	write_data();
	write_add(0x00);
	return 0;
}
/*
 *@brief	清除卡片密码.
 *@param	.
 *@retval	.
*/	
int8_t LF125K_clear_pwd (uint8_t *pwd)
{
	uint8_t buf[10];
	write_od(0x10);
	data_tap[0]=pwd[0];
	data_tap[1]=pwd[1];
	data_tap[2]=pwd[2];
	data_tap[3]=pwd[3];
	write_data();
	write_lock(0x00);
	data_tap[0]=0x00;
	data_tap[1]=0x28;
	data_tap[2]=0x01;
	data_tap[3]=0x17;
	write_data();
	write_add(0);
	delay_10ms(1);
	if(REM_Processing(buf))
		return 1;	
	return 0;
}
/*
 *@brief	.
 *@param	.
 *@retval	.
*/	
int8_t LF125K_get_cardid (uint8_t *buf)
{
	uint8_t tmp_buf[10];
	if(LF125K_read_1_page(tmp_buf)){
		buf[0] = tmp_buf[4];
		buf[1] = tmp_buf[5];
		buf[2] = tmp_buf[6];
		buf[3] = tmp_buf[7];
	}
	return 0;
}
