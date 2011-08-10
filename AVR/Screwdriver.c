/*
 * Screwdriver.c
 *  Author: Nathaniel Chapman
 *	This is the Code to load a basic sonic screwdriver optical communication device on an ATTiny13 based LED device
 *	LED should be on pin 3 (PB4)
 *	PIN7 is a speaker (ADC1 used for DAC)
 *  PIN6 loads the speaker pin.
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>    // Defines pins, ports, etc to make programs easier to read
#define F_CPU 960000UL	      // Sets up the default speed for delay.h
#define TRANSMIT_TIME 45		  //3 ms, that's 45 timer cycles at clk/64
#include <util/delay.h>

/* Global Variables */

//TODO: move this to EEPROM for separation of code and data!
//Packet Format 0xAA sync byte |4 byte secret code | 0x00 clear byte
static unsigned char* code[6] = {0xAA, 0xDE, 0xAD, 0xBE, 0xEF, 0x00};

/* Custom interrupt flags */
#define TRANSMIT 0x01 
#define BOOL2 0x02 //Unused VV
#define BOOL3 0x04 
#define BOOL4 0x08 
#define BOOL5 0x10 
#define BOOL6 0x20 
#define BOOL7 0x40 
#define BOOL8 0x80 

unsigned char flags; 

//timer 0 match interrupt
ISR(TIM0_COMPA_vect){
	PORTB ^= _BV(PB4);
	//flags |= TRANSMIT;
}

int main(){
	/* INIT CODE */
	cli();

	/*Timer 0 init
	* output should change every 3 ms
	* that's 112.5 timer cycles at clk/256
	*/
  	TCCR0A = _BV(WGM01); 	//Timer 0 set to CTC mode
  	TCCR0B = _BV(CS01) | _BV(CS00); 	//Timer on, 64 prescaler
	OCR0A = TRANSMIT_TIME;		//Match 3ms cycle
  	TIMSK0 = _BV(OCIE0A); 	//Enable Match A interrupt

	DDRB = _BV(PB4);		      // enable output on port B, pin 4

	//clear custom interrupt flag
	flags = 0x00;
  
  	sei(); //enable interrupts

	/* LOOP */
	while(1){
		//let ints do their thing...

		//handle the transmit timer interrupt
		if (flags & TRANSMIT) {
			//TODO: send next byte here!
			PORTB ^= _BV(PB4);
			flags &= ~TRANSMIT;
		} 
    }
}
