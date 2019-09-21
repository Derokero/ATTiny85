/*
 * Program.c
 *
 * Created: 18.07.2019 17:46:05
 *  Author: Derokero
 */ 

#define F_CLK 1000000	// Define CPU clock for the timer to use
//#define PRESCALER _8_PRESCALING	// Select prescaler

#include "timer.h"

uint8_t *PORTB = (uint8_t *) 0x18 + 0x20;
uint8_t *DDRB = (uint8_t *) 0x17 + 0x20;
	
void toggle(){
	*PORTB ^= (1<<0);	// Toggle
}

int main(){
	*DDRB |= (1<<0);	// Output
	
	while(1){
		timer(1, toggle);	// timer(seconds, function)
	}
}