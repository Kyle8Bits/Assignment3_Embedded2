//------------------------------------------- main.c CODE STARTS -------------------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"

#define HXT_STATUS 1<<0
#define PLL_STATUS 1<<2
#define PLLCON_FB_DV_VAL 10
#define CPUCLOCKDIVIDE 1
//Macro define when there are keys pressed in each column
#define C3_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))

//Gloabl Array to display on 7segment for NUC140 MCU
int pattern[] = {
               //   gedbaf_dot_c
                  0b10000010,  //Number 0          // ---a----
                  0b11101110,  //Number 1          // |      |
                  0b00000111,  //Number 2          // f      b
                  0b01000110,  //Number 3          // |      |
                  0b01101010,  //Number 4          // ---g----
                  0b01010010,  //Number 5          // |      |
                  0b00010010,  //Number 6          // e      c
                  0b11100110,  //Number 7          // |      |
                  0b00000010,  //Number 8          // ---d----
                  0b01000010,   //Number 9
                  0b11111111   //Blank LED 
                };    
								
static void search_col1(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C1_pressed)
    {
		PE->DOUT = pattern[1];
        return;	
    }
  else
	{		
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);	
    if (C1_pressed)
    {
        // If column1 is LOW, detect key press as K4 (KEY 4)
		PE->DOUT = pattern[4];
        return;
    }
     else{
			 
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);	
    if (C1_pressed)
    {
        // If column1 is LOW, detect key press as K7 (KEY 7)
				PE->DOUT = pattern[7];
        return;
    }
	else
		return;
	}
	}
}		

static void search_col2(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C2_pressed)
    {
        // If column2 is LOW, detect key press as K2 (KEY 2)
		PE->DOUT = pattern[2];
        return;
    }
	else
	{  
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);
    if (C2_pressed)
    {
        // If column2 is LOW, detect key press as K5 (KEY 5)
		PE->DOUT = pattern[5];
        return;
    }	
		else
		{
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);
    if (C2_pressed)
    {
        // If column3 is LOW, detect key press as K8 (KEY 8)
		PE->DOUT = pattern[8];
        return;
    }
		else	
		return;
	}
	}
}	

static void search_col3(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT &= ~(1<<3);
		PA->DOUT |= (1<<4);
		PA->DOUT |= (1<<5);
     
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K3 (KEY 3)
		PE->DOUT = pattern[3];
        return;
    }
		else
		{
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K6 (KEY 6)
		PE->DOUT = pattern[6];
        return;
    }
		else
		{
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);		
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K9 (KEY 9)
		PE->DOUT = pattern[9];
        return;
    }
		else
		return;
	}
	}
}	
														
int main(void)
{    
    SYS_UnlockReg(); // Unlock protected registers
    
    //Enable clock sources
    CLK->PWRCON |= 1<<0;
    while(!(CLK->CLKSTATUS & HXT_STATUS));
    
    //PLL configuration starts
    CLK->PLLCON &= (~(1<<19));
    CLK->PLLCON &= (~(1<<16));
    CLK->PLLCON &= (~(0x01FF << 0));
    CLK->PLLCON |= PLLCON_FB_DV_VAL;	
    CLK->PLLCON &= (~(1<<18));   
    while(!(CLK->CLKSTATUS & PLL_STATUS));
    //PLL configuration ends
    
    //clock source selection
    CLK->CLKSEL0 &= (~(0b111<<0));
    CLK->CLKSEL0 |= 0b010;
    
    //clock frequency division
    CLK->CLKDIV &= ~(0xF<<0);
    CLK->CLKDIV |= (CPUCLOCKDIVIDE-1);    
   
    SYS_LockReg();  // Lock protected registers
    
	//Configure GPIO for Key Matrix
	//Rows - outputs
	PA->PMD &= (~(0b11<< 6));
    PA->PMD |= (0b01 << 6);    
	PA->PMD &= (~(0b11<< 8));
    PA->PMD |= (0b01 << 8);  		
	PA->PMD &= (~(0b11<< 10));
    PA->PMD |= (0b01 << 10);  
		
	//	//COLUMN 	
	//For GPIOs in the column - we will keep them as default (the GPIOs will be in quasi-mode instead of inputs)
		
	//Configure GPIO for 7segment
	//Set mode for PC4 to PC7 
    PC->PMD &= (~(0xFF<< 8));		//clear PMD[15:8] 
    PC->PMD |= (0b01010101 << 8);   //Set output push-pull for PC4 to PC7
	//Set mode for PE0 to PE7
	PE->PMD &= (~(0xFFFF<< 0));		//clear PMD[15:0] 
	PE->PMD |= 0b0101010101010101<<0;   //Set output push-pull for PE0 to PE7
	
	//Select the first digit U11
	PC->DOUT |= (1<<7);     //Logic 1 to turn on the digit
	PC->DOUT &= ~(1<<6);		//SC3
	PC->DOUT &= ~(1<<5);		//SC2
	PC->DOUT &= ~(1<<4);		//SC1		

    while(1)
	{
		//Turn all Rows to LOW 
		PA->DOUT &= ~(1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT &= ~(1<<5);
		if(C1_pressed) 
			{
				search_col1();
			}
		else if(C2_pressed) 
			{
				search_col2();
			}	
		else if(C3_pressed) 
			{
				search_col3();
			}
		else
        {
			
        }		
	}
}		
//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------------------			
