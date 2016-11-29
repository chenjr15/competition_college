/*
PWM


*/
#define uint unsigned int
#define uchar unsigned char
#include<reg52.h>
sbit led0 P1^0;

void delay(uint delays)

void main()
{
	uint i=0,j=0,k=0;
	while(1){	
	for (;i<100;i++){
	led0=0;
	delay(10*i);
	led0=1;
	delay(100);
	}
	for (;i>0;i--){
	led0=0;
	delay(10*i);
	led0=1;
	delay(100);
	}
	
	
	}
	
	
}


void delay(uint sec){
	
	for(){
		
		
		
	}
	
	
}