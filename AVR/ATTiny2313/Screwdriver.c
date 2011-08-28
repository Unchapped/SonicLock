/*
 * Screwdriver.c
 *  Author: Nathaniel Chapman
 *	This is the Code to load a basic sonic screwdriver optical communication device on an ATTiny13 based LED device
 *	Settings for ATTiny2313:
 *	LED should be on pin 12 (PB0)
 *	PIN7 is a speaker (ADC1 used for DAC)
 *  PIN6 loads the speaker pin.
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>    // Defines pins, ports, etc to make programs easier to read


//Set transmit speed and packet info
#define BIT_LENGTH_MILLIS 10 // in milliseconds
#define PACKET_LENGTH 6

//TODO: Use definition math to calculate TRANSMIT TIME

//Supported devices: ATTiny13, ATTiny2313, ATTiny24

//=================================ATTINY13 defs
#if defined (__AVR_ATtiny13__)

//LED output pin PB4
#	define LED_OUT_PORT PORTB
#	define LED_OUT_DIR DDRB
#	define LED_OUT_PIN _BV(PB4)

//Use 9.6 Mhz internal RC Oscillator
#	define TIMER_CYCLES_SECOND 15000 	//Timer Cycles per second
#	define TRANSMIT_TIME 150

//"bugfixes"
#	define TIMSK TIMSK0 //don't know why this register is named differently
#	define TIMER0_OVF_vect TIM0_OVF_vect //again, this is stupid
//================================ATTINY24 defs
//#elif defined (__AVR_ATtiny24__)
//#	define TIMSK TIMSK0 //don't know why this register is named differently
//#	define	TIM0_COMPA_vect TIMER0_COMPA_vect

//================================ATTINY2313 defs
#elif defined (__AVR_ATtiny2313__)

//LED output pin 11 (PD6)
#	define LED_OUT_PORT PORTD
#	define LED_OUT_DIR DDRD
#	define LED_OUT_PIN _BV(PD6)

//use 10 MHz external Oscillator
#	define TIMER_CYCLES_SECOND 15625 	//Timer Cycles per second
#	define TRANSMIT_TIME 156

//unsupported device
#else
#	error "Unsupported Device!"
#endif


/* Global Variables */

//TODO: move this to EEPROM for separation of code and data!
//Packet Format 0xAA sync byte |4 byte secret code | 0xFF clear byte
unsigned char code[PACKET_LENGTH] = {0xAA, 0xDE, 0xAD, 0xBE, 0xEF, 0xFF};

//TCNT reset value
const unsigned char tcnt = 6; //256 - TRANSMIT_TIME;


/* Custom interrupt flags */
#define TRANSMIT 0x01 
#define BOOL2 0x02 //Unused VV
#define BOOL3 0x04 
#define BOOL4 0x08 
#define BOOL5 0x10 
#define BOOL6 0x20 
#define BOOL7 0x40 
#define BOOL8 0x80 

volatile unsigned char flags; 

// current transmitted bit index 
unsigned char index;
volatile unsigned char nextbit;

//timer 0 overflow interrupt
ISR(TIMER0_OVF_vect){
	//reset counter
	TCNT0 = tcnt;

	//update transmit bit in constant time
	LED_OUT_PORT = (LED_OUT_PORT & ~LED_OUT_PIN) | nextbit;

	//set the flag to take care of the logic
	flags |= TRANSMIT;
}

int main(){
	/* INIT CODE */
	cli(); //disable interrupts

	flags = 0x00; //clear custom interrupt flag

	//TODO: load custom pen codes from EEPROM
	//need to add init while loop to handle EEPROM interrupts

	/*
	*Timer 0 init
	* This timer triggers the Output LED to move on to the next bit
	* Luckily this configuration is the same for all supported devices
	*/
	//leave Timer 0 in normal mode
	TCCR0A = 0x00;
	//TCCR0A = _BV(WGM01); 	//Timer 0 set to CTC mode
  	TCCR0B = _BV(CS01) | _BV(CS00); //Timer on, 64 prescaler
	//TCCR0B = _BV(CS02) | _BV(CS00); //Timer on, 1024 prescaler FOR TESTING
	//OCR0A = TRANSMIT_TIME;			//Match 10ms cycle
	TIMSK |= _BV(TOIE0); 			//Enable Overflow interrupt

	
	/* bit transmission setup */
	LED_OUT_DIR |= LED_OUT_PIN;		// enable output on port B, pin 4
	index = 0;						//start transmission at address 0
	nextbit = 0x00;					//clear the next bit variable
  
  	sei(); //enable interrupts

	/*MAIN LOOP */
	while(1){
		//let ints do their thing...

		//handle the transmit timer interrupt
		if (flags & TRANSMIT) {
			
			//increment index
			index = (index + 1) % (PACKET_LENGTH * 8);
			nextbit = 0x00;

			if((code[index / 8] >> (index % 8)) & 0x01)
				nextbit = LED_OUT_PIN;

			//clear the interrupt flag...
			flags &= ~TRANSMIT;
		} 
    }
}
