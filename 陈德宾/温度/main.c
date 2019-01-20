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

sbit cs = P1^2;
sbit sck = P1^1;
sbit so = P1^0;
uchar DQ_switch = 0;			//�¶ȴ�����ѡ��  0����һ��  1���ڶ���

/*   ����ܵ���ʾֵ 0   1   2   3   4    5   6   7   8   9   -   */
uchar code Array1[]={ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40 };

/*                 0.   1.   2.   3.  4.   5.   6.  7.   8.  9.  */
uchar code Array2[]={ 0xBf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef };

uint Buff[5];					// ��ʾ������
uchar ShowID=1;				// ��ǰ��ʾ������һ�������
int AlarmLow=120;				// Ĭ�ϱ������¶�����ֵ��12��
int AlarmHig=320;				// Ĭ�ϱ������¶�����ֵ��32��
unsigned int byte,bytee,t;
/*********************************************************/
// ���뼶����ʱ������time��Ҫ��ʱ�ĺ�����
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
	cs= 0;                    //CS=0,P2.6����͵�ƽ
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
	


	while(1)
	{
		DelayMs(1000);
		temp=read_max6675();      //��ȡ�¶�
		max6675_display(temp);    //��ʾ�¶�ֵ
//		ShowTemp(temp*2);					// ��ʾ�¶�ֵ
		

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


