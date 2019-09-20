/*
 * TEST.c
 *
 * Created: 22.08.2019 14:53:40
 *  Author: Derokero
 */ 


#define F_CPU 1000000	// 1MHz
#include <avr/io.h>
#include "LCD_I2C.h"

uint8_t debounce(uint8_t PBx);
char *intToStr(uint16_t num);
void genChar(uint8_t *customChar, uint8_t index);

int main(){
	
	// Set PB2 to input, enable internal pullup
	MCUCR &= ~(1<<PUD);
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	
	// ADC 
	ADMUX |= (0b0011);	// Select ADC3 (PB3)
	ADCSRA |= (0b010);	// Select prescaler of 4
	ADCSRA |= (1<<ADATE);	// Enable ADC auto trigger
	ADCSRA |= (1<<ADEN);	// Enable ADC
	ADCSRA |= (1<<ADSC);	// Start conversion	
	

	
	START();
	sendAddress(0x27, 0);
	
	initLCD();
	sendToLCD(0b00101000, 0, 0);	// Display and font
	sendToLCD(0b00001100, 0, 0);	// Display Control: Turn on display
	sendToLCD(0b00000001, 0, 0);	// Clear display
	sendToLCD(0b00000110, 0, 0);	// Entry Mode: Increment
	sendToLCD(0b01000000, 0, 0);	// Set CGRAM address to 0
	
	
	// Custom character testing
	uint8_t cChar0[] = {
		0b01110,
		0b01010,
		0b01110,
		0b10100,
		0b11111,
		0b00101,
		0b01010,
		0b10001
	};
	
	uint8_t cChar1[] = {
		0b01110,
		0b01010,
		0b01110,
		0b00101,
		0b11111,
		0b10100,
		0b01010,
		0b10001
	};

	uint8_t cChar2[] = {
		0b01110,
		0b01010,
		0b01110,
		0b10100,
		0b01111,
		0b00101,
		0b01010,
		0b10010
	};
	
	uint8_t cChar3[] = {
		0b01110,
		0b01010,
		0b01110,
		0b00101,
		0b11111,
		0b10100,
		0b01010,
		0b01001
	};
	
	genChar(cChar0, 0);
	genChar(cChar1, 1);
	genChar(cChar2, 2);
	genChar(cChar3, 3);
	
	
	while(1){
		sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
		LCD_Write_Char(0);
		sendToLCD(0b10000010, 0, 0);	// Set DDRAM to 0x02
		LCD_Write_Char(3);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
		sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
		LCD_Write_Char(1);
		sendToLCD(0b10000010, 0, 0);	// Set DDRAM to 0x02
		LCD_Write_Char(2);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
		delayus(UINT16_MAX);
	}

	// End of custom character testing
	
// 	while(1){
// 		uint16_t ADCVal = (uint16_t)ADCL | (uint16_t)ADCH << 8;	// Combine 2 bytes to allow a 10 bit value (Make sure to read ADCL and then ADCH, otherwise the ADC data register won't be updated)
// 		LCD_Write(intToStr(ADCVal));
// 		delaySec();
// 		sendToLCD(0b00000001, 0, 0);	// Clear display
// 	}
	
// 	uint8_t counter = 0, block = 0;
// 	
// 	while(1){
// 		if(!debounce(PB2) && !block){
// 			block = 1;
// 			LCD_Write(intToStr(counter++));
// 			sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
// 		}
// 		else if(debounce(PB2) && block){
// 			block = 0;
// 		}	
// 	}
		
	STOP();
}

// Character generation
void genChar(uint8_t *customChar, uint8_t index){
	sendToLCD(0b01000000 | (index << 3), 0, 0);	// Set CGRAM address (character index starts from bit 3)
	
	for(uint8_t i = 0; i < 8; i++){
		sendToLCD(*customChar++, 1, 0);		// Insert custom pattern
	}
}

// Integer (uint16_t) to string
char *intToStr(uint16_t num){
	static char str[10];	// Define string buffer
	volatile uint16_t count = 0, temp = num;
	
	// Count number of digits (using do-while because number of digits can't be 0)
	do{
		temp /= 10;
		count++;
	}while(temp != 0);
	
	// Convert to string
	for(uint8_t i = 0; i < count; i++){
		str[(count-1) - i] = (num % 10) + '0';	// Get most significant digit and convert to char
		num /= 10;
	}
	
	str[count] = '\0';		// Add a null terminator at the end (removes the need to clear the static array each conversion)
	return str;		// Return string
}

// Debounce using software delay (counter)
uint8_t debounce(uint8_t PBx){
	uint16_t FilterHigh = 0, FilterLow = 0;		// Counters
	
	while(1){
		// Check if high
		if((PINB & (1<<PBx))){
			FilterLow = 0;		// Reset low counter
			FilterHigh++;
			
			if(FilterHigh >= 2500)	// Threshold
				return 1;		// Input should be high
		}
		
		// Check if low
		if(!(PINB & (1<<PBx))){
			FilterHigh = 0;		// Reset high counter
			FilterLow++;
			
			if(FilterLow >= 2500)	// Threshold
				return 0;		// Input should be low
		}		
	}
}

/*
uint8_t debounce(uint8_t PBx){
	volatile uint8_t Filter;
	for(uint8_t i = 0; i < 2; i++){
		
		Filter = 1;
		
		while(Filter < 0xff && Filter > 0x00){
			Filter <<= 1;
			Filter |= (PINB & (1<<PBx)) >> PBx;
		}	
		
	}

	return Filter;
}
*/
