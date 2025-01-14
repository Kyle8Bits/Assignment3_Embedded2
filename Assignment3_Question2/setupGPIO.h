#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"


void setupGPIO(){
	// --START GPIO INITIALIZATION--
	//Setup LED 5,6,7
	PC->PMD &= ~(0xFF << 10);
	PC->PMD |= (0b010101 << 10);

	//Setup GPC8
	PC->PMD &= ~(0b11 << 16);
	PC-> PMD |= (0b01 << 16);

	//Setup 7 segments
	PE->PMD &= ~(0xFFFF << 0);
	PE->PMD |= 0b0101010101010101<<0; 
	
	//Setup PB15
	PB->PMD &= ~(0b11ul<<30);
	PB->IEN |= (1<<15);
	
	//Enable interrupt PB15
	NVIC->ISER[0] |= (1UL << 3);
	NVIC->IP[0] &= ~(0x3UL << 30);
	
	//Enable debounce
	PB->DBEN |= 1ul << 15;
	GPIO-> DBNCECON |= 1ul << 4;
	GPIO -> DBNCECON |= (5ul << 0);
	

}