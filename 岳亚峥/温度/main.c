#include <reg52.h>			// ����ͷ�ļ�
#include <intrins.h>

#define uchar unsigned char		// �Ժ�unsigned char�Ϳ�����uchar����
#define uint  unsigned int		// �Ժ�unsigned int �Ϳ�����uint ����


sbit w1 = P2^4; 				// ����ܵ�1λ�Ŀ�������
sbit w2 = P2^5;				// ����ܵ�2λ�Ŀ�������
sbit w3 = P2^6;				// ����ܵ�3λ�Ŀ�������
sbit w4 = P2^7;			 	// ����ܵ�4λ�Ŀ�������
sbit DQ1 = P1^0;				// ��һ��DS18B20�����������Ŷ���
sbit DQ2 = P2^1;				// �ڶ���DS18B20�����������Ŷ���
sbit LedLow  = P1^2;			// �¶ȹ���ָʾ��
sbit LedHig  = P1^4;			// �¶ȹ���ָʾ��
sbit Buzzer  = P2^3;			// ����������
sbit KeySet  = P3^2;			// ���ð���
sbit KeyDown = P3^3;			// ������
sbit KeyUp   = P3^4;			// �Ӱ���

uchar DQ_switch = 0;			//�¶ȴ�����ѡ��  0����һ��  1���ڶ���

/*   ����ܵ���ʾֵ 0   1   2   3   4    5   6   7   8   9   -   */
uchar code Array1[]={ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40 };

/*                 0.   1.   2.   3.  4.   5.   6.  7.   8.  9.  */
uchar code Array2[]={ 0xBf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };

uchar Buff[4];					// ��ʾ������
uchar ShowID=1;				// ��ǰ��ʾ������һ�������
int AlarmLow=120;				// Ĭ�ϱ������¶�����ֵ��12��
int AlarmHig=320;				// Ĭ�ϱ������¶�����ֵ��32��

/*********************************************************/
// ���뼶����ʱ������time��Ҫ��ʱ�ĺ�����
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=0;i<time;i++)
		for(j=0;j<112;j++);
}

/*********************************************************/
// ��ʱ15΢��
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
// ��λDS18B20����ʼ����	DQ1����һ���¶ȴ�����
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
// ��DS18B20д��һ���ֽ�	DQ1����һ���¶ȴ�����
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
		
		if(btmp>0)		// д1
		{
			DQ1=0;
			Delay15us();
			DQ1=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// д0
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
// ��ȡDQ1�¶�ֵ		DQ1����һ���¶ȴ�����
/*********************************************************/
int DQ1_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DQ1_ReSet();					// ������λ��
	DQ1_WriteByte(0xcc);			// ����ROMָ��
	DQ1_WriteByte(0x44);			// �����¶�ת��ָ��
	DQ1_ReSet();					// ������λ��
	DQ1_WriteByte(0xcc);			// ����ROMָ��
	DQ1_WriteByte(0xbe);			// ��ȡ�¶�ָ��

	for(j=0;j<16;j++)					// ��ȡ�¶�����
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
	temp=temp*0.0625*10;				// �ϳ��¶�ֵ���Ŵ�10��					
	return (temp);						// ���ؼ�⵽���¶�ֵ
}


/*********************************************************/
// ��λDS18B20����ʼ����	  DQ2���ڶ����¶ȴ�����
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
// ��DS18B20д��һ���ֽ�	 DQ2���ڶ����¶ȴ�����
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
		
		if(btmp>0)		// д1
		{
			DQ2=0;
			Delay15us();
			DQ2=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// д0
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
// ��ȡDQ2�¶�ֵ		DQ2���ڶ����¶ȴ�����
/*********************************************************/
int DQ2_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DQ2_ReSet();					// ������λ��
	DQ2_WriteByte(0xcc);			// ����ROMָ��
	DQ2_WriteByte(0x44);			// �����¶�ת��ָ��
	DQ2_ReSet();					// ������λ��
	DQ2_WriteByte(0xcc);			// ����ROMָ��
	DQ2_WriteByte(0xbe);			// ��ȡ�¶�ָ��

	for(j=0;j<16;j++)					// ��ȡ�¶�����
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
	temp=temp*0.0625*10;				// �ϳ��¶�ֵ���Ŵ�10��					
	return (temp);						// ���ؼ�⵽���¶�ֵ
}

/*********************************************************/
// ��ʱ����ʼ��
/*********************************************************/
void TimerInit()
{
	TMOD = 0x01;						// ʹ�ö�ʱ��0��������ʽ1	 
	TH0  = 248;						// ����ʱ��0��TH0װ��ֵ
	TL0  = 48;						// ����ʱ��0��TL0װ��ֵ
	ET0  = 1;							// ��ʱ��0�ж�ʹ��
	EA   = 1;						// �����ж�
	TR0	 = 1;							// ������ʱ��0
}

/*********************************************************/
// ��ʾ�¶�ֵ
/*********************************************************/
void ShowTemp(int dat)
{
	if(dat<0)							// ����
	{
		Buff[0]=Array1[10];
		dat=0-dat;
	}
	else								// ��λ
	{
		Buff[0]=Array1[dat/1000];
	}
	
	Buff[1]=Array1[dat%1000/100];		// ʮλ
	Buff[2]=Array2[dat%100/10];			// ��λ
	Buff[3]=Array1[dat%10];				// С����һλ
}

/*********************************************************/
// �����ж�
/*********************************************************/
void AlarmJudge(int dat)
{
	if(dat<AlarmLow)						// �ж��¶��Ƿ����
	{
		LedLow=0;
		LedHig=1;
		Buzzer=0;
	}
	else if(dat>AlarmHig)					// �ж��¶��Ƿ����
	{
		LedLow=1;
		LedHig=0;
		Buzzer=0;
	}
	else									// �¶�����
	{
		LedLow=1;
		LedHig=1;
		Buzzer=1;
	}
}

/*********************************************************/
// ����ɨ��
/*********************************************************/
void KeyScanf()
{
	static uchar Key_set_num = 0;
	uchar Break_flag = 0;
	if(KeySet==0)							// ������ð���������
	{
		DelayMs(10);						// ��ʱȥ��
		if(KeySet == 0)
		{
			while(!KeySet);					// �ȴ������ͷ�
			Key_set_num ++;					//���ܼ�����
				
		}
		
	}
	if(Key_set_num ==0)
	{
		 DQ_switch = 0;			   			//ѡ���һ���¶ȴ�����
	}
	else if(Key_set_num == 1)
	{
		DQ_switch = 1;			   			//ѡ��ڶ����¶ȴ�����
	}
	else if(Key_set_num == 2)
	{
		/* �����¶����� */
		LedLow=0;							// ������ɫ�ƣ�����ǰ���������¶����ޣ�
		LedHig=1;							// Ϩ���ɫ��
		Buzzer=1;							// �رշ�����
		ShowTemp(AlarmLow);					// ��ʾ�¶�����ֵ
		DelayMs(10);						// ��ʱȥ��
		while(!KeySet);						// �ȴ������ͷ�
		DelayMs(10);						// ��ʱȥ��
		
		while(1)
		{
			if(KeyDown==0)					// �������������������
			{
				if(AlarmLow>-550)			// �жϵ�ǰ�¶������Ƿ����-55��
				{
					AlarmLow--;				// �¶�����ֵ��ȥ0.1��
					ShowTemp(AlarmLow);		// ˢ����ʾ�ı����¶�����ֵ
					DelayMs(200);			// ��ʱ
				}
			}
			
			if(KeyUp==0)					// ������ӡ�����������					
			{
				if(AlarmLow<1250)			// �жϵ�ǰ�¶������Ƿ�С��125��
				{
					AlarmLow++;				// �¶�����ֵ����0.1��
					ShowTemp(AlarmLow);		// ˢ����ʾ�ı����¶�����ֵ
					DelayMs(200);			// ��ʱ
				}
			}
			
			if(KeySet==0)					// ��������á�����������
			{
				Key_set_num = 3;
				break;
			}
		}
		/* �˳�����ģʽ */
		LedLow=1;							// Ϩ���̵�
		LedHig=1;							// Ϩ����
		DelayMs(10);						// ��ʱȥ��
		while(!KeySet);						// �ȴ������ͷ�
		DelayMs(10);						// ��ʱȥ��
	}
	
	if(Key_set_num == 3)
	{
		/* �����¶����� */
		LedLow=1;							// Ϩ����ɫ��
		LedHig=0;							// ������ɫ�ƣ�����ǰ���������¶����ޣ�
		ShowTemp(AlarmHig);					// ��ʾ�¶�����ֵ
		DelayMs(10);						// ��ʱȥ��
		while(!KeySet);						// �ȴ������ͷ�
		DelayMs(10);						// ��ʱȥ��
		
		while(1)
		{
			if(KeyDown==0)					// �������������������							
			{
				if(AlarmHig>-550)			// �жϵ�ǰ�¶������Ƿ����-55��
				{
					AlarmHig--;				// �¶�����ֵ��ȥ0.1��
					ShowTemp(AlarmHig);		// ˢ����ʾ�ı����¶�����ֵ
					DelayMs(200);			// ��ʱ
				}
			}
			
			if(KeyUp==0)					// ������ӡ�����������					
			{
				if(AlarmHig<1250)			// �жϵ�ǰ�¶������Ƿ�С��125��
				{
					AlarmHig++;				// �¶�����ֵ����0.1��
					ShowTemp(AlarmHig);		// ˢ����ʾ�ı����¶�����ֵ
					DelayMs(200);
				}
			}
			
			if(KeySet==0)					// ��������á�����������
			{
				Key_set_num = 0;
				DQ_switch = 0;
				break;						// ׼���˳�����ģʽ
			}
		}
		/* �˳�����ģʽ */
		LedLow=1;							// Ϩ���̵�
		LedHig=1;							// Ϩ����
		DelayMs(10);						// ��ʱȥ��
		while(!KeySet);						// �ȴ������ͷ�
		DelayMs(10);						// ��ʱȥ��
	}
}

/*********************************************************/
// ������
/*********************************************************/
void main()
{
	int temp;
	uchar i;
	
	TimerInit();						// ��ʱ��0�ĳ�ʼ��������ܵĶ�̬ɨ�裩
	
	Buff[0]=Array1[0];					// ���ϵ���ʾ4��0
	Buff[1]=Array1[0];
	Buff[2]=Array1[0];
	Buff[3]=Array1[0];
	
	for(i=0;i<8;i++)  	// ���������ϵ�������¶Ȳ��ȶ�������Ƚ��м��ζ�ȡ������
	{
		DQ1_ReadTemp();
		DQ2_ReadTemp();
		DelayMs(120);
	}

	while(1)
	{
		EA=0;							// �����ж�
		if(DQ_switch == 0 )
			temp=DQ1_ReadTemp();		// ��ȡ��һ��DS18B20�¶�ֵ
		else 
			temp=DQ2_ReadTemp();		// ��ȡ�ڶ���DS18B20�¶�ֵ
		EA=1;							// �ָ��ж�
		
		ShowTemp(temp);					// ��ʾ�¶�ֵ
		
		AlarmJudge(temp);				// �ж��Ƿ���Ҫ����
		
		for(i=0;i<100;i++)				// ��ʱ�����а���ɨ��
		{
			KeyScanf();					
			DelayMs(10);								
		}
	}
}

/*********************************************************/
// ��ʱ��0�������
/*********************************************************/
void Timer0(void) interrupt 1
{
	TH0  = 248;				// ����ʱ��0��TH0װ��ֵ
	TL0  = 48;				// ����ʱ��0��TL0װ��ֵ

	P0=0x00;					// �ȹر�������ʾ
	w1=1;
	w2=1;
	w3=1;
	w4=1;

	if(ShowID==1)  			// �ж��Ƿ���ʾ��1λ�����
	{
		w1=0;		   		// �򿪵�1λ����ܵĿ��ƿ���
		P0=Buff[0];	   		// ��1λ�������ʾ����	
	}
	
	if(ShowID==2)  			// �ж��Ƿ���ʾ��2λ�����
	{
		w2=0;		   		// �򿪵�2λ����ܵĿ��ƿ���
		P0=Buff[1];	   		// ��2λ�������ʾ����	
	}
	
	if(ShowID==3)  			// �ж��Ƿ���ʾ��3λ�����
	{
		w3=0;		   		// �򿪵�3λ����ܵĿ��ƿ���
		P0=Buff[2];	   		// ��3λ�������ʾ����	
	}
	
	if(ShowID==4)  			// �ж��Ƿ���ʾ��4λ�����
	{
		w4=0;		   		// �򿪵�4λ����ܵĿ��ƿ���
		P0=Buff[3];	   		// ��4λ�������ʾ����	
	}	
	
	ShowID++;	  			// �л�����һ������ܵ���ʾ
	if(ShowID==5)
		ShowID=1;
}


