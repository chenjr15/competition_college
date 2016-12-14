
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
#include "mcu_contest.h"


void main(){
	
	Timer0Init();
	//初始化定时器
	EA=1;
	//允许中中断
	ET0=1;EX0=1;
	IT0=1;
	//允许定时器中断,外部中断,中断触发方式为下降沿触发
	//led0=0;
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
		brightness1=(uchar)brightness*(color*0.1);
		brightness2=brightness-brightness1;
		scankey();
		switch(mode){
			case 0: //手动
			lighton=1;
				break;
			case 1://自动
				if(!peoplein) {
					lighton=0;
				}else{
					lighton=1;
				}
			
			
				break;
		}
		
	}
	
	
}