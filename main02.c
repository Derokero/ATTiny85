/*
 * ATTiny85 Test.c
 *
 * Created: 28.07.2019 16:44:00
 * Author : Derokero
 */ 

#include <avr/io.h>

#define GINTR_SET	SREG |= (1<<7)	// Global interrupt enable
#define GINTR_CLR	SREG &= ~(1<<7)	// Global interrupt disable
#define COMP_VAL	200				// CTC compare value

volatile uint16_t Count, Count1, Count2;

int main(void){
	
	
	PORTB |= (1<<PB0)|(1<<PB1);	// Set outputs high
	DDRB |= (1<<PB0)|(1<<PB1);	// Set pin0 & pin1 as outputs
	
	GTCCR |= (1<<TSM)|(1<<PSR0);	// Pause timer and reset prescaler
	TCCR0A |= (1<<WGM01);			// Set timer to CTC mode
	TCCR0B |= (1<<CS00);			// Set prescaler (f_clk/1)
	TCNT0 = 0;						// Make sure it starts at 0
	OCR0A = COMP_VAL - 1;			// Set compare value (-1 because timer takes one more cycle to clear)
	TIMSK |= (1<<OCIE0A);			// Enable output compare match interrupt
	GINTR_SET;						// Global interrupt enable
	GTCCR &= ~(1<<TSM);				// Start timer	
	
	
	volatile uint16_t lastCount, lastCount1, lastCount2;
	
    while (1) {
		
		/*
		This documents the importance of using == instead of >= for accurate, bug free comparisons (depending on the situation ofcourse)
		Apparently the optimizer is not smart (figures), and when using >= it uses a different assembly instruction than when using ==
		>= generates Branch If Carry Set instruction, because it assumes (I think), that the subtraction is going to set the Carry flag.
		== generates Branch If Not Equal instruction, which is literally checking if the value is equal to the other value (using subtraction),
		Hence >= causes bugs and == doesn't
		*/	
		
		
		if(Count - lastCount == 5000/4){
			lastCount = Count;
			PORTB ^= (1<<PB0);
			if(Count2 % 4 == 0){
				PORTB ^= (1<<PB1);
			}
			Count2++;
		}

// 		if(Count - lastCount1 == 5000/8){
// 			lastCount1 = Count;
// 			PORTB ^= (1<<PB0);
// 		}
// 		
// 		if(Count - lastCount2 == 5000/8){
// 			lastCount2 = Count;
// 			PORTB ^= (1<<PB1);
// 		}
    }
}

void __vector_10() __attribute__((interrupt));
void __vector_10(){
	Count++;
}