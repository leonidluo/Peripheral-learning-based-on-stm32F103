#ifndef __LF125K_H
#define __LF125K_H
/* Includes --------------------*/
#include "stm32f10x.h"

/* Types -----------------------*/
/* Constants -------------------*/
/* Define ----------------------*/
/* Variables -------------------*/
/* Functions prototypes --------*/
extern void LF125K_init(void);
extern void LF125K_demo(void);

//adr(0~7), buf(return 4 bytes)
extern int8_t LF125K_read_0_page(uint8_t adr,uint8_t *buf);

//adr(0~7), buf(4 bytes)
extern int8_t LF125K_write_0_page(uint8_t adr,uint8_t *buf);

//return 8 bytes
extern int8_t LF125K_read_1_page(uint8_t *buf);

extern int8_t LF125K_card_init(void);
extern int8_t LF125K_set_pwd (uint8_t *pwd);
extern int8_t LF125K_clear_pwd (uint8_t *pwd);
extern int8_t LF125K_write_0_page_protect(uint8_t adr,uint8_t *buf,uint8_t *pwd);
extern int8_t LF125K_read_0_page_protect(uint8_t adr,uint8_t *buf,uint8_t *pwd);
#endif
