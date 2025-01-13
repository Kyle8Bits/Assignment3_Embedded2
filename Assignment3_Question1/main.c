//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"

void System_Config(void);
void UART0_Config(void);
void UART0_SendChar(int ch);
char UART0_GetChar();


int main(void)
{
	System_Config();
	UART0_Config();
	GPIO_SetMode(PC,BIT12,GPIO_MODE_OUTPUT);

	while(1){
	}
}

void System_Config (void){
	SYS_UnlockReg(); // Unlock protected registers

	// enable clock sources
	CLK->PWRCON |= (0x01 << 0);
	while(!(CLK->CLKSTATUS & (1 << 0)));

	//PLL configuration starts
	CLK->PLLCON &= ~(1<<19); //0: PLL input is HXT
	CLK->PLLCON &= ~(1<<16); //PLL in normal mode
	CLK->PLLCON &= (~(0x01FF << 0));
	CLK->PLLCON |= 48;
	CLK->PLLCON &= ~(1<<18); //0: enable PLLOUT
	while(!(CLK->CLKSTATUS & (0x01 << 2)));
	//PLL configuration ends

	// CPU clock source selection
	CLK->CLKSEL0 &= (~(0x07 << 0));
	CLK->CLKSEL0 |= (0x02 << 0);
	//clock frequency division
	CLK->CLKDIV &= (~0x0F << 0);

	//UART0 Clock selection and configuration
	CLK->CLKSEL1 |= (0x03 << 24); // UART0 clock source is 22.1184 MHz
	CLK->CLKDIV &= ~(0x0F << 8); // clock divider is 1
	CLK->APBCLK |= (0x01 << 16); // enable UART0 clock

	SYS_LockReg();  // Lock protected registers
}

void UART0_Config(void){
    PB->PMD &= ~(0b11 << 2); //Port B 1 for UART
    PB->PMD |= (0b01 << 2); //PB.1 is output pin
    SYS->GPB_MFP |= (1 << 1)|(1<<0);

    // UART0 operation configuration
    UART0->LCR |= (0b11 << 0); // 8 data bit
    UART0->LCR &= ~(1 << 2); // ONE stop bit
    UART0->LCR &= ~(1 << 3); // NO parity bit
    UART0->FCR |= (1 << 1); // clear RX FIFO
    UART0->FCR |= (1 << 2); // clear TX FIFO
    UART0->FCR &= ~(0xF << 16); // FIFO Trigger Level is 1 byte]


    UART0->BAUD &= ~(0b11 << 28); //Baud rate devider = 115200 bps
    UART0->BAUD &= ~(0xFFFF << 0);
    UART0->BAUD |= 10;

    UART0->IER |= (1 << 0);
    
	NVIC->ISER[0]=(1<<12);

}

void UART0_SendChar(int ch){
	while(UART0->FSR & (0x01 << 23)); //wait until TX FIFO is not full
	UART0->DATA = ch;
	if(ch == '\n'){
		while(UART0->FSR & (0x01 << 23));
		UART0->DATA = '\r';
	}
}

char UART0_GetChar(){
	while(1){
		if(!(UART0->FSR & (0x01 << 14))){
			return(UART0->DATA);
		}
	}
}

void UART02_IRQHandler(void) {
    // Handle Transmit Holding Register Empty (THRE) interrupt
    if(UART0->ISR & (1<<0)){
        char data = UART0_GetChar();
        UART0_SendChar(data);
    }
}
//------------------------------------------- main.c CODE ENDS --------------------------------------------------------------------------