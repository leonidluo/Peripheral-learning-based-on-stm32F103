#ifndef _ESP8266_H_
#define _ESP8266_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

extern unsigned char esp8266_buf[200];
extern const char *devSubtopic[] ;//mqtt订阅的主题
extern const char *devPubtopic ;//mqtt发布的主题

extern const char *devSubtopic_2[] ;//mqtt订阅的主题
extern const char *devPubtopic_2 ;//mqtt发布的主题
void ESP8266_Init(void);

void ESP8266_Clear(void);

_Bool ESP8266_WaitRecive(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

void esp8266_Subscribe(const char *topics[]);

void esp8266_Publish(const char *topic, const char *msg);

unsigned char * USER_GetJsonValue(unsigned char *cJson, char *Tag);

#endif
