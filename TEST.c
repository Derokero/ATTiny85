/*
 * TEST.c
 *
 * Created: 22.08.2019 14:53:40
 *  Author: Derokero
 */ 


#define F_CPU 1000000

#include "SERIAL.h"
#include <avr/io.h>


void sendToLCD(uint8_t cmd, uint8_t RS, uint8_t R_W);
void LCD_Write(char *str);
void initLCD();

void delaySec(){
	volatile uint16_t c = 5000;
	while(c--){
		delayus(200);
	}
}
int main(){
	START();
	sendAddress(0x27, 0);

	initLCD();	
	sendToLCD(0b00100000, 0, 0);	// Display and font
	sendToLCD(0b00001100, 0, 0);	// Display Control: Turn on display
	sendToLCD(0b00000001, 0, 0);	// Clear display
	sendToLCD(0b00000110, 0, 0);	// Entry Mode: Increment
	sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0
	
	LCD_Write("Hello World!");
	delaySec();
	
	sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0
	
	/*sendByte(0b10000000);*/
	
	
	START();
	sendAddress(0x27, 1);
	
	volatile uint8_t DATA = readByte();
	
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

void LCD_Write(char *str){
	while(*str != 0){
		sendToLCD(*str++, 1, 0);
	}
}
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

void initLCD(){
	// D7|D6|D5|D4|BKLGHT|E|RW|RS
	sendByte(0b00101100);	// Set 4 bit interface
	sendByte(0b00101000);
}