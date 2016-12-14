#define UINT unsigned int
#define UCHAR unsigned char
#define SINT short int
#include<reg52.h>
#include<intrins.h>

sbit led0=P2^4;
sbit led1=P2^5;

//keys
sbit keymode=P2^0;	//切换模式 自动/手动
sbit keycolor=P2^1;	//调色温
sbit keyminus=P2^2;	//+
sbit keyplus=P2^3;	//-
sbit keytime=P3^7;
sbit peoplein=P1^6;
UCHAR mode=0,color=1,brightness=10;
UCHAR brightness1,brightness2;
UCHAR count=0;
bit lighton=1;
sbit DQ=P1^4; //DS18B20数据传输线接单片机的相应的引脚 
UCHAR I_1,I_0,F_1,F_0;//
unsigned char tempL=0; //设全局变量
unsigned char tempH=0; 
unsigned int sdata;//测量到的温度的整数部分
unsigned char xiaoshu1;//小数第一位
unsigned char xiaoshu2;//小数第二位
unsigned char xiaoshu;//两位小数
char  fg=1;    
void Timer0Init(void);
void delay(UINT sec);
void Delay5ms();
void scankey();
void delay18b20(unsigned char i);
void delay1ms();
void Init_DS18B20(void) ;
ReadOneChar(void);
void WriteOneChar(unsigned char dat);
void ReadTemperature(void);


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
void delay(UINT sec){
	
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
			if(mode>1)mode=0;
			/*
			mode%=3;
		
			*/
	}}
	if (keycolor==0){
		//if (mode!=2){
		Delay5ms();Delay5ms();
		if (keycolor==0){
			while(!keycolor);
			color+=1;
			color%=5;//6档色温，等于5时归0
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






//延时时间是time=i*8+10us

void delay18b20(unsigned char i){
 for(i;i>0;i--);
}


//***********************************************************

//                     延时子程序

//************************************************************
void delay1ms()
{
 unsigned char i;
 for(i=124;i>0;i--);  //延时124*8+10=1002us
}



//*****************************初始化程序 *********************************//

void Init_DS18B20(void) 
{

 DQ=1; //DQ先置高 
 delay18b20(8); //稍延时
 DQ=0; //发送复位脉冲 
 delay18b20(80); //延时（>480us) 
 DQ=1; //拉高数据线 
 delay18b20(5); //等待（15~60us)
 delay18b20(20); 
} 


//**********读一个字节************//

ReadOneChar(void)  
{
unsigned char i=0; 
unsigned char dat=0; 
for (i=8;i>0;i--) //一个字节有8位 
{
 DQ=1; 
 delay18b20(1); 
 DQ=0;
 dat>>=1; 
 DQ=1; 
 if(DQ) 
 dat|=0x80; 
 delay18b20(4);
} 
return(dat);
}



//*********************** **写一个字节**************************//

void WriteOneChar(unsigned char dat) 
{ 
  unsigned char i=0; 
  for(i=8;i>0;i--) 
  {
   DQ=0;
   DQ=dat&0x01; 
   delay18b20(5); 
   DQ=1; 
   dat>>=1;
  } 
  delay18b20(4);
}


//读温度值（低位放tempL;高位放tempH;）
void ReadTemperature(void) 
{ 
 Init_DS18B20(); //初始化
 WriteOneChar(0xcc); //跳过读序列号的操作
 WriteOneChar(0x44); //启动温度转换
 delay18b20(125); 
 Init_DS18B20(); 
 WriteOneChar(0xcc); //跳过读序列号的操作 
 WriteOneChar(0xbe);  
 tempL=ReadOneChar(); 
 tempH=ReadOneChar(); 
  
		if(tempH>0x7f)      //最高位为1时温度是负
		{
		 tempL=~tempL;         //补码转换，取反加一
		 tempH=~tempH+1;       
		 fg=1;      
       }else fg=0;
		sdata = tempL/16+tempH*16;      //整数部分
		xiaoshu1 = (tempL&0x0f)*10/16; //小数第一位
		xiaoshu2 = (tempL&0x0f)*100/16%10;//小数第二位
		//xiaoshu=xiaoshu1*10+xiaoshu2; //小数两位
		I_0=sdata%10;
		I_1=sdata/10;
		F_1=xiaoshu1;
		}

void timer0() interrupt 1{
	
	//因为定时器在溢出后寄存器中的初值寄存器自动归零，
	//所以需要重新赋值才能有想要的延时	
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFc;		//设置定时初值
		count++;
if(lighton==1){
	if (count==brightness1){
		led0=0;}
		
	if (count==brightness2){
		led1=0;}
	
	if (count>=15){
		led0=1;
		led1=1;
		count=0;}	

	//time_PWM %=20;
}
}
