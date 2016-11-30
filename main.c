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

void delay(uint delays);
void Delay200ms();
void main()
{
	
	
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
