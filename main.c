//DEFENSE

//Author: Matthew Cochrane

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

// I/O Definitions
#define CCWlim (PIND & (1 << PD0)) // Limit sensor input from board
#define CWlim (PIND & (1 << PD1))
#define ENABLE PD2	
#define THROW PD3	// Throw, speed 0/1, are not used in defense
#define Speed0 PD4
#define Speed1 PD5
#define CLK PD6
#define DIRECTION PD7

#define Control PORTD

// State Definitions
#define CW 0
#define CCW 1
#define BUSY 1
#define DONE 0

// Global Variables
unsigned int TurnStepCount; // unsigned to allow more information
unsigned char TurnInProgress, DirectionOfStep; // Indicators of movement

// Function Prototypes
void AbortTurn(void);

void Calibrate();

unsigned char Move(short NumberOfSteps, char MovingDirection);


int main(void)
{
	DDRD |= 0xFC; // Sets inputs and outputs. I need bits 1,0 as inputs so they are the only ones set to 0. 0xFC = 11111100
	
	TurnInProgress = DONE; // Lets program know there is no turn happening right now
	
	sei(); // Enable global interrupts
	
	Calibrate (); // Sets up defenders, MAYBE

	while(1) // Loops forever
	{
		
	}
}

void Calibrate()
{
	do 
	{
		while(Move(100, CW) == 1);
	} while (CWlim == 0);
	
	do 
	{
		while(Move(100,CCW) == 1);
	} while (CCWlim == 0);
}

unsigned char Move(short NumberOfSteps, char MovingDirection)
{
	if (TurnInProgress == 1) // Don't move if already moving
	{
		return 1;
	}
	
	DirectionOfStep = MovingDirection; // Save direction of defender to global direction indicator
	
	if (MovingDirection == 1)
	{
		Control = (1 << DIRECTION);
	}
	else {
		Control &= ~(1 << DIRECTION);
	}
	
	TurnStepCount = NumberOfSteps << 1;
	
	TurnInProgress = BUSY;
	
	TCCR0A = (1 << WGM01) | (1 << COM0A0); // Sets CTC mode, toggles CLK when 
	
	TCCR0B = (1 << CS02); // Prescale to 8MHz/256
	
	OCR0A = 4;	
	
	TCNT0 = 0; // Sets timer to zero
	
	Control &= ~(ENABLE); // Enables movement
	
	TIMSK0 = (1 << OCIE0A); // Enables interrupts, they take over
	
	return 0;
}
void AbortTurn(void)
{
	TIMSK0 &= ~(1 << OCIE0A); // Disable interrupts
	
	TurnStepCount = 0; // Clear the step count
	
	TCCR0B = 0; // Turn off timer
	
	Control |= (1 << ENABLE); // Disable movement
	
	TurnInProgress = DONE; // Tells program there is no turn
}

ISR(TIMER0_COMPA_vect)// Goes to limit, forever
{ 
	if ((CWlim != 0 && DirectionOfStep == CW) || (CCWlim != 0 && DirectionOfStep ==CCW) || --TurnStepCount == 0)
	{
		TCCR0B = 0; // Disable timer until further notice
		TIMSK0 &= ~(1 << OCIE0A); // Disable interrupts
		Control |= (1 << ENABLE);
		TurnInProgress = DONE;
	}
}
