#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "spi.h"	 
#include "rc522.h"	 
 
/************************************************
 ALIENTEK战舰STM32开发板实验24
 SPI 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

 				 	

	
 int main(void)
 {	 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE; 
	 
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	 
	my_mem_init(SRAMIN);		//初始化内部内存池
	 
	uart_init(115200);	 	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口

	SPI2_Init();
 	MFRC522_Initializtion();
	while(1)
	{
		RC522_Read_Mess();
//			if(RC522_Read_Mess() == 0)
//			{
//				LED0 = 0 ;
//			}else LED0 = 1;
	}
}


