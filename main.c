//Main c program for offense
//Matthew Page
//Started on March 1, 2014


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


// Offense or Defense

#define TEAM (PINB & (1<<PB0)) //If PB0 is 0, TEAM == 0. If PB0 is 1, TEAM != 0. HOWEVER, TEAM is never == 1.

//OUTPUTS

#define ENABLE  PD2                            //Enables signal to board
#define THROW   PD3                            //Activate solenoid
#define BUMP0   PD4                            //First bumper control
#define BUMP1   PD5                            //Second bumper control
#define STEP    PD6                            //Clk signal to stepper, only pin possible
#define DIRECT  PD7                            //Sets direction, 0=CCW; 1=CW

//INPUTS
#define SenseCCW (PIND & (1 << PD0))    // Counter clockwise limit sensor from driver board.
#define SenseCW (PIND & (1 << PD1))     // Clockwise limit sensor from driver board.
#define Play1 (PINC & (1 << PC1))        //Play selector for pin c 1
#define Play2 (PINC & (1 << PC2))        //Play selector for pin c 2


#define CCW  0
#define CW 1

//define variables
volatile int step_count;
unsigned short int num_step;
unsigned char turn;
unsigned int steps_taken;


//initialize functions
void Steps(unsigned int Steps, unsigned char Dir);
void Delay(unsigned int Delay);

ISR (TIMER0_COMPA_vect){
    step_count++;    //increment the counter

}

void aim1(void){
    Steps(53, CCW);
    return;
}

void aim2(void){
    Steps(70, CW);
    return;
}

void aim3(void){

    Steps(80, CCW);
    return;
}

void unaim1(void){
    Steps(53, CW);
    return;
}

void unaim2(void){
    Steps(70, CCW);
    return;
}

void unaim3(void){

    Steps(80, CW);
    return;
}

void calibrateR(void){        //calibrates the right hand side
    
    while(SenseCW == 0){
        Steps(1, CCW);
    }
    return;
}
void calibrateL(void){        //calibrates the lefthand side
    
    steps_taken = 0;
    
    while(SenseCCW == 0){
        Steps(1, CW);
        steps_taken++;
    }
    
    return;
    
}

void centre(unsigned int middle){
    
    middle = steps_taken/2;
    
    Steps(middle, CCW);
    return;
}

void Steps(unsigned int Steps, unsigned char Dir){
    step_count = 0;
    if(Dir == CW){
        PORTD |= (CW<<DIRECT);
        } else if(Dir == CCW){
        PORTD |= (CCW<<DIRECT);
    }
    TCCR0A |= (1<<WGM01) | (1<<COM0A0);     //Set CTC and output toggle
    TCCR0B |= (1<<CS02) |(1<<CS00);        //1024 prescale
    OCR0A = 20;                           //~5ms pulse
    TCNT0 = 0;      // Reset timer, not sure why I would have to do this in CTC, but the examples have it...
    TIMSK0 |= (1<<OCIE0A);                 //Set ISR compare vector
    sei();                                //Enable interrupts

    while(step_count < Steps){}
    return;

}


void d_calibrateR(void){        //calibrates the right hand side
    
    while(SenseCW == 0){
        Steps(1, CW);
    }
    return;
}
void d_calibrateL(void){        //calibrates the lefthand side
    
    steps_taken = 0;
    
    while(SenseCCW == 0){
        Steps(1, CCW);
        steps_taken++;
    }
    
    return;
    
}

void d_centre(unsigned int middle){  // defense blocking
    
    middle = steps_taken/2;
    
    Steps(middle, CCW);
    return;
}

void block1(void){
    Steps(6000, CW);
    return;
}

void unblock1(void){
    Steps(6000,CCW);
    return;
}

void block2(void){
    Steps(8000, CCW);
    return;
}

void unblock2(void){
    Steps(8000, CW);
    return;
}

void Delay(unsigned int Delay){      //can change to char later when second delay not needed
    int i;
    for(i = 0; i < Delay; i++) {
        TCCR0A = 0;
        TCCR0B = (1<<CS02) | (1<<CS00);        // CLK/1024, No Waveform Generation
        OCR0B = 7;                             //Approx 1mS
        TCNT0 = 0;
        TIFR0 = (1 << OCF0B);
        while ( !(TIFR0 & (1<<OCF0B)));
    }
    TCCR0B = 0;                                // Turn Timer Off
    i = 0;
}


void bumpers(void){                            //Simple turn bumpers on
    PORTD &= ~(1<<BUMP0);
    PORTD |= (1<<BUMP1);
    return;
}

void fire(void){

    Delay(1000);
    PORTD ^= (1<<THROW);
    Delay(300);
    PORTD ^= (1<<THROW);
    Delay(500);                        //Take out while loop and this line for discrete fnct
    return;
}

void play_1(void){
    calibrateR();
    Delay(500);
    calibrateL();
    Delay(500);
    centre(steps_taken);
    Delay(500);
    aim1();
    fire();
	unaim1();
    aim2();
    fire();
    aim3();
    fire();
    return;
}

/*void defense(void){
    
    d_calibrateR();
    Delay(500);
    d_calibrateL();
    Delay(500);
    d_centre(steps_taken);
    Delay(500);
    block1();
    Delay(500);
    unblock1();
    Delay(500);
    block2();
    Delay(500);
    unblock2();
    Delay(500);


    return;

}*/

int main(void){
    DDRD |= (1<<THROW) | (1<<BUMP0) | (1<<BUMP1);         //Initializing outputs
    DDRD |= (1<<ENABLE) | (1<<STEP) | (1<<DIRECT);
    DDRD &= ~(1<<PD1) | ~(1<<PD0);                        //Initialize inputs
    
    PORTD |= (1<<THROW);
	
	if (TEAM == 0)
	{
		play_1();
	}else	{
		block1();
		unblock1();
		block2();
		unblock2();
	}
    
    DDRC |= (1<<PC3);        //Setting up an output so i can choose the plays
    DDRC &= ~(1<<PC1) | ~(1<<PC2); 
    PORTC |= (1<<PC3);        //Always output a 1
    
    
do{
    play_1();
} while(1);

}


