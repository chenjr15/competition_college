#include "oled.h"

#define UINT unsigned int
#define UCHAR unsigned char
#define SINT short int
#define FOSC 12000000
#define F500KHz    (65536-FOSC/4/500000)


//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

#define key_address_add			keymode		//时钟地址 +
#define key_address_sub			keycolor	//时钟地址 -
#define key_value_add			keyminus	//时间值   +
#define key_value_sub			keyplus		//时间值   -


//for debug 
sbit debug1 = P1^7;
sbit debug2 = P3^3;
sbit debug3 = P3^4;
sbit debug4 = P3^5;




sfr T2MOD = 0xc9;                   //timer2 mode register

sbit led0=P2^4;
sbit led1=P2^5;

//keys
sbit keymode=P2^0;	//切换模式 自动/手动
sbit keycolor=P2^1;	//调色温
sbit keyminus=P2^2;	//+
sbit keyplus=P2^3;	//-

sbit peoplein=P1^6;

//ADC0809定义
sbit START = P3^0;
sbit EOC = P3^2;
sbit OE = P3^1;


sbit beep = P1^5;//蜂鸣器

unsigned char mon=1;
unsigned char day=11;
unsigned char hy=6;
unsigned char temper[3] = { 2,7,3 };
u8 t=0;
char fla_num = 13;//显示需要调节的位
u8 select = 0;//灯光调节与时钟调节的切换   0 为灯光调节 ；1为时间调节
char alarmOn = 0;//闹钟默认关闭

//DS1302引脚定义
sbit RST = P1^3;
sbit IO = P1^2;
sbit SCK = P1^1;


sbit key_select = P3 ^ 7;//时钟切换


UCHAR mode=0,color=1,brightness=10;
UCHAR brightness1,brightness2;
SINT brightness_auto=10;
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

int V;
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
void ADC();


typedef enum //星期
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
}Week;

//闹钟时间
typedef struct
{
	unsigned char hour[2];
	unsigned char minute[2];
}ALARM;

ALARM alarm = { 0 };//初始化闹钟
//封装时间
typedef struct {
	unsigned char year[4];//年
	unsigned char month[2];//月
	unsigned char day[2];//日
	Week week;
	unsigned char hour[2];
	unsigned char minute[2];
	unsigned char sec[2];
}TIME;

TIME time={0};


/////////////////////
//初始时间定义
UCHAR time_buf[8] = { 0x20,0x10,0x06,0x01,0x23,0x59,0x55,0x02 };//初始时间2010年6月1号23点59分55秒 星期二

void Delay6ms()		//@11.0592MHz
{
	unsigned char i, j;
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void ds1302_init(void)
{
	RST = 0;			//RST脚置低
	SCK = 0;			//SCK脚置低
}
//向DS1302写入一字节数据
void ds1302_write_byte(UCHAR addr, UCHAR d)
{
	UCHAR i;
	RST = 1;					
	//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   
	//最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i++) {
		if (addr & 0x01) {
			IO = 1;
		}
		else {
			IO = 0;
		}
		SCK = 1;      //²úÉúÊ±ÖÓ
		SCK = 0;
		addr = addr >> 1;
	}
	//产生时钟
	for (i = 0; i < 8; i++) {
		if (d & 0x01) {
			IO = 1;
		}
		else {
			IO = 0;
		}
		SCK = 1;    //产生时钟
		SCK = 0;
		d = d >> 1;
	}
	RST = 0;		//停止DS1302总线
}

//从DS1302读出一字节数据
UCHAR ds1302_read_byte(UCHAR addr) {

	UCHAR i, temp;
	RST = 1;					
	//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    
	//最低位置高，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i++) {
		if (addr & 0x01) {
			IO = 1;
		}
		else {
			IO = 0;
		}
		SCK = 1;
		SCK = 0;
		addr = addr >> 1;
	}
	//输出数据：temp
	for (i = 0; i < 8; i++) {
		temp = temp >> 1;
		if (IO) {
			temp |= 0x80;
		}
		else {
			temp &= 0x7F;
		}
		SCK = 1;
		SCK = 0;
	}
	RST = 0;					//停止DS1302总线
	return temp;
}
//向DS302写入时钟数据
void ds1302_write_time(void)
{
//	ds1302_write_byte(ds1302_control_add, 0x00);			//关闭写保护
//	ds1302_write_byte(ds1302_sec_add, 0x80);				//暂停时钟 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电
	ds1302_write_byte(ds1302_year_add, time_buf[1]);		//年
	ds1302_write_byte(ds1302_month_add, time_buf[2]);	//月
	ds1302_write_byte(ds1302_date_add, time_buf[3]);		//日 
	ds1302_write_byte(ds1302_hr_add, time_buf[4]);		//时 
	ds1302_write_byte(ds1302_min_add, time_buf[5]);		//分
	ds1302_write_byte(ds1302_sec_add, time_buf[6]);		//秒
	ds1302_write_byte(ds1302_day_add, time_buf[7]);		//周
	//ds1302_write_byte(ds1302_control_add, 0x80);			//打开写保护    
}

//从DS302读出时钟数据
void ds1302_read_time(void)
{
	time_buf[1] = ds1302_read_byte(ds1302_year_add);		//年  
	time_buf[2] = ds1302_read_byte(ds1302_month_add);		//月 
	time_buf[3] = ds1302_read_byte(ds1302_date_add);		//ÈÕ 
	time_buf[4] = ds1302_read_byte(ds1302_hr_add);		//日 
	time_buf[5] = ds1302_read_byte(ds1302_min_add);		//时 
	time_buf[6] = (ds1302_read_byte(ds1302_sec_add)) & 0x7f;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7] = ds1302_read_byte(ds1302_day_add);		//周	
}

//星期闪烁
void flashWeek(UCHAR content)
{
	if (fla_num==12&&select)
	{
		if (t > 2)
		{
			OLED_ShowCHinese(68 + 32 + 8, 6, 14);
			return;
		}
	}
	OLED_ShowCHinese(68 + 32 + 8, 6, content);
}

void showWeek(Week week)
{
	switch (week)
	{
	case Sunday:
		flashWeek(10);
		//OLED_ShowCHinese(68 + 32 + 8, 6, 9);
		break;
	case Monday:
		flashWeek(4);
		//OLED_ShowCHinese(68 + 32 + 8, 6, 4);
		break;
	case Tuesday:
		flashWeek(5);
		//OLED_ShowCHinese(68 + 32 + 8, 6, 5);
		break;
	case Wednesday:
		flashWeek(6);
		//OLED_ShowCHinse(68 + 32 + 8, 6, 6);
		break;
	case Thursday:
		flashWeek(7);
		//OLED_ShowCHinese(68 + 32 + 8, 6, 7);
		break;
	case Friday:
		flashWeek(8);
		//OLED_ShowCHinese(68 + 32 + 8, 6, 8);
		break;
	default:
		flashWeek(9);
	//	OLED_ShowCHinese(68+32+8,6,4);
		break;
	}
}


void flashAlarm() {
	if (fla_num==13&&select){
		if (t > 2){
			OLED_ShowCHinese(56, 4, 14);
			return;
		}
	}
	if (alarmOn){
		OLED_ShowCHinese(56, 4, 17);//钩
	}else{
		OLED_ShowCHinese(56, 4, 16);//叉
	}
}
//数字闪烁
void flash(UCHAR x, UCHAR y, UCHAR content, char flashNum) {
	if (flashNum==fla_num&&select)//调时间
	{
		if (t > 2)
		{
			OLED_ShowChar(x, y, 45, 16);
			return;
		}
	}
	OLED_ShowChar(x, y, content+48, 16);
}



void show(TIME* time,unsigned char *temper){
	
	showWeek(time->week);
	//小时
	flash(28, 2, time->hour[0], 0);
	flash(36, 2, time->hour[1], 1);
	//分钟
	flash(54, 2, time->minute[0], 2);
	flash(62, 2, time->minute[1], 3);
	
	//秒
	flash(80, 2, time->sec[0], 4);
	flash(88, 2, time->sec[1], 5);

	OLED_ShowChar(45, 2, ':', 16);
	OLED_ShowChar(71, 2, ':', 16);
	//显示年
	flash(0, 4, time->year[0], 20);
	flash(8, 4, time->year[1], 20);
	flash(16, 4, time->year[2], 6);//后两位十位
	flash(24, 4, time->year[3], 7);//后两位个位
	//显示第几日
	flash(34, 6, time->day[0], 10);
	flash(42, 6, time->day[1], 11);

	//显示温度
	OLED_ShowChar(58, 0, 48 + temper[0], 16);//显示温度
	OLED_ShowChar(66, 0, 48 + temper[1], 16);
	OLED_ShowChar(82, 0, 48 + temper[2], 16);
	//月
	flash(0, 6, time->month[0], 8); 
	flash(8, 6, time->month[1], 9);
	
	//闹钟
	flash(74, 4, alarm.hour[0], 14);//闹钟时
	flash(82, 4, alarm.hour[1], 15);
	flash(98, 4, alarm.minute[0], 16);//闹钟分
	flash(106, 4, alarm.minute[1], 17);

	flashAlarm();//闹钟图标

}

void Timer1Init(void)		//50毫秒@12MHz
{
	//AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0xB0;		//设置定时初值
	TH1 = 0x3C;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}

//判断是增加还是减少数值
void changeValue(char tmp) {
	switch (fla_num)
	{
	case 0://小时
		time_buf[4] += tmp * 16;
		break;
	case 1:
		time_buf[4] += tmp;
		break;
	case 2://分钟
		time_buf[5] += tmp * 16;
		break;
	case 3:
		time_buf[5] += tmp;
		break;
	case 4://秒
		time_buf[6] += tmp * 16;
		break;
	case 5:
		time_buf[6] += tmp;
		break;
	case 6://年
		time_buf[1] += tmp * 16;
		break;
	case 7:
		time_buf[1] += tmp ;
		break;
	case 8://月
		time_buf[2] += tmp * 16;
		break;
	case 9:
		time_buf[2] += tmp;
		break;
	case 10://日
		time_buf[3] += tmp * 16;
		break;
	case 11:
		time_buf[3] += tmp;
		break;
	case 12://星期
		time_buf[7] += tmp;
		break;
		//以下为闹钟部分
	case 14:
		alarm.hour[0] += tmp;
		break;
	case 15:
		alarm.hour[1] += tmp;
		break;
	case 16:
		alarm.minute[0] += tmp;
		break;
	case 17:
		alarm.minute[1] += tmp;
		break;
	default://闹钟图标 13
		alarmOn = (++alarmOn) % 2;
		break;
	}
}

	 void TimeInit(){
	 	Timer1Init();

	 	OLED_Init();			//初始化OLED  
	 	OLED_Clear();
	 	OLED_ShowCHinese(16, 0, 11);//温
	 	OLED_ShowCHinese(32, 0, 12);//度
	 	OLED_ShowChar(48, 0, ':', 16);//
	 	OLED_ShowChar(76,0,'.',16);//.

	 	OLED_ShowCHinese(96, 0, 13);//℃

	 	OLED_ShowCHinese(34, 4, 15);//年
	 	OLED_ShowCHinese(16,6,0);//月
	 	OLED_ShowCHinese(48 + 5,6,1);//日
	 	OLED_ShowCHinese(68 + 8,6,2);//星
	 	OLED_ShowCHinese(68 + 8 + 16,6,3);//期

	 									  /*闹钟部分******************************************************************/
	 	OLED_ShowCHinese(56, 4, 16);//叉
	 	OLED_ShowChar(90, 4, ':', 16);//：
	 }

	 //显示屏幕的数据
	 void ShowTime() {
	 	if (alarmOn&&!select)//闹钟
	 	{
	 		if (alarm.hour[0] == time.hour[0] && alarm.hour[1] == time.hour[1] && alarm.minute[0] == time.minute[0] && alarm.minute[1] == time.minute[1])//判断时间是否相等
	 		{
	 			beep = 0;//蜂鸣器响
	 		}
	 		else
	 		{
	 			beep = 1;
	 		}
	 	}
	 	show(&time, temper);
	 }

	////////////////////////////////////


	void Timer0Init(void)		//1毫秒@12MHz
	{
		//AUXR &= 0x7F;		//定时器时钟12T模式
		TMOD &= 0xF0;		//设置定时器模式
		TMOD |= 0x01;		//设置定时器模式
		TL0 = 0x18;		//设置定时初值
		TH0 = 0xFC;		//设置定时初值
		TF0 = 0;		//清除TF0标志
		TR0 = 1;		//定时器0开始计时
	}
	void delay(UINT sec) {

		while (sec--);

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

void scankey() {

		//灯光 时钟模式切换键
		if (key_select == 0)
		{
			Delay5ms();
			if (key_select == 0)
			{
				select = (++select) % 2;
				//这边可能有bug
				if (select)//selet为1时调时间，0为正常显示时间
				{
					ds1302_write_byte(ds1302_control_add, 0x00);			//关闭写保护
					ds1302_write_byte(ds1302_sec_add, 0x80);				//暂停时钟 
				}
				else
				{
					ds1302_write_time();
					ds1302_write_byte(ds1302_control_add, 0x80);			//打开写保护
					ds1302_read_time();
				}
			}
		}

		if (keymode == 0) {

			Delay5ms(); Delay5ms();
			if (keymode == 0 && !select) {
				while (!keymode);
				
				mode++;
				if (mode > 1)mode = 0;
			}
			if (key_address_add == 0 && select)//时钟地址增加键
			{
				fla_num++;
				if (fla_num == 18)
				{
					fla_num = 0;
				}
			}
		}

	if (keycolor == 0) {

		Delay5ms(); Delay5ms();
		if (keycolor == 0 && !select) {
			while (!keycolor);
			color += 1;
			color %= 5;//6档色温，等于5时归0
		}
		if (key_address_sub == 0 && select)//时钟地址减少键
		{
			Delay5ms();
			if (key_address_sub == 0)
			{
				fla_num--;
				if (fla_num < 0)
				{
					fla_num = 10;
				}
			}
		}
	}
	/*debug2=mode;*/

	if (keyplus == 0) {
		Delay5ms();
		if (mode == 0 && keyplus == 0&& !select) {
			while (!keyplus);
			brightness = 5 + brightness;
			if (brightness >= 15) { brightness = 15; }
		}
		if (key_value_add == 0&&select)
		{
			Delay5ms();
			if (key_value_add == 0)
			{
				changeValue(1);
			}
		}
	}


	if (keyminus == 0) {
		Delay5ms();
		if (mode == 0 && keyminus == 0 && !select) {
			while (!keyminus);
			brightness = brightness - 5;
			if (brightness <= 0)
				brightness = 5;
		}
		if (key_value_sub == 0)
		{
			Delay5ms();
			if (key_value_sub == 0)
			{
				changeValue(-1);
			}
		}
	}
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


void ADC(void){
	START=0;
	START=1;
	START=0;
	while(EOC==0);//wait for replay
	OE=1;
	
	V=(int)P0*1.96078;
	OE=0;
	brightness_auto=(int)((V-62)/420)*15;
	brightness_auto=15-brightness_auto;
	if ( brightness_auto<3) brightness_auto=3;
	if ( brightness_auto>15) brightness_auto=15;
}

void timer0() interrupt 1{
	
	//因为定时器在溢出后寄存器中的初值寄存器自动归零，
	//所以需要重新赋值才能有想要的延时	
	TL0 = 0x18;		//设置定时初值
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


void Timer2() interrupt 3	  //定时器1是3号中断
{
	
	TL1 = 0xB0;		//设置定时初值
	TH1 = 0x3C;		//设置定时初值
	TF1=0;
	debug1 = ~debug1;
	 t++;
	if(t==4)               //间隔200ms(50ms*4)读取一次时间
	{
		t=0;
		if (!select)
		{
			ds1302_read_time();  //读取时间 
		}
	
	   time.year[0]=(time_buf[0]>>4); //年   
	   time.year[1]=(time_buf[0]&0x0f);
   
	   time.year[2]=(time_buf[1]>>4);
	   time.year[3]=(time_buf[1]&0x0f);
   
	   time.month[0]=(time_buf[2]>>4); //月  
	   time.month[1] =(time_buf[2]&0x0f);
   

	   time.day[0]=(time_buf[3]>>4); //日   
	   time.day[1] =(time_buf[3]&0x0f);
   
	   time.week=(time_buf[7]&0x07); //星期
   
	   //第2行显示  
	   time.hour[0]=(time_buf[4]>>4); //时   
	   time.hour[1]=(time_buf[4]&0x0f);   

	   time.minute[0]=(time_buf[5]>>4); //分   
	   time.minute[1]=(time_buf[5]&0x0f);   

	   time.sec[0]=(time_buf[6]>>4); //秒   
	   time.sec[1]=(time_buf[6]&0x0f);
   
		//读取温度
		temper[0]=I_1;
		temper[1]=I_0;
		temper[2]=F_1;
	  }
}