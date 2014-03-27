//DEFENSE

//Author: Matthew Cochrane

#include <avr/io.h>
#include <avr/interrupt.h>

#define CCWlim (PIND & (1 << PD0)) // Limit sensor input from board
#define CWlim (PIND & (1 << PD1))
#define ENABLE PD2	
#define THROW PD3	// Throw, speed 0/1, are not used in defense
#define Speed0 PD4
#define Speed1 PD5
#define CLK PD6
#define DIRECTION PD7

volatile int Step = 0;
volatile int Steplim = 0;

int Timer(int Count);
void Calibrate(int dir);
int Position(void);

ISR(TIMER0_COMPA_vect){ // Goes to limit, forever
	
	PORTD ^= (1 << CLK); // Toggles CLK pin
	
	/*Step++;	// Counts steps, until steps reaches a set limit
	if (Step >= (2*Steplim)) // Step limit here needs to be twice the set value bc there is two pulses to move once
	{
		Timer(0); // Turns off timer
		Step = 0; // Resets values of step and step limit in case they are used later
		Steplim = 0;
	}*/
		
}

int main(void)
{
	DDRD |= 0xFC; // Sets inputs and outputs. I need bits 1,0 as inputs so they are the only ones set to 0. 0xFC = 11111100
	
	PORTD &= ~(1 << ENABLE); // Sets enable low so that movement can occur
	
	sei(); // Enable global interrupts
	
	Calibrate (1); // Sets up defenders, MAYBE
	
	// Timer(200); // value inversely proportional to freq. Argument = (Period * Internal freq)/(2*Prescaler) - 1
	
	while(1) // TODO: explain
	{
		
	}
}

int Timer(int Count) // The function that activates the interrupt
{
	if (Count == 0) // Turns timer off
	{
		TCCR0B = 0;
		
		TIMSK0 = 0;
		
		OCR0A = 0;
		
		return 1;
	}
	else
	{
		TCCR0B |= (1 << WGM01); // Configure timer 0 to CTC
		
		TIMSK0 |= (1 << OCIE0A); // Enable CTC interrupt, interrupt occurs every time the value of the timer equals a set value 
		
		OCR0A = Count; // Set compare value to a compare rate, set value that is counted up to to activate interrupt7
		
		TCCR0B |= ((1 << CS02) || (1 << CS00)); // Prescale timer to 8MHz/1024
		
		return 1;
	}
}

void Calibrate(int dir) // TODO
{
	while(1) // Moves until limit is reached
	{
		PORTD |= (1 << DIRECTION); // Enables CCW motion
				
		Timer(200); // Steps defenders
			
		if (CCWlim != 0) // When CW limit is reached break the while loop
		{
			break; // break the while loop
		}
	}
	while (1)
	{
		PORTD &= ~(DIRECTION); // Enables CW motion
		
		Timer(200); // Steps defenders
		
		if (CWlim != 0) // When CCW lim is reached break the while loop
		{
			break; // break the while loop
		}
	}
		
}

/*int Position(void) // TODO: Move defender to desired positions
{	
	// TODO: move from sensor to sensor
	if(CWlim)
	{
		PORTD |= (1 << DIRECTION);
		// TODO: step 400
	}
	else if(CCWlim)
	{
		PORTD &= ~(1 << DIRECTION);
		// TODO: step
	}
	while(CWlim != 1)
	{
		
	}
}
*/
