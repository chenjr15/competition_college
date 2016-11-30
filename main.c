/*
PWM
16/11/30
抛弃原来错误的方法
采用定时器中断
*/
/*中断号 0,2为外部中断 1,3为定时器中断 4为串口中断 
即：
0	INT0
1	T0
2	INT1
3	T1
4	serial 

TCON:D7-D0			D3
	TF1 TR1 TF0	TR0	IE1	IT1	IE0	IT0
IE	:D7-D0			
	EA			ES	ET1	EX1	ET0	EX0
TMOD:D7-D0
	GATE	C/T M1	M0|	GATE	C/T	M1	M0
			T1		  |			T0

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
static count_CB=0;
static time_PWM=0;
void Timer0Init(void)		//5微秒@11.0592MHz
{
	//AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xFB;		//设置定时初值
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
	TL0 = 0xFB;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
		count++;
		count_CB++;
	if (count==time_PWM ){
		led0=~led0;}
	if (count==50){
		led0=~led0;
		count=0;}		
	if (time_PWM >= 49){
		time_PWM=1;}
		}

		
void KeyINT0() interrupt 0{
	
	time_PWM +=20;
	
	
	
}

void main()
{
	
	Timer0Init();
	//初始化定时器
	EA=1;
	//允许中中断
	ET0=1;EX0=1;
	IT0=1;
	//允许定时器中断,外部中断,中断触发方式为下降沿触发
	led0=0;
	while(1);
	
	
}