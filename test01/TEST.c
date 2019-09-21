/*
 * TEST.c
 *
 * Created: 22.08.2019 14:53:40
 *  Author: Derokero
 */ 


#define F_CPU 1000000	// 1MHz
#include "LCD_I2C.h"

int main(){
	
	START();
	sendAddress(0x27, 0);
	
	initLCD();
	sendToLCD(0b00101000, 0, 0);	// Display and font
	sendToLCD(0b00001100, 0, 0);	// Display Control: Turn on display
	sendToLCD(0b00000001, 0, 0);	// Clear display
	sendToLCD(0b00000110, 0, 0);	// Entry Mode: Increment
	sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
	
	LCD_Write("Hello!");
		
	sendToLCD(0b11000000, 0, 0);	// Set DDRAM to 0x40
	
	START();
	sendAddress(0x27, 1);
	
	volatile uint8_t DATA = readByte();
	delaySec();
		
	START();
	
	sendAddress(0x27, 0);
		
	for(uint8_t i = 0; i < 8; i++){
		if((DATA << i) & 0x80)
			LCD_Write("1");
		else
			LCD_Write("0");
	}
	
	STOP();
}
