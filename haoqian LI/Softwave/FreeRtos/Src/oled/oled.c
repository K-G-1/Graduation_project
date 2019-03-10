#include "oled.h"
#include "codetable.h"
#include "image.h"
#include "i2c.h"

#define OLED_CMD  0x00	//д����
#define OLED_DATA 0x40	//д����

/*********************OLEDд����************************************/ 
void OLED_write(unsigned char IIC_Data,unsigned char IIC_Command)
{
	HAL_I2C_Mem_Write(&hi2c2, 0x78,IIC_Command , I2C_MEMADD_SIZE_8BIT,&IIC_Data,1, 0x10);
}
void OLED_WrDat(unsigned char IIC_Data)
{
//    IIC_ADD_write(0x78,0x40,IIC_Data);
    HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT,&IIC_Data,1, 0x10);
}
/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{ 
    HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT,&IIC_Command,1, 0x10);

}
/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);			//ȡ����λ����������λ�Ŀ�ʼһλ��һ
	OLED_WrCmd((x&0x0f)|0x01);				//ȡ����λ����������λ�����һλ��һ
} 
/*********************OLEDȫ��************************************/
void OLED_Fill(unsigned char bmp_dat) 		 //ȫ����ʾͬһ����
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_dat);
	}
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(0);
	}
}
/*********************OLED��ʼ��************************************/
void OLED_Init(void)
{

	
	OLED_WrCmd(0xAE); //display off
    OLED_WrCmd(0x20); //Set Memory Addressing Mode    
    OLED_WrCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WrCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WrCmd(0xc0); //Set COM Output Scan Direction
    OLED_WrCmd(0x00); //---set low column address
    OLED_WrCmd(0x10); //---set high column address
    OLED_WrCmd(0x40); //--set start line address
    OLED_WrCmd(0x81); //--set contrast control register
    OLED_WrCmd(0xff); //���ȵ��� 0x00~0xff
    OLED_WrCmd(0xa0); //--set segment re-map 0 to 127
    OLED_WrCmd(0xa6); //--set normal display
    OLED_WrCmd(0xa8); //--set multiplex ratio(1 to 64)
    OLED_WrCmd(0x3F); //
    OLED_WrCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WrCmd(0xd3); //-set display offset
    OLED_WrCmd(0x00); //-not offset
    OLED_WrCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_WrCmd(0xf0); //--set divide ratio
    OLED_WrCmd(0xd9); //--set pre-charge period
    OLED_WrCmd(0x22); //
    OLED_WrCmd(0xda); //--set com pins hardware configuration
    OLED_WrCmd(0x12);
    OLED_WrCmd(0xdb); //--set vcomh
    OLED_WrCmd(0x20); //0x20,0.77xVcc
    OLED_WrCmd(0x8d); //--set DC-DC enable
    OLED_WrCmd(0x14); //
    OLED_WrCmd(0xaf); //--turn on oled panel
	OLED_Fill(0x00); //��ʼ����
	Draw_BMP(0,0,127,64,(unsigned char *)gImage_2);
	OLED_Set_Pos(0,0);
} 

/***************������������ʾ6*8һ���׼ASCII�ַ���	��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}
void OLED_P6x8data(unsigned char x,unsigned char y,int ch)
{
	unsigned char i=0;
	uint16_t bai,shi,ge;
	if(ch>=00)
	{
		
		OLED_P6x8Str(x,y," ");
	}
	else
	{
		ch = -ch;
		OLED_P6x8Str(x,y,"-");
	}
	bai = ch/100%10 +16;
	shi= ch/10%10   +16;
	ge = ch%10      +16;
	
	if(x>126){x=0;y++;}
	OLED_Set_Pos(x+6,y);

	for(i=0;i<6;i++)
	OLED_WrDat(F6x8[bai][i]);
	OLED_Set_Pos(x+12,y);
	for(i=0;i<6;i++)
		OLED_WrDat(F6x8[shi][i]);
	OLED_Set_Pos(x+18,y);
	for(i=0;i<6;i++)
		OLED_WrDat(F6x8[ge][i]);
	

}
/*******************������������ʾ8*16һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[])				//һ��8ҳi������8�У�ÿ��������һ�����ֻ�һ������
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32; //���ַ�ͨ�����ֵļӼ�ת�����Լ������������λ����,�Ӷ���ʾ����.0��asiciiΪ48,48-32=16��������0������12λ
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);		     	//���д�ӡ���Է�����ÿ��8����
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}


void Draw_line(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char *data)
{
//	unsigned int j=0;
	unsigned char x;
	
	
	OLED_Set_Pos(x0,y0);
	for(x=x0;x<x1;x++)
	{      
		OLED_WrDat(*data);
		data++;
	}
	
}


/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[960])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	
	for(y=0;y<8;y++)
	{
		OLED_Set_Pos(0,y);
		for(x=0;x<128;x++)
	    {      

	    	OLED_WrDat(BMP[y*113 +x ]);
	    }
	}

}

/**********************************************************/

void oled_show_RC522_data(unsigned char  **data)
{
	OLED_P6x8Str(0,32,*data);

//	OLED_P6x8Str(36,5,(data+1));
//	OLED_P6x8Str(36,6,(data+2));
//	OLED_P6x8Str(36,7,(data+3));
	
}


