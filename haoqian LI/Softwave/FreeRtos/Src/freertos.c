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
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId LED_TASKHandle;
osThreadId RC522_TASKHandle;
osThreadId LCD_TASKHandle;
osThreadId DoorTskHandle;
osTimerId myTimer01Handle;
osSemaphoreId LEDSemaphroesHandle;
osSemaphoreId DoorSemHandle;

/* USER CODE BEGIN Variables */
osMailQId RC522_mailHandle;
uint8_t check_card[14] ;
//{{"    have card"},{"not have card"}};
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartTask_LED(void const * argument);
void StartTask_RC522(void const * argument);
void StartTask_LCD(void const * argument);
void StartTask_Door(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void Door_control(uint8_t status)  ;
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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED_TASK */
  osThreadDef(LED_TASK, StartTask_LED, osPriorityNormal, 0, 128);
  LED_TASKHandle = osThreadCreate(osThread(LED_TASK), NULL);

  /* definition and creation of RC522_TASK */
  osThreadDef(RC522_TASK, StartTask_RC522, osPriorityHigh, 0, 256);
  RC522_TASKHandle = osThreadCreate(osThread(RC522_TASK), NULL);

  /* definition and creation of LCD_TASK */
  osThreadDef(LCD_TASK, StartTask_LCD, osPriorityAboveNormal, 0, 256);
  LCD_TASKHandle = osThreadCreate(osThread(LCD_TASK), NULL);

  /* definition and creation of DoorTsk */
  osThreadDef(DoorTsk, StartTask_Door, osPriorityBelowNormal, 0, 128);
  DoorTskHandle = osThreadCreate(osThread(DoorTsk), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	
	osMailQDef(RC522_Value, 14, uint8_t);
  RC522_mailHandle = osMailCreate(osMailQ(RC522_Value), NULL);   
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
		if(RC522_search()== 0)
		{
			
			

			read_message(student_info);
			memset(buf,0,sizeof(buf));
			sscanf(student_info,"%[^:]:%8s",buf1,buf);
			OLED_P6x8Str(0,1,buf);
			memset(check_card,0,14);
			memcpy(check_card,"    have card",14);
			
			osSemaphoreRelease(LEDSemaphroesHandle);
			osSemaphoreRelease(DoorSemHandle);

		}
		else 
		{
			memset(check_card,0,14);
			memcpy(check_card,"not have card",14);
			OLED_P6x8Str(0,1,"        ");
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
	
	osEvent display_mail;
	uint8_t num = 0;
  /* Infinite loop */
  for(;;)
  {
		display_mail = osMailGet(RC522_mailHandle,10);
    if(display_mail.status == osEventMail)
    {
			OLED_P6x8Str(0,2,(display_mail.value.p));
    }

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
			door_status = 1;
			time_count = 0;
			
			Door_control(1);
			time_pre = HAL_GetTick();
		}
		else if(door_status ==1 && time_count > 10000) {
			door_status = 0;
			Door_control(0);
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
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
