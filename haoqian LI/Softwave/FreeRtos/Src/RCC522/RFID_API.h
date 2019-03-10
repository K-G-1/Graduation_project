#ifndef __RFID_API_H
#define __RFID_API_H	

#include "main.h"
#include "stm32f1xx_hal.h"
#include "RC522.h"
#include "spi.h"
#include "stdint.h"
#include "string.h"
extern uint8_t CardID[4],Card_Type[2], DefaultKey[6];
extern unsigned char RC522_Wbuff[16],RC522_Rbuff[16];






uint8_t RC522_Read_Mess(void);
uint8_t RC522_Add_User(uint8_t num);
uint8_t  write_DoorKey(uint8_t *key,uint8_t* name) ;
uint8_t RC522_Add_ic_card(void);
void add_ic(void);
void del_ic(void);

uint8_t RC522_search(void);
uint16_t read_message(unsigned char * student_info);










#endif



