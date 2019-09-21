/*
 * ATTiny85.c
 *
 * Created: 13.07.2019 21:01:54
 * Author : Derokero
 */ 

//#define F_CPU 1000000UL

//#include <avr/io.h>
//#include <util/delay.h>


int main(void)
{
	
	/*
	unsigned char *TCCR0B = (unsigned char *) 0x53;
	unsigned char *GTCCR = (unsigned char *) 0x4C;
	unsigned char *TIFR = (unsigned char *) 0x58;
	unsigned char *TCNT0 = (unsigned char *) 0x52;
	*/
	
	//PORTB |= (1<<0);
	//DDRB |= (1<<0);
	
	/*
		The ATTiny85 features 64 I/O registers, all of which are offset by 0x20 due to memory structure. 
		Hence, to access them, we need to offset the addresses by 0x20 before addressing the registers themselves.
		This is done easily by adding 0x20 to the register's address.
	*/
	
	unsigned char *PORTB = (unsigned char *) 0x18 + 0x20; // Define pointer for PORTB, cast integer to char pointer type, add 0x20 offset
	unsigned char *DDRB = (unsigned char *) 0x17 + 0x20; // Define pointer for DDRB, cast, add offset
	/*
		Accessing the registers directly by using their addresses here.
		The value setting is done using read-modify-write access, which reads, modifies, and writes the new value back.
	*/
	*PORTB &= ~(1<<0); // Set pin0 low  (use read-modify-write access)
	*DDRB |= (1<<0); // Set pin0 as output  (use RMW access)

	unsigned int T; // Timer overflow counter
	unsigned int lastT; 
	
	/*
	*GTCCR |= (1<<7)|(1<<0); // Enter timer synchronization mode, reset prescaler (halt)
	*TCCR0B |= (1<<1); // clk/8 prescaling
	*TCNT0 = 255 - 123; // Set appropriate interval
	*GTCCR &= ~(1<<7); // Start counting
	*/
	
	// Direct address access of memory
	*(volatile unsigned char *) 0x4C |= (1<<7)|(1<<0); // GTCCR
	*(volatile unsigned char *) 0x53 |= (1<<1); // TCCR0B
	*(volatile unsigned char *) 0x52 |= 255 - 124; // TCNT0
	*(volatile unsigned char *) 0x4C &= ~(1<<7); // GTCCR
	
	while (1)
	{
		//PORTB ^= (1<<0);
		//_delay_ms(1000);
		if( ((*(volatile unsigned char *) 0x58) & (1<<1)) == (1<<1) ){ // Check for TOV0 (timer overflow) flag
			T++; // Count overflows
			//*TIFR |= (1<<1); // Clear overflow flag
			*(volatile unsigned char *) 0x58 |= (1<<1);
			*(volatile unsigned char *) 0x52 |= 255 - 124;
		}
		
		if(T - lastT >= 1000){
			*PORTB ^= (1<<0); // XOR contents using RMW access to toggle bit 0 (pin0)
			lastT = T;
		}
		
		//*PORTB ^= (1<<0); // XOR contents using RMW access to toggle bit 0 (pin0)
		
		//for(volatile unsigned char i = 0; i < 2; i++){
			//for(volatile unsigned char j = 0; j < 200; j++){
				//for(volatile unsigned char k = 0; k < 250; k++){
					///*	
						//This is the delay function.
						//TODO: Find a reliable way to calculate the delay generated, even though it's a shit way of accurately generating delays
					//*/
				//}
			//}
		//}
		
	}
	
}

