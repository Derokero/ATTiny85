/*
 * SERIAL.h
 *
 * Created: 22.08.2019 14:39:26
 *  Author: Derokero
 */ 



#ifndef SERIAL_H_
#define SERIAL_H_

#include <avr/io.h>

typedef	unsigned char uint8_t;

// Define macros
#define SCL(x)	x ? ( DDRB &= ~(1<<PB0) ) : ( DDRB |= (1<<PB0) )	// Serial CLOCK
#define SDA(x)	x ? ( DDRB &= ~(1<<PB1) ) : ( DDRB |= (1<<PB1) )	// Serial DATA
#define DELAY 10	// Delay between sending bits

// Initialize (use GCC constructor attribute)
void __attribute__((constructor)) init(){
	PORTB &= ~((1<<PB0)|(1<<PB1));		// Clear SCL, SDA bits
	DDRB |= (1<<PB4);		// Debug LED (ACK/NACK)
} 

// Define delay microseconds (not accurate, but good enough)
void delayus(volatile uint16_t t){
	t *= F_CPU / 1000000;	// Calculate correct values according to CPU speed
	t /= 4;					// 4 is optimal to convert to micros with -O2 optimization (trial and error)
	while(t--){
		asm(" ");	// Prevent optimization 
	}
}

// Start condition: Set SDA low, while SCL is high
void START(){
	SCL(1);		// CLK high Z
	SDA(1);		// DATA high Z
	delayus(DELAY);
	SDA(0);		// DATA low
	delayus(DELAY);
	SCL(0);		// CLK low
}

// STOP condition: Set SCL high, while SDA is low
void STOP(){
	SCL(0);		// SCL low
	SDA(0);		// DATA low
	delayus(DELAY);
	SCL(1);		// CLK high Z
	delayus(DELAY);
	SDA(1);		// DATA high Z
}

// Main byte sending function
void sendByte(uint8_t byte){
	volatile uint8_t DATA = 0;
	
	for(volatile uint8_t i = 0; i < 8; i++){
		DATA = ((byte << i) & 0x80) >> 7;	/* Send byte data, bit by bit
											   (Shift to the right to get MSB, mask with 0b1000.0000, shift back to LSB) */
		SDA(DATA);		//	Output data
		delayus(DELAY);
		
		SCL(1);			// Pulse CLK
		delayus(DELAY);
		SCL(0);
		delayus(DELAY);

	}
	
	// Check acknowledge
	SDA(1);		// SDA high Z
	delayus(DELAY);
	SCL(1);	
		
	if(PINB & (1<<PB1)){	// Read DATA line, execute if NACK
		PORTB |= (1<<PB4);	// Light RED LED
	}
	SCL(0);
}

uint8_t readByte(){
	volatile uint8_t DATA = 0;
	
	for(volatile uint8_t i = 0; i < 8; i++){
		
		DATA |= (PINB & (1<<PB1)) << 6;	// Get data bit
		DATA >>= 1;		// Make space for next bit
		
		SCL(1);			// Pulse CLK
		delayus(DELAY);
		SCL(0);
		delayus(DELAY);
	}
	
	return DATA;	// Return byte, don't ACK because we want to read only 1 byte
}
// Address sending function
void sendAddress(uint8_t address, uint8_t r_w){
	address <<= 1;		// Make space for read/write bit
	address |= r_w;		// Insert read/write bit
	sendByte(address);	// Send address + read/write bit
}

#endif /* SERIAL_H_ */