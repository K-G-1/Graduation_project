#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "spi.h"	 
#include "rc522.h"	 
 
/************************************************
 ALIENTEKս��STM32������ʵ��24
 SPI ʵ��   
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

 				 	

	
 int main(void)
 {	 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE; 
	 
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	 
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	 
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�

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


