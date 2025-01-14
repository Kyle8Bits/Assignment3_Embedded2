#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"


#define HXT_STATUS 1 << 0
#define LXT_STATUS 1 << 1


void enableClockSource(){

	// --START SYSTEM INITIALIZATION--
	// Enable HXT
	CLK->PWRCON |= (1 << 0);
	while(!(CLK->CLKSTATUS & HXT_STATUS));
	
	//Enable LXT - 32768kHz
	CLK->PWRCON |= (1 << 1);
	while(!(CLK->CLKSTATUS & LXT_STATUS));
	
	// Select LXT as clock source
	CLK->CLKSEL0 &= ~(0x7 << 0);
	CLK->CLKSEL0 |= (0x1 << 0);
	
	// Set power mode
	CLK->PWRCON &= ~(1 << 7);
	
	// Set up CLKDIV
	CLK->CLKDIV &= ~(0xF << 0);
	
	// --END SYSTEM INITIALIZATION--
}