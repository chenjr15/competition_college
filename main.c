
/*PWM OK

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
#include <reg52.h>
#include <intrins.h>
#include <math.h>
#include "mcu_contest.h"



void main(){
	
	Timer0Init();
	//初始化定时器
	EA=1;
	//允许中断
	ET0=1;EX0=1;
	IT0=1;
	//允许定时器中断
    T2MOD = 0x02;                   //enable timer2 output clock
    RCAP2L = TL2 = F500KHz;        //initial timer2 low byte
    RCAP2H = TH2 = F500KHz >> 8;   //initial timer2 high byte
    TR2 = 1;                        //timer2 start running
	//EA=1;
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 
	TimeInit();
	peoplein=0;
	while(1){
	/*
	1.扫描按键　（亮度／模式／色温）
	2.判断模式，自动或手动
	若自动：
		读取红外传感器的值
			若为0 设置PWM亮度（占空比）为1
			若为1 读取亮度传感器的值，并设置相应占空比
	若手动;
		设置亮度往传感器对应值接近两个单位  正常亮度时光敏电阻传感器大概为2.5-2.7K
	3.读取温度湿度
	4.读取时间
	5.刷新OLED屏 时间，日期，温度/湿度，亮度，自动/手动
	
	调整时间未完成
	PWM将在定时器中断中完成
	
	
	*/
		//1.key
		scankey();
		brightness1=(UCHAR)brightness*(color*0.2);
		brightness2=brightness-brightness1;
		
		ReadTemperature();
		showtime();

		switch(mode){
			case 0: //手动
			lighton=1;
				break;
			case 1://自动
				if(!peoplein) {
					lighton=0;
				}else{
					lighton=1;
					ADC();
					if ((brightness>brightness_auto)&&(brightness_auto<=15)) brightness+=2;
					if ((brightness<brightness_auto)&&(brightness_auto>=0)) brightness-=2;	
				}
				break;}
				//debug
				
//				OLED_ShowChar(4,0,mode+'0',16);
//				OLED_ShowChar(20,0,color+'0',16);
//				OLED_ShowNum(36,0,brightness,2,16);
//				OLED_ShowNum(58,0,brightness1,2,16);
//				OLED_ShowNum(90,0,brightness2,2,16);
//				OLED_ShowNum(0,2,V,3,16);
//				OLED_ShowNum(58,2,count,1,16);
//				OLED_ShowNum(90,2,peoplein,1,16);
//				OLED_ShowNum(106,2,lighton,2,16);
//				OLED_ShowNum(0,4,I_1*100+I_0*10+F_1,3,16);
		t++;
		if (t == 4)               //间隔200ms(50ms*4)读取一次时间
		{
			t = 0;
		}
		if (!select)
		{
			ds1302_read_time();  //读取时间 
		}

		time.year[0] = (time_buf[0] >> 4); //年   
		time.year[1] = (time_buf[0] & 0x0f);

		time.year[2] = (time_buf[1] >> 4);
		time.year[3] = (time_buf[1] & 0x0f);

		time.month[0] = (time_buf[2] >> 4); //月  
		time.month[1] = (time_buf[2] & 0x0f);


		time.day[0] = (time_buf[3] >> 4); //日   
		time.day[1] = (time_buf[3] & 0x0f);

		time.week = (time_buf[7] & 0x07); //星期

										  //第2行显示  
		time.hour[0] = (time_buf[4] >> 4); //时   
		time.hour[1] = (time_buf[4] & 0x0f);

		time.minute[0] = (time_buf[5] >> 4); //分   
		time.minute[1] = (time_buf[5] & 0x0f);

		time.sec[0] = (time_buf[6] >> 4); //秒   
		time.sec[1] = (time_buf[6] & 0x0f);

		//读取温度
		temper[0] = I_1;
		temper[1] = I_0;
		temper[2] = F_1;
	}
}