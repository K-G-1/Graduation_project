/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "main.h"
#include "gpio.h"
#include "RC522.h"
#include "oled.h"
#include "stdlib.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId LED_TASKHandle;
osThreadId RC522_TASKHandle;
osThreadId LCD_TASKHandle;
osThreadId DoorTskHandle;
osThreadId KeyTaskHandle;
osTimerId myTimer01Handle;
osSemaphoreId LEDSemaphroesHandle;
osSemaphoreId DoorSemHandle;

/* USER CODE BEGIN Variables */
osMailQId RC522_mailHandle;
osMailQId password_mailHandle;
osMailQId word_mailHandle;
uint8_t display_words[24]="                        ";
uint8_t display_password[16]="               ";
uint8_t check_card[14]= "             ";
uint8_t key_input[5]={"    "};
uint8_t password_status = 0;
uint8_t control_door_flag = 0;
uint8_t password[4] = "1234";
uint8_t ic_card_status;
uint8_t password_err_cnt = 0;
//{{"    have card"},{"not have card"}};
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartTask_LED(void const * argument);
void StartTask_RC522(void const * argument);
void StartTask_LCD(void const * argument);
void StartTask_Door(void const * argument);
void StartTask_Key(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void Door_control(uint8_t status)  ;
char keypress(void);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx );
void KeyOpen(uint8_t key_value);
int my_strcmp(char const *p,char const *q)  ;
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of LEDSemaphroes */
  osSemaphoreDef(LEDSemaphroes);
  LEDSemaphroesHandle = osSemaphoreCreate(osSemaphore(LEDSemaphroes), 1);

  /* definition and creation of DoorSem */
  osSemaphoreDef(DoorSem);
  DoorSemHandle = osSemaphoreCreate(osSemaphore(DoorSem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED_TASK */
  osThreadDef(LED_TASK, StartTask_LED, osPriorityBelowNormal, 0, 128);
  LED_TASKHandle = osThreadCreate(osThread(LED_TASK), NULL);

  /* definition and creation of RC522_TASK */
  osThreadDef(RC522_TASK, StartTask_RC522, osPriorityHigh, 0, 384);
  RC522_TASKHandle = osThreadCreate(osThread(RC522_TASK), NULL);

  /* definition and creation of LCD_TASK */
  osThreadDef(LCD_TASK, StartTask_LCD, osPriorityHigh, 0, 384);
  LCD_TASKHandle = osThreadCreate(osThread(LCD_TASK), NULL);

  /* definition and creation of DoorTsk */
  osThreadDef(DoorTsk, StartTask_Door, osPriorityLow, 0, 256);
  DoorTskHandle = osThreadCreate(osThread(DoorTsk), NULL);

  /* definition and creation of KeyTask */
  osThreadDef(KeyTask, StartTask_Key, osPriorityHigh, 0, 384);
  KeyTaskHandle = osThreadCreate(osThread(KeyTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	
	osMailQDef(RC522_Value, 14, uint8_t);
  RC522_mailHandle = osMailCreate(osMailQ(RC522_Value), NULL);   
	osMailQDef(password_mail, 16, uint8_t);
  password_mailHandle = osMailCreate(osMailQ(password_mail), NULL);   
	osMailQDef(word_mail, 24, uint8_t);
  word_mailHandle = osMailCreate(osMailQ(word_mail), NULL);   
//	osTimerStart(myTimer01Handle,1000);
	osSemaphoreWait(DoorSemHandle,0x10);
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* StartTask_LED function */
void StartTask_LED(void const * argument)
{
  /* USER CODE BEGIN StartTask_LED */
  /* Infinite loop */
  for(;;)
  {
		if(osSemaphoreWait(LEDSemaphroesHandle,0x10) == osOK)
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET);
		}
		else  
		{
			HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
		}
    		

		osDelay(1000);
  }
  /* USER CODE END StartTask_LED */
}

/* StartTask_RC522 function */
void StartTask_RC522(void const * argument)
{
  /* USER CODE BEGIN StartTask_RC522 */
	unsigned char student_info[32] ={0};
	char buf1[8]= {0};
	char buf[8]= "0000000";
  /* Infinite loop */
  for(;;)
  {
		if(I2C2->SR2 & 0x02)
		{
			HAL_I2C_DeInit(&hi2c2);
			HAL_I2C_Init(&hi2c2);
		}
			
		
		if(RC522_search()== 0)
		{
			
			

			read_message(student_info);
//			memset(buf,0,sizeof(buf));
//			sscanf(student_info,"%[^:]:%8s",buf1,buf);
//			OLED_P6x8Str(0,1,buf);
			memset(check_card,0,14);
			memcpy(check_card,"    have card",14);
			
			osSemaphoreRelease(LEDSemaphroesHandle);
			osSemaphoreRelease(DoorSemHandle);

		}
		else 
		{
			memset(check_card,0,14);
			memcpy(check_card,"not have card",14);
//			OLED_P6x8Str(0,1,"        ");
		}
		osMailPut(RC522_mailHandle,check_card);

    osDelay(1000);
  }
  /* USER CODE END StartTask_RC522 */
}

/* StartTask_LCD function */
void StartTask_LCD(void const * argument)
{
  /* USER CODE BEGIN StartTask_LCD */
	
	osEvent Rc522_mail;
	osEvent Password_mail;
	osEvent word_mail;
	uint8_t num = 0;
  /* Infinite loop */
  for(;;)
  {
		Rc522_mail = osMailGet(RC522_mailHandle,10);
    if(Rc522_mail.status == osEventMail)
    {
//			OLED_P6x8Str(0,2,check_card);
			oled_show_Str(0,2,check_card,13);
    }
		Password_mail = osMailGet(password_mailHandle,10);
		if(Password_mail.status == osEventMail)
    {
			oled_show_Str(0,4,display_password,15);
    }
		word_mail = osMailGet(word_mailHandle,10);
//		if(word_mail.status == osEventMail)
//    {
//			oled_show_Str(0,7,display_words,20);
//    }
		oled_show_Str(0,7,display_words,20);
		
		num++;
		OLED_P6x8data(64,0,num);
		

		osDelay(1000);
  }
	
  /* USER CODE END StartTask_LCD */
}

/* StartTask_Door function */
void StartTask_Door(void const * argument)
{
  /* USER CODE BEGIN StartTask_Door */
	uint8_t door_status = 0;
	uint32_t time_count = 0 ;
	uint32_t tickstart = 0;
	uint32_t time_pre = 0 ;
  /* Infinite loop */
  for(;;)
  {
		if(osSemaphoreWait(DoorSemHandle,0x10) == osOK && door_status == 0)
		{
			osThreadSuspend(RC522_TASKHandle);
			osThreadSuspend(LED_TASKHandle);
			door_status = 1;
			time_count = 0;
			
			Door_control(1);
			time_pre = HAL_GetTick();
		}
		else if(door_status ==1 && time_count > 5000) {
			door_status = 0;
			Door_control(0);
			osThreadResume(RC522_TASKHandle);
			osDelay(10);
		}
		else if(door_status == 1)
		{
			tickstart = HAL_GetTick();
			time_count += tickstart - time_pre;
			
			time_pre = tickstart;
		}
    osDelay(200);
  }
  /* USER CODE END StartTask_Door */
}

/* StartTask_Key function */
void StartTask_Key(void const * argument)
{
  /* USER CODE BEGIN StartTask_Key */
	uint8_t Key_temp;
	uint8_t dis_time_cnt= 0 ;
	uint8_t err_time_cnt= 0 ;
  /* Infinite loop */
  for(;;)
  {

		Key_temp = keypress();
		KeyOpen(Key_temp);
		
		if(Key_temp == 17)
		{
			dis_time_cnt++;
			
			if(dis_time_cnt >= 20)
			{
				dis_time_cnt = 20;
				memset(display_words ,' ',sizeof(display_words));
			}
		}
		else 
		{
			dis_time_cnt = 0;
		}
		
		
		if(password_err_cnt >=3)
		{
			memcpy(display_words ,"max err cnt         ",20);
			err_time_cnt++;
			
			if(err_time_cnt >= 80)
			{
				err_time_cnt = 0;
				password_err_cnt = 0;
			}
		}
		if(control_door_flag == 1&& password_err_cnt<3)
		{
			osSemaphoreRelease(DoorSemHandle);
			
		}
		osMailPut(password_mailHandle,display_password);
		osMailPut(word_mailHandle,display_words);
		control_door_flag = 0;
    osDelay(100);
  }
  /* USER CODE END StartTask_Key */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */

  /* USER CODE END Callback01 */
}

/* USER CODE BEGIN Application */
/*  
	status 控制门的方式
	0： 关门
  1：开门
*/
void Door_control(uint8_t status)    
{
	uint8_t cnt = 0;
	if(status == 0)
	{
		while(cnt < 100)
		{
			
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_0_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin|ULN2003_2_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_0_Pin|ULN2003_2_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin|ULN2003_0_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin|ULN2003_2_Pin|ULN2003_0_Pin,GPIO_PIN_RESET);
			osDelay(10);
			cnt ++;
		}
	}else if(status >0)
	{
		while(cnt < 100)
		{
			
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin|ULN2003_2_Pin|ULN2003_0_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_0_Pin|ULN2003_1_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_2_Pin|ULN2003_0_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_0_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ULN2003_0_GPIO_Port,ULN2003_1_Pin|ULN2003_2_Pin|ULN2003_3_Pin,GPIO_PIN_RESET);
			osDelay(10);
			cnt ++;
		}
	}
}

uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx )
{
	return ((uint16_t)GPIOx->IDR);
}
/***************************************************
*@version		:V1.0
*@CreatDate		:
*@Description	:矩阵键盘，默认支持连按，
*@Author		:K.G. 
****************************************************/
char keypress(void)
{
    static uint8_t KeyHold = 0;
    int KeyValue=17;  
    
    if((GPIO_ReadInputData(GPIOA)&0xFF)!=0x0F)   
    {   
        osDelay(2);  
        if((GPIO_ReadInputData(GPIOA)&0xFF)!=0x0F&&KeyHold ==0)   
        {   
            HAL_GPIO_WritePin(Key0_GPIO_Port,Key0_Pin,GPIO_PIN_SET);
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key1_Pin|Key2_Pin|Key3_Pin,GPIO_PIN_RESET);

            switch(GPIO_ReadInputData(GPIOA)&0xFF)  
            {   
                case 0x11:KeyValue=7;break;   

                case 0x21:KeyValue=4;break;   

                case 0x41:KeyValue=1;break;   

                case 0x81:KeyValue=11;break;   
            }   
            
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key1_Pin,GPIO_PIN_SET);
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key0_Pin|Key2_Pin|Key3_Pin,GPIO_PIN_RESET);

            switch(GPIO_ReadInputData(GPIOA)&0xff)  
            {   
             case 0x12:KeyValue=8;break;  

             case 0x22:KeyValue=5;break;   

             case 0x42:KeyValue=2;break;  

             case 0x82:KeyValue=0;break; 

            }  
            
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key2_Pin,GPIO_PIN_SET);
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key0_Pin|Key1_Pin|Key3_Pin,GPIO_PIN_RESET);

            switch(GPIO_ReadInputData(GPIOA)&0xff)  
            {   
                case 0x14:KeyValue=9;break;  

                case 0x24:KeyValue=6;break;  

                case 0x44:KeyValue=3;break;  

                case 0x84:KeyValue=10;break;  
            }   
            
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key3_Pin,GPIO_PIN_SET);
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key0_Pin|Key2_Pin|Key1_Pin,GPIO_PIN_RESET);
                            
            switch(GPIO_ReadInputData(GPIOA)&0xff)   
            {  
                case 0x18:KeyValue=12;break;  

                case 0x28:KeyValue=13;break;   

                case 0x48:KeyValue=14;break;  

                case 0x88:KeyValue=15;break;   
            }   
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key0_Pin|Key1_Pin|Key2_Pin|Key3_Pin,GPIO_PIN_SET);
						HAL_GPIO_WritePin(Key0_GPIO_Port,Key4_Pin|Key5_Pin|Key6_Pin|Key7_Pin,GPIO_PIN_RESET);
            if((GPIO_ReadInputData(GPIOA)&0xff)==0x0F&&KeyHold==1)    //按下就会中断程序
            {
                KeyValue = 17;
            }
               KeyHold = 1;
        }    
    }  
    else 
        KeyHold = 0;

    
    return KeyValue;
}


int my_strcmp(char const *p,char const *q)  
{  
//    assert(p);  
//    assert(q);  
    while(*p==*q)  
    {  
        if(*p=='\0')  
            return 0;  
        p++;  
        q++;  
    }  
    if(*p>*q)  
        return 1;  
    if(*p<*q)  
        return -1;  
    return -1;
}  


/***************************************************
*@version		:V1.0
*@CreatDate		:2018/5/5
*@Description	:按键开门，加上模式1下的按键切换功能
*@Author		:K.G. 
****************************************************/

void KeyOpen(uint8_t key_value)
{

    static uint8_t num = 0;
    uint8_t i = 0;
//    LCD_ShowxNum(0,130,key_value,2,12,0);
    if(key_value!= 17)
    {
        if(key_value<10&&num<4)                 //输入密码
        {
            key_input[num] = key_value+0x30;
            num++;
        }
        else if(key_value == 11&&num > 0&&num<5)                //删除密码                
        {
            num--;
            key_input[num]=' ';           
        }
        else if(key_value == 10)
        {   
            if(password_status == 0)
            {
                if(my_strcmp((const char *)password, (const char *)key_input) == 0)
                {
//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"open door      ");
											memcpy(display_words,"open door           ",20);
                    control_door_flag = 1;
                }
                else
                {
//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"password error ");
									password_err_cnt++;
									memcpy(display_words,"password error      ",20);
                }
//								OLED_P6x8Str(0,3,(uint8_t *)"password error ");
                num = 0;
                memset(key_input,0,sizeof(key_input));
            }
						//修改密码部分，需要自己设置 status  = 1
            else if(password_status == 1)
            {
                if(my_strcmp((const char *)password, (const char *)key_input) == 0)
                {
//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"modif password   ");
//                    LCD_ShowString(0,115,120,12,12,(uint8_t *)"input new password   ");
										memcpy(display_words,"input new password     ",20);
                    num=0;
                    memset(key_input,' ',4*sizeof(uint8_t));
                    password_status = 2;
                }
                else 
                {
//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"password error   ");
										memcpy(display_words,"password error         ",20);
                    num=0;
                    memset(key_input,' ',4*sizeof(uint8_t));
                }
            }
            else if(password_status == 2)
            {
                strcpy((char *)password, (char *)key_input);
//                LCD_ShowString(0,115,120,12,12,(uint8_t *)"modif   success!!!   ");
								memcpy(display_words,"modif   success!!!   ",20);
                
                password_status = 0;
                num=0;
                memset(key_input,' ',4*sizeof(uint8_t));
            }
            
            else if(ic_card_status ==1 )
            {
                if(my_strcmp((const char *)password, (const char *)key_input) == 0)
                {
                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"add ic card     ");
                    if(RC522_Add_ic_card() == MI_OK)
										{
											memcpy(display_words,"add ic card OK      ",20);
											ic_card_status  = 0;
											password_status = 0;
										}
										else 
										{

											memcpy(display_words,"add ic card fail    ",20);
										}
                    num=0;
                    memset(key_input,' ',4*sizeof(uint8_t));
//                    ic_card_status = 2;
                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"please enter angain");
                }
                else 
                {
//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"password error   ");
										memcpy(display_words,"password error      ",20);
                    num=0;
                    memset(key_input,' ',4*sizeof(uint8_t));
                }
            }
            else if(ic_card_status ==2 )
            {
                if(RC522_Read_Mess() == 0)
                {
                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"add ic card success ");
									memcpy(display_words,"add ic card OK      ",20);
                }
                else
                {
//                    ic_card_status = 1;
                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"add ic card faild");
									memcpy(display_words,"add ic card fail    ",20);
                }
								
								ic_card_status  = 0;
								password_status = 0;
            }
						else if(ic_card_status ==3 )
            {
									if(my_strcmp((const char *)password, (const char *)key_input) == 0)
									{
											LCD_ShowString(0,145,120,12,12,(uint8_t *)"Del ic card     ");
											if(RC522_del_ic_card() == MI_OK)
											{
												memcpy(display_words,"del ic card OK      ",20);
												ic_card_status  = 0;
												password_status = 0;
											}
											else 
											{

												memcpy(display_words,"del ic card fail    ",20);
											}
											num=0;
											memset(key_input,' ',4*sizeof(uint8_t));
											
											LCD_ShowString(0,145,120,12,12,(uint8_t *)"please enter angain");
									}
									else 
									{
	//                    LCD_ShowString(0,145,120,12,12,(uint8_t *)"password error   ");
											memcpy(display_words,"password error      ",20);
											num=0;
											memset(key_input,' ',4*sizeof(uint8_t));
									}
            }
        }

        else if(key_value == 13)                //切换模式，待修改
        {
						osThreadSuspend(RC522_TASKHandle);
            ic_card_status  = 1;
            password_status = 3;
            num=0;
//            LCD_ShowString(0,115,120,12,12,(uint8_t *)"add ic card,input  ");
					memcpy(display_words,"add ic card,input   ",20);
        }
        else if(key_value == 12)                //修改密码
        {
//            display_manual = 1;
            ic_card_status = 0;
            password_status = 1;
//            fingerprint_status = 0;
            num=0;
//            LCD_ShowString(0,115,120,12,12,(uint8_t *)"modif password,input");
					memcpy(display_words,"modif password,input",24);
        }
        else if(key_value == 14)                //修改密码
        {
					osThreadSuspend(RC522_TASKHandle);
					ic_card_status  = 3;
            password_status = 3;
            num=0;
					memcpy(display_words,"del ic card,input   ",20);
        }
				else if(key_value == 15)                //修改密码
        {
					osThreadResume(RC522_TASKHandle);

					osDelay(10);
        }
        else if(num >=4)
        {
            num = 4;
        }


    }

			if(num == 0)
			{
				memcpy(display_password,"                ",16);
			}
			else if(num ==1 )
			{
				memcpy(display_password,"*               ",16);
			}
			else if(num ==2 )
			{
				memcpy(display_password,"**              ",16);
			}
			else if(num ==3 )
			{
				memcpy(display_password,"***             ",16);
			}
			else if(num ==4 )
			{
				memcpy(display_password,"****            ",16);
			}


//    LCD_ShowString(60,130,24,12,12,key_input);
}



/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
