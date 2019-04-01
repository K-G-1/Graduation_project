#include "RFID_API.h"
#include "stdlib.h"
#include "oled.h"
uint8_t CardID[4];   //存放M卡类别
uint8_t study_num[10]={":15082114"};   //学号
uint8_t name[6] = "顾毅  ";
uint8_t Card_Type[2];   //卡的类型
uint8_t key_func =3;			//3,无操作  ；0，运行控制功能；   1  运行读卡信息    ；2添加用户卡
uint8_t  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 					//RFID的读取数据的密码

uint8_t ic_card_key[4]={6,6,6,6};

uint8_t door_key[6] = {6,6,4,7,4,8};																			//自定义的密码  用于开门
uint8_t door_key1[6] = "123456";
uint8_t default_card_key[4]={1,2,3,4};
unsigned char RC522_Wbuff[16];
unsigned char RC522_Rbuff[16];

uint8_t RC522_search(void)
{

		static uint8_t status;
		static char *Str_temp;
		char *key_temp;
    status = PcdRequest(0x52,Card_Type);
    
		if(status == MI_OK)
    {
        
        status = PcdAnticoll(CardID);/*防冲撞*/
        status = PcdSelect(CardID);//选卡
        if(status == MI_OK)
        {

            status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
            memset(RC522_Rbuff,0,16);   //清空数组
            status = PcdRead(2,RC522_Rbuff);
            Str_temp = (char*)malloc(16* sizeof(char));
            sprintf(Str_temp,"%d%d%d%d",RC522_Rbuff[0],RC522_Rbuff[1],RC522_Rbuff[2],RC522_Rbuff[3]);  //显示卡号
            key_temp = (char*)malloc(16* sizeof(char));
            sprintf(key_temp,"%d%d%d%d",ic_card_key[0],ic_card_key[1],ic_card_key[2],ic_card_key[3]);
            if(strcmp(key_temp,Str_temp)==0)
            {
                status = MI_OK;
            }
            else
            {
                status = KEY_ERROR;
            }
						
            free(key_temp);//释放内存
            key_temp=0;			//指向空地址
            free(Str_temp);//释放内存
            Str_temp=0;			//指向空地址
        }
    }
		
    return status;
}
uint8_t RC522_Read_Mess(void)
{
	static uint8_t status;
	static char *Str_temp;
    char *key_temp;
	char *student_info;
    status = PcdRequest(0x52,Card_Type);			
    


    if(status == MI_OK)
    {
        
        status = PcdAnticoll(CardID);/*防冲撞*/
        status = PcdSelect(CardID);//选卡
        if(status == MI_OK)
        {

            status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
            memset(RC522_Rbuff,0,16);   //清空数组
            status = PcdRead(2,RC522_Rbuff);
            Str_temp = (char*)malloc(16* sizeof(char));
            sprintf(Str_temp,"%d%d%d%d",RC522_Rbuff[0],RC522_Rbuff[1],RC522_Rbuff[2],RC522_Rbuff[3]);  //显示卡号
            key_temp = (char*)malloc(16* sizeof(char));
            sprintf(key_temp,"%d%d%d%d",ic_card_key[0],ic_card_key[1],ic_card_key[2],ic_card_key[3]);
            if(strcmp(key_temp,Str_temp)==0)
            {
                status = MI_OK;
            }
            else
            {
                status = KEY_ERROR;
            }
						
						student_info = (char*)malloc(32* sizeof(char));
//						status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
//            status = PcdRead(1,student_info);
						free(student_info);//释放内存
            student_info=0;			//指向空地址
						
            free(key_temp);//释放内存
            key_temp=0;			//指向空地址
            free(Str_temp);//释放内存
            Str_temp=0;			//指向空地址
        }
    }

	
    return status;
}

//uint8_t RC522_Add_User(uint8_t num)
//{
//	char status;
//	uint8_t name[18] = "顾毅  ";
////	uint8_t Card_Type[4];
////	uint8_t Last_Key=0;

////	uint8_t* tips;

//	status = write_DoorKey(ic_card_key,&name[0]);	
//		
//	if(status == MI_OK)
//	{

//		OLED_P6x8Str(0,2,"write ok");
////		Show_Str(100,400,300,24,"用户添加成功",24,1);	
////		Show_Str(130,370,90,24,&name[0],24,1);   //只显示前3个字

//	}
//	else 
//	{

////		Show_Str(100,400,300,24,"用户添加失败",24,1);	
//	}
//	HAL_Delay(1000);
////	Show_Str(100,400,300,24,"               ",24,0);
////	Show_Str(130,370,90,24,"           ",24,0);   //只显示前3个字

//    return status;
//}

//uint8_t  write_DoorKey(uint8_t *key,uint8_t* name)  //写入”看门"密码
//{
//	char status = 0xff;
//	uint8_t i;
//	uint8_t Card_Type[4];
//	status = PcdRequest(0x52,Card_Type);			
//	status = PcdAnticoll(CardID);/*防冲撞*/
//	status = PcdSelect(CardID);//选卡
//	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
//	memset(RC522_Wbuff,0,16);
//	if(status == MI_OK)
//	{
//		status = PcdWrite(2,key);

//	}
//	status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
//	for(i=0;i<6;i++)          //6字节 姓名，0-5
//	{
//		RC522_Wbuff[i] = name[i];
//	}							
//	for(i = 0 ;i < 4;i++)   //4字节卡号  6 - 9
//	{
//		RC522_Wbuff[6+i] = study_num[i];
//	}
//	status = PcdWrite(1,RC522_Wbuff);
//	return status;
//}


uint8_t RC522_Add_ic_card(void)
{
    char status;
	uint8_t i = 0;
	
	uint8_t Card_Type[4];
	status = PcdRequest(0x52,Card_Type);			
	status = PcdAnticoll(CardID);/*防冲撞*/
	status = PcdSelect(CardID);//选卡
	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
	memset(RC522_Wbuff,0,16);
	if(status == MI_OK)
	{
        HAL_Delay(10);
		status = PcdWrite(2,ic_card_key);

	}

//  HAL_Delay(500);
//	status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
//		
//	for(i=0;i<6;i++)          //6字节 姓名，0-5
//	{
//		RC522_Wbuff[i] = name[i];
//	}							
//	for(i = 0 ;i < 10;i++)   //4字节卡号  6 - 9
//	{
//		RC522_Wbuff[6+i] = study_num[i];
//	}
//	status = PcdWrite(1,RC522_Wbuff);
	return status;
}
/*****************************************************
@Version		:V1.0
@CreatDate	:
@Description	:
@Author		: K.G.
*****************************************************/
uint16_t read_message(unsigned char * student_info)
{
	static uint8_t status;

	status = PcdAuthState(PICC_AUTHENT1A,1,DefaultKey,CardID);
	status = PcdRead(1,student_info);


	return status;
}
/***************************************************
*@version		:V1.0
*@CreatDate		:2018/5/5
*@Description	:选卡之后向快2写入0000
*@Author		:K.G. 
****************************************************/
uint8_t RC522_del_ic_card(void)
{
    char status;
	uint8_t i=0;
	uint8_t Card_Type[4];
	status = PcdRequest(0x52,Card_Type);			
	status = PcdAnticoll(CardID);/*防冲撞*/
	status = PcdSelect(CardID);//选卡
	status = PcdAuthState(PICC_AUTHENT1A,2,DefaultKey,CardID);
	memset(RC522_Wbuff,0,16);
	if(status == MI_OK)
	{
        HAL_Delay(10);
        while(PcdWrite(2,RC522_Wbuff) !=0 && i <10)
        {
            i++;
        }
        if(i>+10)
        {
           return  MI_ERR;
        }

	}

	return status;
}
///***************************************************
//*@version		:V1.0
//*@CreatDate		:2018/5/5
//*@Description	:增加ic 卡
//*@Author		:K.G. 
//****************************************************/
//void add_ic(void)
//{
//    uint8_t status =1;
//    if(ic_card_status ==1 )
//    {
//        
//        if(strcmp((const char *)password, (const char *)key_input) == 0)
//        {
//            status = RC522_Add_ic_card();   
//            if(status == 0)
//            {
//                ic_card_status = 2;
//                LCD_ShowString(0,75,120,12,12,(uint8_t *)"             ");
//            }                
//            else 
//                LCD_ShowString(0,75,120,12,12,(uint8_t *)"please replay");  
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"please enter angain");
//        }
//        else 
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"password error   ");                       
//        }
//    }
//    else if(ic_card_status ==2 )
//    {
//        if(RC522_Read_Mess() == 0)
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"add ic card success ");
//            ic_card_status = 0;
//        }
//        else
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"add ic card faild");
//        }
//    }
//    memset(key_input,0,sizeof(key_input));
//}
///***************************************************
//*@version		:V1.0
//*@CreatDate		:2018/5/5
//*@Description	:删除ic卡
//*@Author		:K.G. 
//****************************************************/

//void del_ic(void)
//{
//    uint8_t status = 0;
//    if(ic_card_status ==11 )
//    {
//        if(strcmp((const char *)password, (const char *)key_input) == 0)
//        {
//            status = RC522_del_ic_card();  
//            
//            if(status == 0)
//            {
//                ic_card_status = 12;
//                LCD_ShowString(0,75,120,12,12,(uint8_t *)"             ");
//            }                
//            else 
//                LCD_ShowString(0,75,120,12,12,(uint8_t *)"please replay"); 
//            
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"please enter angain");
//        }
//        else 
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"password error   ");                       
//        }
//    }
//    else if(ic_card_status ==12 )
//    {
//        if(RC522_Read_Mess() != 0)
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"del ic card success ");
//            ic_card_status = 0;
//        }
//        else
//        {
//            LCD_ShowString(0,60,120,12,12,(uint8_t *)"del ic card faild");
//        }
//    }
//    memset(key_input,0,sizeof(key_input));
//}

