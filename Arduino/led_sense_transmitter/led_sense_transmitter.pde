#include "FlexiTimer2.h"

#define BIT_LENGTH_MILLIS 10 // in milliseconds

int ledPin = 13;

unsigned char bytes[] = { 0xaa , 0xde , 0xad , 0xbe , 0xef , 0xff };
volatile int interruptFlag = 0;
volatile int nextBit = 0;

int byteIndex = 0;
int byteSize  = 6;

void setup()
{
  pinMode( ledPin, OUTPUT );
  digitalWrite( ledPin, LOW );

  interruptFlag = 0;

  FlexiTimer2::set( BIT_LENGTH_MILLIS, onInterrupt );
  FlexiTimer2::start();
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
    nextBit = (bytes[byteIndex / 8] >> byteIndex % 8 ) & 0x01;

    // Prevent byteIndex from overflowing
    byteIndex = (byteIndex + 1) % ( 8 * byteSize );
  }
}

/*
void displayBit( unsigned char byte, int bitIndex )
{
  int bit = ( ( byte >> bitIndex ) & 0x01 );
  digitalWrite( ledPin, bit );
}
*/
