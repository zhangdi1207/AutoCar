#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
//define wheel direction leftfront/leftback/rightfront/rightback
sbit LF=P2^0;
sbit LB=P2^1;
sbit RF=P2^2;
sbit RB=P2^3;
//for ultrasonic
sbit IN=P2^4;
sbit test=P2^5;
sbit OUT=P3^2;
//计数器起止位
uchar th=0x00;
uchar tl=0x00;
uchar serial_num;
uchar interLoop=30;
//判断离墙距离是否到警戒,小于8.8cm，alertDistance=1,distanceFlag判断测距是否成功，distanceTimeFlag判断计数器读数是否成功
bit alertDistance=0;
bit ultrasonicFlag=0;	  //仪器接收到信号
bit TF1Flag=0;	  //计数器溢出标志
bit TF0Flag=0;

void init_bluetooth()
{
	IE=0x90;
	TMOD=0x20;
	SCON=0x50;
	PCON=0x00;
	TH1=0xfd;
	TL1=0xfd;
	TR1=1;
	REN=1;
	
	serial_num=0;
}

void ahead()
{
	P2=0x05;
}
void back()
{
	P2=0x0a;	
}
void turnLeft()
{
	P2=0x06;
}
void turnRight()
{
	P2=0x09;
}
void stop()
{
	P2=0;
}

void getBluetooth()
{
	init_bluetooth();
	if(serial_num=='a' ||serial_num=='s'||serial_num=='d'||serial_num=='w'||serial_num=='q')
	{
		switch(serial_num)
		{
			case 'w':
			{
				if(alertDistance==0)
					ahead();
				break;
			}
			case 's':back();break;
			case 'a':turnLeft();break;
			case 'd':turnRight();break;
			case 'q':stop();break;
		}
		serial_num=0;			
	}
}
//////////////////////////////////////////////////
void delay20us()
{
	uchar i;
	for(i=0;i<20;i++);
}

void init_ultrasonic()
{
	IN=0;  	
	TMOD=0x19;
	TCON=0x11;
	TH0=th;
	TL0=tl;
	TH1=th;
	TL1=tl;
	alertDistance=0;
	ultrasonicFlag=0;
	TF0Flag=0;
	TF1Flag=0;
	IE=0x8b;	
}

void getDistance()
{
	init_ultrasonic();
	IN=1;
	delay20us();
	IN=0;
	TR1=1;
	
	while((ultrasonicFlag|TF1Flag|TF0Flag)==0);
	test=TF0Flag;
}

void main()
{	
	uchar a;
	OUT=0;
	while(1)
	{
		getDistance();
		for(a=interLoop;a>0;a--)
			getBluetooth();
	}
}


/*
void main()
{
	init();
	P2=0;
	while(1)
	{
		if(serial_num=='w')
		P2=1;
		else P2=2;
	}
}	//*/

//外部计数器0计数，判断距离是否小于8.8cm
void exterInt0() interrupt 0  using 1
{
	EA=0;
	if(TH0<0x02)
		alertDistance=1;
	ultrasonicFlag=1;
}
//计数器0/1溢出，收到返回信号但时间太长
void timer0() interrupt 1 using 2
{
	EA=0;
	TH0=th;
	TL0=tl;
	TF0Flag=1;

}
void timer1() interrupt 3	using 3
{
	EA=0;
	TH1=th;
	TL1=tl;
	TF1Flag=1;	
}



void serial() interrupt 4 using 2
{
	EA=0;
	if(RI==1)
	{
		serial_num=SBUF;
		RI=0;
	}
	if(TI==1)
		TI=0;
}