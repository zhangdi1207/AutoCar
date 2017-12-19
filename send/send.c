#include  <reg52.h>
#define uint unsigned int
#define uchar unsigned char

sbit ahead=P2^0;
sbit back=P2^1;
sbit turnLeft=P2^2;
sbit turnRight=P2^3;

void delay_ms(uchar ms)
{
	uchar a,b,c;
	for (a=0;a<ms;a++)
		for(b=0;b<3;b++)
			for(c=0;c<219;c++);
}

void init_bluetooth()
{
	TMOD=0x20;
	SCON=0x40;
	PCON=0x00;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
}
uchar getDirection()
{
	uchar a=0;
	uchar b='q';
	P2=0x0f;
	if(P2!=0x0f)
	{
		a=P2&0x0f;
		delay_ms(10);
		if(a ==(P2&0x0f))
		{
			switch(a)
			{
				case 0x0e:b='w';break;
				case 0x0d:b='s';break;
				case 0x0b:b='a';break;
				case 0x07:b='d';break;
			}
		}
	}
	else
		delay_ms(10);
	return b;
}

void sendByte(uchar a)
{
	SBUF=a;
	while(TI==0);
	TI=0;
}

void main()
{
	uchar a;
	uchar preChar=0;
	uchar count=0;
	init_bluetooth();
	while(1)
	{
		a=getDirection();
		if(a=='a' ||a=='s' ||a=='d' ||a=='w' ||a=='q')
		{
			if(preChar==a)
			{
				count+=1;
				if((count%20)<2)
					sendByte(a);				
			}
			else
			{
				count=0;
				preChar=a;
			}
		}			
	}
}