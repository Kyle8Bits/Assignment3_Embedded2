#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"


void setupGPIO(){
	// --START GPIO INITIALIZATION--
	
	PC->PMD &= ~(0xFF << 10);
	PC->PMD |= (0b01010101 << 10);
	
	PC->PMD &= ~(0x3 << 28);
	PC->PMD |= (0x1 << 28);
	
	PE->PMD &= ~(0xFFFF << 0);
	PE->PMD |= 0b0101010101010101<<0; 
	
	PB->PMD &= ~(0b11<<30);
	PB->IEN |= (1<<15);

}