#include <stdio.h>
#include "NUC100Series.h"

/* 
NS state - traffic going straight through in North and South directions
EW state - traffic going straight through in East and West directions
*/

enum light_state {NS, EW};
enum light_state state = NS; // Initial state is NS

void GPIO_Init(void)
{
    // Configure LED5 and LED8 as output
    PC->PMD &= (~(0xFF << 24));  // Clear PMD[24:31]
    PC->PMD |= (0x01 << 24);     // Set PC12 (LED5) as output
    PC->PMD |= (0x01 << 30);     // Set PC15 (LED8) as output

    // Configure K1 (e.g., PB0) as input
    PB->PMD &= ~(0x3);           // Set PB0 as input (PMD[1:0] = 00)
    PB->DBEN |= (1 << 0);        // Enable debounce for PB0
}

int main(void)
{
    GPIO_Init();                 // Initialize GPIO pins

		PA->PMD &= (~(0b11<< 6));
    PA->PMD |= (0b01 << 6);    
		PA->PMD &= (~(0b11<< 8));
    PA->PMD |= (0b01 << 8);  		
		PA->PMD &= (~(0b11<< 10));
    PA->PMD |= (0b01 << 10); 
	
    while (1) {
        CLK_SysTickDelay(100000); // Delay to allow for stable button reading
        
				PA->DOUT &= ~(1<<3);
				PA->DOUT &= ~(1<<4);
				PA->DOUT &= ~(1<<5);
        if (!(PA->PIN & (1<<0))) {  // PB0 is K1
            CLK_SysTickDelay(20000);      // Simple debounce delay
            while ((PB->PIN & (1 << 0)) == 0); // Wait for button release
            
            // Change state on button press
            if (state == NS) {
                state = EW;
            } else {
                state = NS;
            }
        }

        // Update LEDs based on state
        switch (state) {
            case NS: 
                PC->DOUT |= (1 << 12);    // Turn on LED5
                PC->DOUT &= ~(1 << 15);  // Turn off LED8
                break;

            case EW: 
                PC->DOUT &= ~(1 << 12);  // Turn off LED5
                PC->DOUT |= (1 << 15);   // Turn on LED8
                break;

            default: 
                PC->DOUT &= ~(1 << 12);  // Turn off LED5
                PC->DOUT &= ~(1 << 15);  // Turn off LED8
                state = NS; 
                break;
        }
    }
}