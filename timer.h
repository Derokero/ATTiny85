/*
 * timer.h
 *
 * Created: 18.07.2019 19:00:01
 *  Author: Derokero
 */ 


#ifndef TIMER_H_ /* TIMER_H */
#define TIMER_H_

// Make sure F_CLK is defined
#ifndef F_CLK
#define F_CLK 1
#error F_CLK not defined!
#endif

// Prescaling options
#define _NO_PRESCALING		1	// (0<<2)|(0<<1)|(1<<0)
#define _8_PRESCALING		2	// (0<<2)|(1<<1)|(0<<0)
#define _64_PRESCALING		3	// (0<<2)|(1<<1)|(1<<0)
#define _256_PRESCALING		4	// (1<<2)|(0<<1)|(0<<0)
#define _1024_PRESCALING	5	// (1<<2)|(0<<1)|(1<<0)

#ifndef PRESCALER
	#define PRESCALER _NO_PRESCALING // Default prescaling
#endif

// Select division based on prescaling
#ifdef PRESCALER 
	#if PRESCALER == _NO_PRESCALING
		#define _DIV 1
	#elif PRESCALER == _8_PRESCALING
		#define _DIV 8
	#elif PRESCALER == _64_PRESCALING
		#define _DIV 64
	#elif PRESCALER == _256_PRESCALING
		#define _DIV 256
	#elif PRESCALER == _1024_PRESCALING
		#define _DIV 1024
	#endif
#endif

#define TIMER_OVF_COUNT (uint32_t)(F_CLK / _DIV / 256)	// Calculate amount of overflows until target time
#define TIMER_REMAINDER ((((float)F_CLK / (float)_DIV / (float)256) - (F_CLK / _DIV / 256)) * 256)	// Calculate remained until target time

#define offset 0x20 // I/O registers are offset by 0x20

// Type definitions
typedef unsigned char uint8_t;
typedef unsigned long uint32_t;

static volatile uint32_t ovf_count;
static volatile uint32_t curtime;

// Initialization

// Declare and I/O registers and their memory locations
volatile uint8_t *TCNT0 = (uint8_t *)(0x32 + offset);	// Timer/Counter Register (used by ISR)

void init(void) __attribute__ ((constructor));	// Execute 'init' before 'main' automatically ('constructor' (GCC) compiler attribute)
void init(void){
	
	volatile uint8_t *GTCCR = (uint8_t *)(0x2C + offset);	// General Timer/Counter Control Register
	volatile uint8_t *TCCR0B = (uint8_t *)(0x33 + offset);	// Timer/Counter Control Register B
	volatile uint8_t *TIMSK = (uint8_t *)(0x39 + offset);	// Timer/Counter Interrupt Mask Register
	volatile uint8_t *SREG = (uint8_t *)(0x3F + offset);	// Status register
	//volatile uint8_t *TIFR = (uint8_t *)(0x38 + offset);	// Timer/Counter Interrupt Flag Register
	
	*GTCCR |= (1<<7)|(1<<0);	// Enter synchronization mode and halt timer/counter0
	*TCCR0B |= PRESCALER;		// Set prescaler
	*TCNT0 = (uint8_t)(256 - TIMER_REMAINDER);	// Set remainder 
	
	*SREG |= (1<<7);	// Global interrupt enable
	*TIMSK |= (1<<1);	// Enable overflow interrupt
	
	*GTCCR &= ~(1<<7);	// Start timer
	
}

// Interrupt (TIMER/COUNTER0 OVERFLOW)
void __vector_5(void) __attribute__((interrupt));
void __vector_5(void){
	
	// Count overflows
	
	if(ovf_count++ == TIMER_OVF_COUNT){
		*TCNT0 += (uint8_t)(256 - TIMER_REMAINDER); // Remainder ticks
		ovf_count = 0; // Reset
		curtime++; // Main time variable
	}
	
}

// Timer function
void timer(uint32_t t, void (*funct)(void)){	// Takes 2 arguments, time(seconds) and a function
	static uint32_t lastTime;	// Keep variable alive between calls
	
	if(curtime - lastTime == t){	// Check for time
		lastTime = curtime;	// Update time
		funct();	// Call argument function
	}
}

#endif /* TIMER_H_ */