#include <stdio.h>
#include "SYS_init.h"
#include "NUC100Series.h"
#include "enableClockSource.h"
#include "setupGPIO.h"
#include "enableTimer0.h"
#include "displayDigit.h"
#include "setupKeyMatrix.h"
#include "clockState.h"

#define K1_pressed (!(PA->PIN & (1<<2)))
#define Col2_pressed (!(PA->PIN & (1<<1)))
#define Col3_pressed (!(PA->PIN & (1<<0)))

volatile int countU14 = 0;
volatile int countU13 = 0;
volatile int countU12 = 0;
volatile int countU11 = 0;

volatile int countU14temp = 0;
volatile int countU13temp = 0;
volatile int countU12temp = 0;
volatile int countU11temp = 0;



#define MAX_HISTORY 5
int history[MAX_HISTORY][4] = {0}; // Array to store up to 5 sets of counter values
volatile 
	
int history_index = 0;  
volatile int display_index = 0;

enum clock_state {START, PAUSE, IDLE, RESET, DISPLAY};
enum clock_state state = IDLE;

void TMR1_IRQHandler(void);
void saveToHistory(int,int,int,int);
void resetCounters(void);
void exitDisplay(void);
void displayLapsRecord(void);
void enterDisplay(void);


//K1 handler
static void K1_checkpress(){
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
	
	if(K1_pressed){
		if(state == START && state != DISPLAY){
			state = PAUSE;
		}else if(state != START && state != DISPLAY){
			state = START;
		}
	}
}

//K9 handler 
static void K9_checkpress(){
	PA->DOUT |= (1<<3);
	PA->DOUT |= (1<<4);		
	PA->DOUT &= ~(1<<5);
	
	if(Col3_pressed){
		if(state == PAUSE){
			resetCounters();
			state = IDLE; // Transition to IDLE after resetting
		}
		else if(state == START){
			saveToHistory(countU11, countU12, countU13, countU14);
		}
	}
}


//K5 check press
static void K5_checkpress(){
	PA->DOUT |= (1<<3);
	PA->DOUT &= ~(1<<4);
	PA->DOUT |= (1<<5);
	
	if(Col2_pressed){
		if(state != DISPLAY){
			enterDisplay();
			state = DISPLAY;
			displayLapsRecord();
		}
		else if (state == DISPLAY){
			exitDisplay();
			state = PAUSE;
		}
	}
}

//Handle PB15
static void handleRotatePress(){
	if(state == DISPLAY){
		if(display_index < 4 ){
			display_index+=1;
		}
		else{
			display_index = 0;
		}
			displayLapsRecord();
	}
}


//System state
void checkStateForClock(){
	switch (state){
		case START:
			PC->DOUT |= (1<<14);
			PC->DOUT &= ~(1<<13);
			PC->DOUT |= (1<<12);
			startCount();
			return;
			break;
		case PAUSE:
			PC->DOUT &= ~(1<<14);
			PC->DOUT |= (1<<13);
			PC->DOUT |= (1<<12);
			pauseCount();
			return;
			break;
	
		case DISPLAY:
			PC->DOUT |= (1<<14);
			PC->DOUT |= (1<<13);
			PC->DOUT |= (1<<12);
			pauseCount();
			break;
		default:
			PC->DOUT |= (1<<14);
			PC->DOUT |= (1<<13);
			PC->DOUT &= ~(1<<12);
			pauseCount();
			return;
	}
}


//Save the laps time
void saveToHistory(int min, int sec1, int sec2, int tick) {
	
	if(history_index == 0){
		 history[history_index][0] = min;
			history[history_index][1] = sec1;
			history[history_index][2] = sec2;
			history[history_index][3] = tick;
	}
	else if(history_index <= 4){
		history[history_index][0] = min - history[history_index-1][0];
    history[history_index][1] = sec1 - history[history_index-1][1];
    history[history_index][2] = sec2 - history[history_index-1][2];
    history[history_index][3] = tick - history[history_index-1][3];
	}
    // Update the index, wrapping around if it exceeds MAX_HISTORY
    history_index = (history_index + 1) % MAX_HISTORY;
}

void resetCounters() {
   countU11 = 0;
		countU12 = 0;
	countU13 = 0;
		countU14 = 0;
	
	for (int i = 0; i < MAX_HISTORY; i++) {
    for (int j = 0; j < 4; j++) {
        history[i][j] = 0;
    }
}
}


//Save the current at pause mode
void enterDisplay(){
	countU11temp = countU11;
	countU12temp = countU12;
	countU13temp = countU13;
	countU14temp = countU14;

}


//Reload the previous pause mode
void exitDisplay(){
	countU11 = countU11temp;
	countU12 = countU12temp;
	countU13 = countU13temp;
	countU14 = countU14temp;
	display_index = 0;
}

void displayLapsRecord(){
		countU11= display_index+1; // Assign history value for U11
		countU12= history[display_index][1]; // Assign history value for U12
		countU13= history[display_index][2]; // Assign history value for U13
		countU14= history[display_index][3]; // Assign history value for U14
}


int main(void)
{
	// -- SYSTEM INITIALIZATION --
	SYS_UnlockReg();
	
	// System configuration
	enableClockSource();
	setupKeyMatrix();
	setupGPIO();
	enableTimer0();
	
	SYS_LockReg();
	
	// -- DISPLAY OPERATON --
	// Toggle the output pin
	
	while(1){
		
		PA->DOUT &= ~(1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT &= ~(1<<5);
		
		checkStateForClock();
		
		if(state != IDLE){
		showNumbers(countU11,countU12,countU13, countU14);
		}
		
		if(K1_pressed){
			K1_checkpress();
			 while (K1_pressed);
		}
		if(Col3_pressed){
			K9_checkpress(); // Transition to RESET state
			while (Col3_pressed); // Debounce the button
		}	
		if(Col2_pressed){
			K5_checkpress();
			while(Col2_pressed);
		}
	}
}

void TMR0_IRQHandler(void) {
    // Clear the interrupt flag
    TIMER0->TISR |= (1 << 0);

    // Perform desired actions (e.g., increment counters, toggle LED)
    if (countU14 == 10) {
			++countU13;       // Increment U13
			countU14 = 0;        // Reset U14
    }
    if (countU13 == 10) {
				++countU12;       // Increment U12
        countU13 = 0;        // Reset U13  
    }
    if (countU12 == 6) {
			  ++countU11;       // Increment U11
        countU12 = 0;        // Reset U12
    }
    if (countU11 == 10) {
        countU11 = 0;        // Reset U11 (optional if your display rolls over)
    }
		PC->DOUT ^= (1<<8);
    countU14 += 1;
	
    // Toggle the built-in LED for frequency verification
}

void EINT1_IRQHandler(void){
	handleRotatePress();
	CLK_SysTickDelay(50);
	PB->ISRC |= (1 << 15);

}
