#ifndef _FUN_H
#define _FUN_H

extern int status;
extern int tempr; 
extern int hum;
extern const char * id;
extern int  set_th;
void Reset(void);
void Pause(void);
void Running(void);
void TimeYearSet(char *dataPtr);
void TimeDaySet(char *dataPtr);

#endif
