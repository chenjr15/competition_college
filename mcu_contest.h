#define uint unsigned int
#define uchar unsigned char
#define sint short int
#include<reg52.h>
#include<intrins.h>
//debug LEDs
sbit led0=P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3=P1^3;
sbit led4=P1^4;
sbit led5=P1^5;
sbit led6=P1^6;
sbit led7=P1^7;
//keys
sbit keymode=P2^0;	//切换模式 自动/手动
sbit keycolor=P2^1;	//调色温
sbit keyminus=P2^2;	//+
sbit keyplus=P2^3;	//-

sbit peoplein=P2^4;
uchar mode=0,color=1,brightness=10;
uchar brightness1,brightness2;
uchar count=0;
bit lighton=1;
void Timer0Init(void);
void delay(uint sec);
void Delay5ms();
void scankey();





void Timer0Init(void)		//1ms@11.0592MHz
{
	//AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xfc;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
void delay(uint sec){
	
	while(sec--);

}


void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 9;
	j = 244;
	do
	{
		while (--j);
	} while (--i);
}

void scankey(){
	
	if (keymode==0){ 
		//未做防抖处理
		Delay5ms();Delay5ms();
		if (keymode==0){
			while(!keymode);
			mode++;
			mode%=2;
			/*
			mode%=3;
		
			*/
	}}
	if (keycolor==0){
		//if (mode!=2){
		Delay5ms();Delay5ms();
		if (keycolor==0){
			while(!keycolor);
			color++;
			color%=10;//11档色温，等于10时归0
	//}
	/*
	if (mode==2){
		savedata();
		mode=0;
		
	}
	
	*/
	}}
	if (mode==0){
		if (keyplus==0){
			Delay5ms();
			if(keyplus==0){
				while(!keyplus);
			brightness=5+brightness;
			if(brightness>=15){brightness=15;}
			}}
		if (keyminus==0){
			Delay5ms();
			if(keyminus==0){
				while(!keyminus);
				brightness=brightness-5;
				if(brightness<=0)
					brightness=5;}
	}}
	}

void timer0() interrupt 1{
	
	//因为定时器在溢出后寄存器中的初值寄存器自动归零，
	//所以需要重新赋值才能有想要的延时	
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFc;		//设置定时初值
//	if(brightness>=30){brightness=30;}
		count++;
if(lighton){
	if (count==brightness1){
		led0=1;}
		
	if (count==brightness2){
		led1=1;}
	
	if (count>=15){
		led0=0;
		led1=0;
		count=0;}	

	//time_PWM %=20;
}
}
