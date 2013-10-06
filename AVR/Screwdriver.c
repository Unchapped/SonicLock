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

//ADC Input Pin
#	define ADC_IN_PIN ADC1
#	define ADC_IN_DIR DDRB
#	define ADC_IN_PORT PORTB


//Use 9.6 Mhz internal RC Oscillator
#	define TIMER_CYCLES_SECOND 15000 	//Timer Cycles per second
#	define TRANSMIT_TIME 150			//try 189.4 ish? Pot says this should work...

//"bugfixes"
#	define TIMSK TIMSK0 //don't know why this register is named differently
#	define TIMER0_COMPA_vect TIM0_COMPA_vect //again, this is stupid
//================================ATTINY24 defs
#elif defined (__AVR_ATtiny24__)

//LED output pin PA0
#	define LED_OUT_PORT PORTA
#	define LED_OUT_DIR DDRA
#	define LED_OUT_PIN _BV(PA0)

//ADC Input Pin
#	define ADC_IN_PIN PA1
#	define ADC_IN_DIR DDRA
#	define ADC_IN_PORT PORTA

//use 10 MHz external Oscillator
#	define TIMER_CYCLES_SECOND 15625 	//Timer Cycles per second
//#	define TRANSMIT_TIME 156
#	define TRANSMIT_TIME 0xC5 //don"t know how the hell this works, but this is what the pot says...
//#	define TRANSMIT_TIME 100 //@ 1024 prescaler for 100 ms bits

//"bugfixes"
#	define TIMSK TIMSK0 //don't know why this register is named differently
#	define TIMER0_COMPA_vect TIM0_COMPA_vect //again, this is stupid

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
unsigned char code[PACKET_LENGTH] = {0xaa, 0xde, 0xad, 0xbe, 0xef, 0xff};
//unsigned char code[PACKET_LENGTH] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


/* Custom interrupt flags */
#define TRANSMIT 0x01 
#define ADC_DONE 0x02
#define BOOL3 0x04 //Unused VV
#define BOOL4 0x08 
#define BOOL5 0x10 
#define BOOL6 0x20 
#define BOOL7 0x40 
#define BOOL8 0x80 

volatile unsigned char flags; 

// current transmitted bit index 
unsigned char index;
volatile unsigned char nextbit;

//this variable is a cache for the ADC result
unsigned char transmitTime;

//timer 0 match interrupt
ISR(TIMER0_COMPA_vect){
	//update transmit bit in constant time
	LED_OUT_PORT = (LED_OUT_PORT & ~LED_OUT_PIN) | nextbit;
	//set the flag to take care of the logic
	flags |= TRANSMIT;
}

//ADC conversio done interrupt
ISR(ADC_vect){
	//set the flag to take care of the logic
	flags |= ADC_DONE;
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
	TCCR0A = _BV(WGM01); 	//Timer 0 set to CTC mode
  	TCCR0B = _BV(CS01) | _BV(CS00); //Timer on, 64 prescaler
	//TCCR0B = _BV(CS02) | _BV(CS00); //Timer on, 1024 prescaler FOR TESTING
	transmitTime = TRANSMIT_TIME;   //init timing cache
	OCR0A = transmitTime;			//Match 10ms cycle
	TIMSK |= _BV(OCIE0A); 			//Enable Match interrupt

	/*
	* ADC1 init
	*	this adc input takes a voltage from a potentiometer
	*	and tunes the transmission rate of the screwdriver
	*/
	ADMUX = 0x01; //use Vcc as reference voltage and ADC1 as input
	ADCSRA |= _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | 0x07; //enable ADC with auto trigger and interrupt, with /128 prescaler
	ADCSRB |= _BV(ADLAR); //keep the top bits in ADCH (lower accuracy, higher tuning)
	DIDR0 |= _BV(ADC1D); //disable digital input on ADC1
	ADC_IN_PORT |= _BV(ADC_IN_PIN); //set digital output to a 1 to save power
	ADCSRA |= _BV(ADSC); //start conversions

	/* bit transmission setup */
	LED_OUT_DIR |= LED_OUT_PIN;		// enable output on port B, pin 4
	index = 0;						//start transmission at address 0
	nextbit = 0x00;					//clear the next bit variable
  
  	sei(); //enable interrupts

	/*MAIN LOOP */
	while(1){
		//let ints do their thing...
		//handle the transmit timer interrupt
		if (flags & ADC_DONE) {
			//update transmit time from the ADC
			//range = 128-256, done with bit shifts
			//transmitTime = (ADCH >> 1) | 0x80;
			
			//clear the interrupt flag...
			flags &= ~ADC_DONE;
		} 

		//handle the transmit timer interrupt
		if (flags & TRANSMIT) {
			//update transmit time from the ADC
			OCR0A = transmitTime;
			
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
