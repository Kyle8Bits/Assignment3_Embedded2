#include <stdio.h>
#include "NUC100Series.h"

void setupKeyMatrix(){
		//Set up key matrix
		PA->PMD &= (~(0b11<< 6));
    PA->PMD |= (0b01 << 6);    
		PA->PMD &= (~(0b11<< 8));
    PA->PMD |= (0b01 << 8);  		
		PA->PMD &= (~(0b11<< 10));
    PA->PMD |= (0b01 << 10);  
}