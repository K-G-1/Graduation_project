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

uchar DQ_switch = 0;			//温度传感器选择  0：第一个  1：第二个

/*   数码管的显示值 0   1   2   3   4    5   6   7   8   9   -   */
uchar code Array1[]={ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40 };

/*                 0.   1.   2.   3.  4.   5.   6.  7.   8.  9.  */
uchar code Array2[]={ 0xBf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };

uchar Buff[4];					// 显示缓冲区
uchar ShowID=1;				// 当前显示的是哪一个数码管
int AlarmLow=120;				// 默认报警的温度下限值是12度
int AlarmHig=320;				// 默认报警的温度上限值是32度

/*********************************************************/
// 毫秒级的延时函数，time是要延时的毫秒数
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
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

/*********************************************************/
// 复位DS18B20（初始化）	DQ1：第一个温度传感器
/*********************************************************/
void DQ1_ReSet(void)
{
	uchar i;
	DQ1=0;
	i=240;
	while(--i);
	DQ1=1;
	i=30;
	while(--i);
	while(~DQ1);
	i=4;
	while(--i);
}

/*********************************************************/
// 向DS18B20写入一个字节	DQ1：第一个温度传感器
/*********************************************************/
void DQ1_WriteByte(uchar dat)
{
	uchar j;
	uchar btmp;
	
	for(j=0;j<8;j++)
	{
		btmp=0x01;
		btmp=btmp<<j;
		btmp=btmp&dat;
		
		if(btmp>0)		// 写1
		{
			DQ1=0;
			Delay15us();
			DQ1=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// 写0
		{
			DQ1=0;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
			DQ1=1;
			Delay15us();
		}
	}
}

/*********************************************************/
// 读取DQ1温度值		DQ1：第一个温度传感器
/*********************************************************/
int DQ1_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DQ1_ReSet();					// 产生复位脉
	DQ1_WriteByte(0xcc);			// 忽略ROM指令
	DQ1_WriteByte(0x44);			// 启动温度转换指令
	DQ1_ReSet();					// 产生复位脉
	DQ1_WriteByte(0xcc);			// 忽略ROM指令
	DQ1_WriteByte(0xbe);			// 读取温度指令

	for(j=0;j<16;j++)					// 读取温度数量
	{						
		DQ1=0;
		_nop_();
		_nop_();
		DQ1=1;	
		Delay15us();
		b=DQ1;
		Delay15us();
		Delay15us();
		Delay15us();
		b=b<<j;
		temp=temp|b;
	}
	temp=temp*0.0625*10;				// 合成温度值并放大10倍					
	return (temp);						// 返回检测到的温度值
}


/*********************************************************/
// 复位DS18B20（初始化）	  DQ2：第二个温度传感器
/*********************************************************/
void DQ2_ReSet(void)
{
	uchar i;
	DQ2=0;
	i=240;
	while(--i);
	DQ2=1;
	i=30;
	while(--i);
	while(~DQ2);
	i=4;
	while(--i);
}

/*********************************************************/
// 向DS18B20写入一个字节	 DQ2：第二个温度传感器
/*********************************************************/
void DQ2_WriteByte(uchar dat)
{
	uchar j;
	uchar btmp;
	
	for(j=0;j<8;j++)
	{
		btmp=0x01;
		btmp=btmp<<j;
		btmp=btmp&dat;
		
		if(btmp>0)		// 写1
		{
			DQ2=0;
			Delay15us();
			DQ2=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// 写0
		{
			DQ2=0;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
			DQ2=1;
			Delay15us();
		}
	}
}

/*********************************************************/
// 读取DQ2温度值		DQ2：第二个温度传感器
/*********************************************************/
int DQ2_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DQ2_ReSet();					// 产生复位脉
	DQ2_WriteByte(0xcc);			// 忽略ROM指令
	DQ2_WriteByte(0x44);			// 启动温度转换指令
	DQ2_ReSet();					// 产生复位脉
	DQ2_WriteByte(0xcc);			// 忽略ROM指令
	DQ2_WriteByte(0xbe);			// 读取温度指令

	for(j=0;j<16;j++)					// 读取温度数量
	{						
		DQ2=0;
		_nop_();
		_nop_();
		DQ2=1;	
		Delay15us();
		b=DQ2;
		Delay15us();
		Delay15us();
		Delay15us();
		b=b<<j;
		temp=temp|b;
	}
	temp=temp*0.0625*10;				// 合成温度值并放大10倍					
	return (temp);						// 返回检测到的温度值
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
// 报警判断
/*********************************************************/
void AlarmJudge(int dat)
{
	if(dat<AlarmLow)						// 判断温度是否过低
	{
		LedLow=0;
		LedHig=1;
		Buzzer=0;
	}
	else if(dat>AlarmHig)					// 判断温度是否过高
	{
		LedLow=1;
		LedHig=0;
		Buzzer=0;
	}
	else									// 温度正常
	{
		LedLow=1;
		LedHig=1;
		Buzzer=1;
	}
}

/*********************************************************/
// 按键扫描
/*********************************************************/
void KeyScanf()
{
	static uchar Key_set_num = 0;
	uchar Break_flag = 0;
	if(KeySet==0)							// 如果设置按键被按下
	{
		DelayMs(10);						// 延时去抖
		if(KeySet == 0)
		{
			while(!KeySet);					// 等待按键释放
			Key_set_num ++;					//功能键计数
				
		}
		
	}
	if(Key_set_num ==0)
	{
		 DQ_switch = 0;			   			//选择第一个温度传感器
	}
	else if(Key_set_num == 1)
	{
		DQ_switch = 1;			   			//选择第二个温度传感器
	}
	else if(Key_set_num == 2)
	{
		/* 设置温度下限 */
		LedLow=0;							// 点亮绿色灯（代表当前正在设置温度下限）
		LedHig=1;							// 熄灭红色灯
		Buzzer=1;							// 关闭蜂鸣器
		ShowTemp(AlarmLow);					// 显示温度下限值
		DelayMs(10);						// 延时去抖
		while(!KeySet);						// 等待按键释放
		DelayMs(10);						// 延时去抖
		
		while(1)
		{
			if(KeyDown==0)					// 如果“减”按键被按下
			{
				if(AlarmLow>-550)			// 判断当前温度下限是否大于-55度
				{
					AlarmLow--;				// 温度下限值减去0.1度
					ShowTemp(AlarmLow);		// 刷新显示改变后的温度下限值
					DelayMs(200);			// 延时
				}
			}
			
			if(KeyUp==0)					// 如果“加”按键被按下					
			{
				if(AlarmLow<1250)			// 判断当前温度下限是否小于125度
				{
					AlarmLow++;				// 温度下限值加上0.1度
					ShowTemp(AlarmLow);		// 刷新显示改变后的温度下限值
					DelayMs(200);			// 延时
				}
			}
			
			if(KeySet==0)					// 如果“设置”按键被按下
			{
				Key_set_num = 3;
				break;
			}
		}
		/* 退出设置模式 */
		LedLow=1;							// 熄灭绿灯
		LedHig=1;							// 熄灭红灯
		DelayMs(10);						// 延时去抖
		while(!KeySet);						// 等待按键释放
		DelayMs(10);						// 延时去抖
	}
	
	if(Key_set_num == 3)
	{
		/* 设置温度上限 */
		LedLow=1;							// 熄灭绿色灯
		LedHig=0;							// 点亮红色灯（代表当前正在设置温度上限）
		ShowTemp(AlarmHig);					// 显示温度上限值
		DelayMs(10);						// 延时去抖
		while(!KeySet);						// 等待按键释放
		DelayMs(10);						// 延时去抖
		
		while(1)
		{
			if(KeyDown==0)					// 如果“减”按键被按下							
			{
				if(AlarmHig>-550)			// 判断当前温度上限是否大于-55度
				{
					AlarmHig--;				// 温度上限值减去0.1度
					ShowTemp(AlarmHig);		// 刷新显示改变后的温度上限值
					DelayMs(200);			// 延时
				}
			}
			
			if(KeyUp==0)					// 如果“加”按键被按下					
			{
				if(AlarmHig<1250)			// 判断当前温度上限是否小于125度
				{
					AlarmHig++;				// 温度上限值加上0.1度
					ShowTemp(AlarmHig);		// 刷新显示改变后的温度上限值
					DelayMs(200);
				}
			}
			
			if(KeySet==0)					// 如果“设置”按键被按下
			{
				Key_set_num = 0;
				DQ_switch = 0;
				break;						// 准备退出设置模式
			}
		}
		/* 退出设置模式 */
		LedLow=1;							// 熄灭绿灯
		LedHig=1;							// 熄灭红灯
		DelayMs(10);						// 延时去抖
		while(!KeySet);						// 等待按键释放
		DelayMs(10);						// 延时去抖
	}
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
	
	for(i=0;i<8;i++)  	// 传感器刚上电读出的温度不稳定，因此先进行几次读取并丢弃
	{
		DQ1_ReadTemp();
		DQ2_ReadTemp();
		DelayMs(120);
	}

	while(1)
	{
		EA=0;							// 屏蔽中断
		if(DQ_switch == 0 )
			temp=DQ1_ReadTemp();		// 读取第一个DS18B20温度值
		else 
			temp=DQ2_ReadTemp();		// 读取第二个DS18B20温度值
		EA=1;							// 恢复中断
		
		ShowTemp(temp);					// 显示温度值
		
		AlarmJudge(temp);				// 判断是否需要报警
		
		for(i=0;i<100;i++)				// 延时并进行按键扫描
		{
			KeyScanf();					
			DelayMs(10);								
		}
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


