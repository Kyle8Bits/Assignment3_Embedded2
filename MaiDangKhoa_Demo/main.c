#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"
#include "sevenSegments.h"

#define TIMER3_COUNT 4000000 - 1

#define HXT_STATUS 1 << 0

void enableClockSource(void);
void enableTimer3(void);
void setupGPIO(void);


volatile int segment_number = 0;

enum segment_state
{
	ON,
	OFF
};
enum segment_state state = ON;

int main(void){
	
	SYS_UnlockReg();
	enableClockSource();
	setupGPIO();
	enableTimer3();

	SYS_LockReg();
	
	while(1){
	}
}




void enableClockSource(){
	CLK->PWRCON |= (1 << 0);
	while (!(CLK->CLKSTATUS & HXT_STATUS));

	CLK->CLKSEL0 &= ~(0x7 << 0);

	CLK->PWRCON &= ~(1 << 7);

	CLK->CLKDIV &= ~(0xF << 0);
	CLK->CLKDIV |= (0b10 << 0);
}

void enableTimer3(){
	CLK->CLKSEL1 &= ~(0x7 << 20);
	CLK->CLKSEL1 |= (0b010 << 20);

	CLK->APBCLK |= (1 << 5);

	TIMER3->TCSR &= ~(0xFF << 0);
	TIMER3->TCSR |= (1 << 26);

	TIMER3->TCSR &= ~(0x3 << 27);
	TIMER3->TCSR |= (0x1 << 27);
	TIMER3->TCSR &= ~(1 << 24);

	TIMER3->TCSR |= (1 << 16);

	TIMER3->TCSR |= (1 << 29);

	NVIC->ISER[0] = (1 << 11);
	NVIC->IP[2] &= ~(0b11 << 30);

	TIMER3->TCMPR = TIMER3_COUNT;

	TIMER3->TCSR |= (1 << 30);
}

void setupGPIO(){
	PE->PMD &= ~(0xFFFF << 0);
	PE->PMD |= 0b0101010101010101<<0;
	
	PB->PMD &= ~(0b11ul << 30);
	PB->IEN |= (1 << 15);
	
	NVIC->ISER[0] |= (1 << 3);
	NVIC->IP[0] &= ~(0b11 << 30);
}

void TMR3_IRQHandler(void){
	if (state == ON)
	{
		state = OFF;
		turnOffDigit();
	}
	else
	{
		state = ON;
		if(segment_number == 0){
			turnOnU14();
		}
		else if(segment_number == 1){
			turnOnU13();
		}
		else if(segment_number == 2){
			turnOnU12();
		}
		else if(segment_number == 3){
			turnOnU11();
		}
		updateDigit(segment_number);
	}
	TIMER3->TISR |= (1 << 0);
}

void EINT1_IRQHandler(void){
	segment_number++;
	if (segment_number > 3)
	{
		segment_number = 0;
	}
	CLK_SysTickDelay(100000);
	PB->ISRC |= (1 << 15);
}

