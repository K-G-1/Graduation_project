#ifndef _oled_H_
#define	_oled_H_

#include "stdint.h"
#define X_WIDTH 128





void OLED_Init(void);
void OLED_Fill(unsigned char bmp_dat);

void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[])	;
void OLED_P6x8data(unsigned char x,unsigned char y,int ch);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);


void oled_dis_data(int P,int R,int Y,uint16_t H);
void oled_dis_str(void);
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[960]);
 void Draw_line(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char *data);

void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);


void oled_show_RC522_data(unsigned char  **data);
void oled_show_Str( uint8_t x,uint8_t y  ,unsigned char  *data,uint8_t num);


#endif


