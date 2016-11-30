/*
PWM
16/11/30
抛弃原来错误的方法
采用定时器中断

*/
#define uint unsigned int
#define uchar unsigned char
#include<reg52.h>
#include<intrins.h>
sbit led0=P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3=P1^3;
sbit led4=P1^4;
sbit led5=P1^5;
sbit led6=P1^6;
sbit led7=P1^7;
static count=0;

void Timer0Init(void)		//20微秒@11.0592MHz
{
	//AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xFF;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
void delay(uint sec){
	
	while(sec--);

}


void Delay200ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 2;
	j = 103;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void timer0() interrupt 1{
	
	//因为定时器在溢出后寄存器中的初值寄存器自动归零，
	//所以需要重新赋值才能有想要的延时	
	TL0 = 0xFF;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
		count++;
	if (count==1 ){
		led0=~led0;}
	if (count==10){
		led0=~led0;
		count=0;
	}
	
	
}


void main()
{
	
	Timer0Init();
	//初始化定时器
	EA=1;
	//允许中中断
	ET0=1;
	//允许定时器中断
	led0=0;
	led1=0;
	while(1);
	
	
}