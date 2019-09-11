/*
 * LCD_I2C.h
 *
 * Created: 05.09.2019 22:36:52
 *  Author: Derokero
 */ 

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

// Includes
#include "SERIAL.h"

#ifndef SERIAL_H_
#error SERIAL.h was not included!
#endif

// Function prototypes
void initLCD();
void sendToLCD(uint8_t cmd, uint8_t RS, uint8_t R_W);
void LCD_Write(char *str);
void LCD_Write_Char(char Chr);
void delaySec();

// Initialize LCD
void initLCD(){
	// D7|D6|D5|D4|BKLGHT|E|RW|RS
	sendByte(0b00101100);	// Set 4 bit interface
	sendByte(0b00101000);
}

// Send command to LCD
void sendToLCD(uint8_t cmd, uint8_t RS, uint8_t R_W){
	
	// D7|D6|D5|D4|BKLGHT|E|RW|RS
	uint8_t cmdHigh = cmd & 0xf0;
	uint8_t cmdLow = cmd << 4;		// Make sure the nibble is MSB
	
	cmd = cmdHigh | (1<<3) | (1<<2) | (R_W<<1) | (RS<<0);		// Insert cmd high nibble, BlackLight enable, E enable, and RS and R_W bits
	sendByte(cmd);
	sendByte((cmd &= ~(1<<2)));	// Clear E enable bit
	
	cmd = cmdLow | (1<<3) | (1<<2) | (R_W<<1) | (RS<<0);		// Insert cmd low nibble ....
	sendByte(cmd);
	sendByte((cmd &= ~(1<<2)));	// Clear E enable bit
	
	delayus(50);
}

// Write string to LCD
void LCD_Write(char *str){
	while(*str != 0){
		LCD_Write_Char(*str++);
	}
}

// Write char to LCD
void LCD_Write_Char(char Chr){
	sendToLCD(Chr, 1, 0);
}
// Delay of 1 second
void delaySec(){
	volatile uint16_t c = 5000;
	while(c--){
		delayus(200);
	}
}

#endif /* LCD_I2C_H_ */