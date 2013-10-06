//#include "FlexiTimer2.h"

#define BIT_LENGTH_MILLIS 100 // in milliseconds

const int ledPin = 13;

//unsigned char bytes[] = { 0xaa , 0xde , 0xad , 0xbe , 0xef , 0xff };
unsigned char bytes[] = { 0xaa , 0xaa , 0xaa , 0xaa , 0xaa , 0xaa };
volatile int interruptFlag = 0;
volatile int nextBit = 0;
//volatile unsigned int tcnt2 = 6; //counter start value, 250 (256 - 6)duty cycle at 16 Mhz = 250 * 16MHz/64 = 10ms

int byteIndex = 0;
int byteSize  = 6;

void setup()
{
  pinMode( ledPin, OUTPUT );
  digitalWrite( ledPin, LOW );

  interruptFlag = 0;

  //FlexiTimer2::set( BIT_LENGTH_MILLIS, onInterrupt );
  //FlexiTimer2::start();
  initTimer();
}

void initTimer()
{
  //set up timer to for normal mode + /64 prescaler 
  TIMSK2 &= ~(1<<TOIE2 | 1<<OCIE2A);
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~((1<<WGM22) | (1<<CS21) | (1<<CS20));
  TCCR2B |= (1<<CS22);
  TCCR2B |= (1<<CS21) | (1<<CS20); //1024 prescaler FOR TESTING
  ASSR &= ~(1<<AS2);
  //start the timer
  TIMSK2 |= (1<<TOIE2);
}

//Timer2 overflow interrupt vector
ISR(TIMER2_OVF_vect) {
  TCNT2 = 6; //counter start value, 250 (256 - 6)duty cycle at 16 Mhz = 250 * 16MHz/64 = 10ms
  onInterrupt();
}


void onInterrupt()
{
  digitalWrite( ledPin, nextBit);
  interruptFlag = 1;
}


void loop()
{
  if ( interruptFlag ) {
    interruptFlag = 0;
    // Prevent byteIndex from overflowing
    byteIndex = (byteIndex + 1) % ( 8 * byteSize );
    
    nextBit = (bytes[byteIndex / 8] >> byteIndex % 8 ) & 0x01;
  }
}
