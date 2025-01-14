#include <stdio.h>
#include "NUC100Series.h"

#define K1_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))

void setupKeyMatrix(){
		//Set up key matrix
		PA->PMD &= (~(0b11<< 6));
    PA->PMD |= (0b01 << 6);    
		PA->PMD &= (~(0b11<< 8));
    PA->PMD |= (0b01 << 8);  		
		PA->PMD &= (~(0b11<< 10));
    PA->PMD |= (0b01 << 10);  
}