#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"

void startCount(){
	TIMER0->TCSR |= (1 << 30);
}

void pauseCount(){
	 TIMER0->TCSR &= ~(1 << 30);
}