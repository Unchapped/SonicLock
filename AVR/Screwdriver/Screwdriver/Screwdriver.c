/*
 * Screwdriver.c
 *  Author: Nathaniel Chapman
 *	This is the Code to load a basic sonic screwdriver optical communication device on an ATTiny13 based LED device
 *	LED should be on pin 1 (PB5)
 *	PIN7 is a speaker (ADC1 used for DAC)
 *  PIN6 loads the speaker pin.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>    // Defines pins, ports, etc to make programs easier to read
#define F_CPU 98000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>

int main(){
	//init code
	DDRB = _BV(PB5);		      // enable output on port B, pin 5
  
	//Packet Format 0xAA sync byte | 4 byte secret code | 0x00 clear byte
	
	while(1){

    PORTB = _BV(PB5); 
    _delay_ms(1000);

    PORTB &= ~_BV(PB5);	
	_delay_ms(1000);
    
	//TODO:: Please write your application code 
	
    }
}