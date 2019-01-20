#include <reg52.h>			// 包含头文件
#include <intrins.h>

#define uchar unsigned char		// 以后unsigned char就可以用uchar代替
#define uint  unsigned int		// 以后unsigned int 就可以用uint 代替


sbit w1 = P2^4; 				// 数码管第1位的控制引脚
sbit w2 = P2^5;				// 数码管第2位的控制引脚
sbit w3 = P2^6;				// 数码管第3位的控制引脚
sbit w4 = P2^7;			 	// 数码管第4位的控制引脚
sbit DQ1 = P1^0;				// 第一个DS18B20传感器的引脚定义
sbit DQ2 = P2^1;				// 第二个DS18B20传感器的引脚定义
sbit LedLow  = P1^2;			// 温度过低指示灯
sbit LedHig  = P1^4;			// 温度过高指示灯
sbit Buzzer  = P2^3;			// 蜂鸣器引脚
sbit KeySet  = P3^2;			// 设置按键
sbit KeyDown = P3^3;			// 减按键
sbit KeyUp   = P3^4;			// 加按键

sbit cs = P1^2;
sbit sck = P1^1;
sbit so = P1^0;
uchar DQ_switch = 0;			//温度传感器选择  0：第一个  1：第二个

/*   数码管的显示值 0   1   2   3   4    5   6   7   8   9   -   */
uchar code Array1[]={ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40 };

/*                 0.   1.   2.   3.  4.   5.   6.  7.   8.  9.  */
uchar code Array2[]={ 0xBf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };

uint Buff[5];					// 显示缓冲区
uchar ShowID=1;				// 当前显示的是哪一个数码管
int AlarmLow=120;				// 默认报警的温度下限值是12度
int AlarmHig=320;				// 默认报警的温度上限值是32度
unsigned int byte,bytee,t;
/*********************************************************/
// 毫秒级的延时函数，time是要延时的毫秒数
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}
void Delay(unsigned int n)
 {
  unsigned int j;
  for(j=0;j<n;j++)
   {
    ;
   }
  }
/*********************************************************/
// 延时15微秒
/*********************************************************/
void Delay15us(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
void max6675_display(unsigned int x) 
{       
	unsigned int temp[5]={0,0,0,0,0}; 
	unsigned int n,m,t;
	m=(10237.5*x/4095)-245;
	temp[0]=m/10000; 
	n=m-temp[0]*10000; 
	temp[1]=n/1000; 
	n=n-temp[1]*1000; 
	temp[2]=n/100; 
	n=n-temp[2]*100; 
	temp[3]=n/10; 
	n=n-temp[3]*10; 
	temp[4]=n/1;
	t=temp[4];

	Buff[0] = 	Array1[temp[1]];
	Buff[1] = 	Array1[temp[2]];
	Buff[2] = 	Array1[temp[3]];
	Buff[3] = 	Array1[temp[4]];
}

unsigned int read_max6675(void) 
{    
	
	unsigned int i=0; 
	byte=0;
	cs= 0;                    //CS=0,P2.6输出低电平
	sck= 0;                    //SCK=0    
	Delay(10); 
	for(i=0;i<16;i++) 
	 { 
	 	byte<<=1;
	   Delay(10);
	   sck = 1;                 //SCK=1 
	    Delay(10);
	   if(so==1)
	   	byte+=1;
	   
//	   byte|=(1<<(15-i));
	   sck = 0;                //SCK=0  
	 }
	cs =1;;                    //CS=1 
	sck = 0;//SCK=0 
	bytee=byte;
	if(bytee&0x8000)byte=0xfe;
	else if(bytee&0x4)byte=0xff;
	else
	 {
	   byte=byte&0x7ff8;
	   byte=byte>>3;
	   //byte=byte>>2;
	  }
	return byte;
} 


/*********************************************************/
// 定时器初始化
/*********************************************************/
void TimerInit()
{
	TMOD = 0x01;						// 使用定时器0，工作方式1	 
	TH0  = 248;						// 给定时器0的TH0装初值
	TL0  = 48;						// 给定时器0的TL0装初值
	ET0  = 1;							// 定时器0中断使能
	EA   = 1;						// 打开总中断
	TR0	 = 1;							// 启动定时器0
}

/*********************************************************/
// 显示温度值
/*********************************************************/
void ShowTemp(int dat)
{
	if(dat<0)							// 负号
	{
		Buff[0]=Array1[10];
		dat=0-dat;
	}
	else								// 百位
	{
		Buff[0]=Array1[dat/1000];
	}
	
	Buff[1]=Array1[dat%1000/100];		// 十位
	Buff[2]=Array2[dat%100/10];			// 个位
	Buff[3]=Array1[dat%10];				// 小数后一位
}


/*********************************************************/
// 主函数
/*********************************************************/
void main()
{
	int temp;
	uchar i;
	
	TimerInit();						// 定时器0的初始化（数码管的动态扫描）
	
	Buff[0]=Array1[0];					// 刚上电显示4个0
	Buff[1]=Array1[0];
	Buff[2]=Array1[0];
	Buff[3]=Array1[0];
	


	while(1)
	{
		DelayMs(1000);
		temp=read_max6675();      //读取温度
		max6675_display(temp);    //显示温度值
//		ShowTemp(temp*2);					// 显示温度值
		

	}
}

/*********************************************************/
// 定时器0服务程序
/*********************************************************/
void Timer0(void) interrupt 1
{
	TH0  = 248;				// 给定时器0的TH0装初值
	TL0  = 48;				// 给定时器0的TL0装初值

	P0=0x00;					// 先关闭所有显示
	w1=1;
	w2=1;
	w3=1;
	w4=1;

	if(ShowID==1)  			// 判断是否显示第1位数码管
	{
		w1=0;		   		// 打开第1位数码管的控制开关
		P0=Buff[0];	   		// 第1位数码管显示内容	
	}
	
	if(ShowID==2)  			// 判断是否显示第2位数码管
	{
		w2=0;		   		// 打开第2位数码管的控制开关
		P0=Buff[1];	   		// 第2位数码管显示内容	
	}
	
	if(ShowID==3)  			// 判断是否显示第3位数码管
	{
		w3=0;		   		// 打开第3位数码管的控制开关
		P0=Buff[2];	   		// 第3位数码管显示内容	
	}
	
	if(ShowID==4)  			// 判断是否显示第4位数码管
	{
		w4=0;		   		// 打开第4位数码管的控制开关
		P0=Buff[3];	   		// 第4位数码管显示内容	
	}	
	
	ShowID++;	  			// 切换到下一个数码管的显示
	if(ShowID==5)
		ShowID=1;
}


