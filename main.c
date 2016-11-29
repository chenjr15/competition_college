/*
PWM


*/
#define uint unsigned int
#define uchar unsigned char
#include<reg52.h>
#include<intrins.h>
sbit led0=P1^0;

void delay(uint delays);
void Delay200ms();
void main()
{
	uint i=1,j=0,k=0;
	while(1){	
		i=0;
	for (;i<100;i+=i){
	led0=0;
	delay(i);
	led0=1;
	delay(2001-i);
	}
	Delay200ms();
	for (;i>1;i-=i){
	led0=0;
	delay(i);
	led0=1;
	delay(2001-i);
	}
	Delay200ms();Delay200ms();
	
	}
	
	
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
