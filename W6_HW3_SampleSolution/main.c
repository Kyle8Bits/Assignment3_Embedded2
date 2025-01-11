#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"
#include "enableClockSource.h"
#include "setupGPIO.h"
#include "enableTimer0.h"
#include "displayDigit.h"
volatile int countU13 = 0;
volatile int countU14 = 0;
volatile int countU12 = 0;
volatile int countU11 = 0;

void TMR1_IRQHandler(void);
int main(void)
{
	// -- SYSTEM INITIALIZATION --
	SYS_UnlockReg();
	
	// System configuration
	enableClockSource();
	setupGPIO();
	enableTimer0();
	
	SYS_LockReg();
	
	// -- DISPLAY OPERATON --
	// Toggle the output pin
	
	while(1){
		showNumbers(countU11,countU12,countU13, countU14);
	}
}

void TMR0_IRQHandler(void) {
    // Clear the interrupt flag
    TIMER0->TISR |= (1 << 0);

    // Perform desired actions (e.g., increment counters, toggle LED)
    if (countU14 == 10) {
    countU14 = 0;        // Reset U14
    countU13 += 1;       // Increment U13
    }
    if (countU13 == 10) {
        countU13 = 0;        // Reset U13
        countU12 += 1;       // Increment U12
    }
    if (countU12 == 6) {
        countU12 = 0;        // Reset U12
        countU11 += 1;       // Increment U11
    }
    if (countU11 == 10) {
        countU11 = 0;        // Reset U11 (optional if your display rolls over)
    }

    countU14 += 1;

    // Toggle the built-in LED for frequency verification
    PC->DOUT ^= (1 << 14);
}

