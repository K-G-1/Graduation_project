#include "RFID_API.h"
#include "led.h"

u8 CardID[4];   //���M�����
u8 study_num[4]={15,0x08,21,14};   //ѧ��
u8 Card_Type[2];   //��������
u8 key_func =3;			//3,�޲���  ��0�����п��ƹ��ܣ�   1  ���ж�����Ϣ    ��2����û���
u8  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 					//RFID�Ķ�ȡ���ݵ�����

u8 ic_card_key[4]={6,6,6,6};
u8 door_key[6] = {6,6,4,7,4,8};																			//�Զ��������  ���ڿ���
u8 door_key1[6] = "123456";

unsigned char RC522_Wbuff[16];
unsigned char RC522_Rbuff[16];

u8 RC522_search(void)
{
    static u8 status;
    
    status = PcdRequest(0x52,Card_Type);
    
    if(status == MI_OK)
    {
        LCD_ShowString(0,115,120,12,12,"have card");
    }
    else 
    {
        LCD_ShowString(0,115,120,12,12,"no   card");
    }
    return status;
}
u8 RC522_Read_Mess(void)
{
	static u8 status;
	static char *Str_temp;
    char *key_temp;
    status = PcdRequest(0x52,Card_Type);			
    


    if(status == MI_OK)
    {
        
        status = PcdAnticoll(CardID);/*����ײ*/
        status = PcdSelect(CardID);//ѡ��
        if(status == MI_OK)
        {
            if(status == MI_OK)
            {
                GPIO_SetBits(GPIOC,GPIO_Pin_13);
            }
            else 
            {
                GPIO_ResetBits(GPIOC,GPIO_Pin_13);
            }
            status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
            memset(RC522_Rbuff,0,16);   //�������
//            status = PcdRead(2,RC522_Rbuff);
//            Str_temp = mymalloc(SRAMIN,16);
//            sprintf(Str_temp,"%d%d%d%d",RC522_Rbuff[0],RC522_Rbuff[1],RC522_Rbuff[2],RC522_Rbuff[3]);  //��ʾ����
////            LCD_ShowString(0,115,120,12,12,Str_temp);
//            key_temp = mymalloc(SRAMIN,16);
//            sprintf(key_temp,"%d%d%d%d",ic_card_key[0],ic_card_key[1],ic_card_key[2],ic_card_key[3]);
//            if(strcmp(key_temp,Str_temp)==0)
//            {
//                LCD_ShowString(60,115,120,12,12,"ok   ");
//                status = MI_OK;
//            }
//            else
//            {
//                LCD_ShowString(60,115,120,12,12,"faild");
//                status = KEY_ERROR;
//            }
//            myfree(SRAMIN,Str_temp);//�ͷ��ڴ�
//            Str_temp=0;			//ָ��յ�ַ
//            myfree(SRAMIN,key_temp);//�ͷ��ڴ�
//            key_temp=0;			//ָ��յ�ַ
        }
    }
    else{
        GPIO_ResetBits(GPIOC,GPIO_Pin_13);
//        LCD_Fill(0,115,120,135,WHITE);
    }
	
    return status;
}

u8 RC522_Add_User(u8 num)
{
	char status;
	u8 name[18] = "����  ";
//	u8 Card_Type[4];
//	u8 Last_Key=0;

//	u8* tips;

	status = write_DoorKey(door_key,&name[0]);	
		
	if(status == MI_OK)
	{


//		Show_Str(100,400,300,24,"�û���ӳɹ�",24,1);	
//		Show_Str(130,370,90,24,&name[0],24,1);   //ֻ��ʾǰ3����

	}
	else 
	{
//		Show_Str(100,400,300,24,"�û����ʧ��",24,1);	
	}
	delay_ms(1000);
//	Show_Str(100,400,300,24,"               ",24,0);
//	Show_Str(130,370,90,24,"           ",24,0);   //ֻ��ʾǰ3����

    return status;
}

u8  write_DoorKey(u8 *key,u8* name)  //д�롱����"����
{
	char status;
	u8 i;
	u8 Card_Type[4];
	status = PcdRequest(0x52,Card_Type);			
	status = PcdAnticoll(CardID);/*����ײ*/
	status = PcdSelect(CardID);//ѡ��
	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
	memset(RC522_Wbuff,0,16);
	if(status == MI_OK)
	{
		status = PcdWrite(2,key);

	}
	status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
	for(i=0;i<6;i++)          //6�ֽ� ������0-5
	{
		RC522_Wbuff[i] = name[i];
	}							
	for(i = 0 ;i < 4;i++)   //4�ֽڿ���  6 - 9
	{
		RC522_Wbuff[6+i] = study_num[i];
	}
	status = PcdWrite(1,RC522_Wbuff);
	return status;
}


//u8 RC522_Add_ic_card(void)
//{
//    char status;

//	u8 Card_Type[4];
//	status = PcdRequest(0x52,Card_Type);			
//	status = PcdAnticoll(CardID);/*����ײ*/
//	status = PcdSelect(CardID);//ѡ��
//	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
//	memset(RC522_Wbuff,0,16);
//	if(status == MI_OK)
//	{
//        delay_ms(10);
//		status = PcdWrite(2,ic_card_key);
//        if(status == 0)
//        {
//            LCD_ShowString(0,145,120,12,12,"write success");
//        }else
//            LCD_ShowString(0,145,120,12,12,"write faild  ");
//	}
//    else 
//        LCD_ShowString(0,145,120,12,12,"no card  ");
//    delay_ms(500);
////	status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
////	for(i=0;i<6;i++)          //6�ֽ� ������0-5
////	{
////		RC522_Wbuff[i] = name[i];
////	}							
////	for(i = 0 ;i < 4;i++)   //4�ֽڿ���  6 - 9
////	{
////		RC522_Wbuff[6+i] = study_num[i];
////	}
////	status = PcdWrite(1,RC522_Wbuff);
//	return status;
//}
///***************************************************
//*@version		:V1.0
//*@CreatDate		:2018/5/5
//*@Description	:ѡ��֮�����2д��0000
//*@Author		:K.G. 
//****************************************************/
//u8 RC522_del_ic_card(void)
//{
//    char status;
//	u8 i=0;
//	u8 Card_Type[4];
//	status = PcdRequest(0x52,Card_Type);			
//	status = PcdAnticoll(CardID);/*����ײ*/
//	status = PcdSelect(CardID);//ѡ��
//	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
//	memset(RC522_Wbuff,0,16);
//	if(status == MI_OK)
//	{
//        delay_ms(10);
//        while(PcdWrite(2,RC522_Wbuff) !=0 && i <10)
//        {
//            i++;
//        }
//        if(i>+10)
//        {
//            LCD_ShowString(0,145,120,12,12,"write faild  ");
//           return  MI_ERR;
//        }
//        if(status == 0)
//        {
//            LCD_ShowString(0,145,120,12,12,"write success");
//        }else
//            LCD_ShowString(0,145,120,12,12,"write faild  ");
//	}
//    else 
//        LCD_ShowString(0,145,120,12,12,"no card  ");
//    delay_ms(500);

//	return status;
//}
///***************************************************
//*@version		:V1.0
//*@CreatDate		:2018/5/5
//*@Description	:����ic ��
//*@Author		:K.G. 
//****************************************************/
//void add_ic(void)
//{
//    u8 status =1;
//    if(ic_card_status ==1 )
//    {
//        
//        if(strcmp((const char *)password, (const char *)key_input) == 0)
//        {
//            status = RC522_Add_ic_card();   
//            if(status == 0)
//            {
//                ic_card_status = 2;
//                LCD_ShowString(0,75,120,12,12,(u8 *)"             ");
//            }                
//            else 
//                LCD_ShowString(0,75,120,12,12,(u8 *)"please replay");  
//            LCD_ShowString(0,60,120,12,12,(u8 *)"please enter angain");
//        }
//        else 
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"password error   ");                       
//        }
//    }
//    else if(ic_card_status ==2 )
//    {
//        if(RC522_Read_Mess() == 0)
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"add ic card success ");
//            ic_card_status = 0;
//        }
//        else
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"add ic card faild");
//        }
//    }
//    memset(key_input,0,sizeof(key_input));
//}
///***************************************************
//*@version		:V1.0
//*@CreatDate		:2018/5/5
//*@Description	:ɾ��ic��
//*@Author		:K.G. 
//****************************************************/

//void del_ic(void)
//{
//    u8 status = 0;
//    if(ic_card_status ==11 )
//    {
//        if(strcmp((const char *)password, (const char *)key_input) == 0)
//        {
//            status = RC522_del_ic_card();  
//            
//            if(status == 0)
//            {
//                ic_card_status = 12;
//                LCD_ShowString(0,75,120,12,12,(u8 *)"             ");
//            }                
//            else 
//                LCD_ShowString(0,75,120,12,12,(u8 *)"please replay"); 
//            
//            LCD_ShowString(0,60,120,12,12,(u8 *)"please enter angain");
//        }
//        else 
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"password error   ");                       
//        }
//    }
//    else if(ic_card_status ==12 )
//    {
//        if(RC522_Read_Mess() != 0)
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"del ic card success ");
//            ic_card_status = 0;
//        }
//        else
//        {
//            LCD_ShowString(0,60,120,12,12,(u8 *)"del ic card faild");
//        }
//    }
//    memset(key_input,0,sizeof(key_input));
//}

