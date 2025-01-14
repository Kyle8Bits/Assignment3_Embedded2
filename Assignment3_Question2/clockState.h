#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"

void startCount(){
	//Start counting
	TIMER0->TCSR |= (1 << 30);
}

void pauseCount(){
	//Stop counting
	 TIMER0->TCSR &= ~(1 << 30);
}