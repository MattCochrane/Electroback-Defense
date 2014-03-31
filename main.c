
// basic laser, put stuff where it needs to go
//global vars
int cwlaser;
int ccwlaser;

//main
 PCICR |= (1<<PCIE0);   			 // vector PCINT0 enabled

 PCMSK0 |= (1<<PDINT4)|(1<<PDINT5);    	 // enables PD4,PD5 interrupts

 sei();


//isr
ISR(PCINT0_vect){

 
 if(BUMP0 == 0){   		 // use the same pin because they arent used in defense

 cwlaser = 1;   			 // variable that tells what direction to turn
 PCMSK0 &= ~(1<<PDINT5);    	 //disables the other laser from interrupting    
PCMSK0 &= ~(1<<PDINT4);
return;    
 }
 
 if(BUMP1 == 0){

 ccwlaser = 1;   			 // variable that tells what direction to turn
 PCMSK0 &= ~(1<<PDINT4);   	 // disables other laser    
PCMSK0 &= ~(1<<PDINT5);
 }
}
//defense function or main or whatever







while(1){

if(cwlaser == 1){
 Steps(53, CW);
 Delay (500);
 Steps(53, CCW);
 cwlaser=0;
if(ccwlaser == 1){
Steps(53, CCW);
 Delay (500);
 Steps(53, CW);
 ccwlaser = 0;
}



}
