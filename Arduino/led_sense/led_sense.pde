#include "FlexiTimer2.h"
#include "unit_8.h"

#define HISTORY_QUEUE_SIZE ( 8 * 6 * 2 )
#define PACKET_SIZE 6

int notificationLedPin = 5;

int packetSize = PACKET_SIZE;
int ledAnode   = 3;
int ledCathode = A0;
int sampleValue;
unsigned int runCounter = 0;

int historyQueue[HISTORY_QUEUE_SIZE];

char serialText[512];

const unsigned long INTERNAL_SAMPLE_RATE = 500;  // Microseconds
const unsigned long EXTERNAL_SAMPLE_RATE = 5;    // 5 Milliseconds = 5000 Microseconds

unsigned long microsStart;
unsigned long microsEnd;

unit_8 currentPacket[PACKET_SIZE];
unit_8 acceptedPattern[] = { 0xaa , 0xde , 0xad , 0xbe , 0xef , 0xff };

volatile int interruptFlag = 0;

void setup()
{
  _SFR_IO8(0x35) |= 0x10;   // global disable pull up resistors

  Serial.begin( 9600 );
  Serial.println( "Begin" );
  
  int i;
  for ( i = 0 ; i < HISTORY_QUEUE_SIZE ; i++ ) {
    historyQueue[i] = 0;
  }

  pinMode( notificationLedPin, OUTPUT );
  digitalWrite( notificationLedPin, LOW );

  FlexiTimer2::set( EXTERNAL_SAMPLE_RATE, interrupt );
  FlexiTimer2::start();
}

void interrupt()
{
  interruptFlag = 1;
}

void loop() {
  if ( interruptFlag ) {
    interruptFlag = 0;
    readOneValue();
  }
}

void readOneValue()
{
  microsStart = micros();
  
  // Apply reverse voltage, charge up the pin and led capacitance
  pinMode( ledCathode, OUTPUT );
  pinMode( ledAnode,   OUTPUT );

  digitalWrite( ledCathode, HIGH );
  digitalWrite( ledAnode,   LOW  );

  pinMode( ledCathode , INPUT );

  // Take the difference of two samples
  sampleValue = analogRead( ledCathode  );
  delayMicroseconds( INTERNAL_SAMPLE_RATE );
  sampleValue = sampleValue - analogRead( ledCathode  );
  
  pushToHistoryQueue( sampleValue );

  processHistoryQueue();

  runCounter++;

  microsEnd = micros();

  printCurrentPacket();
}

void pushToHistoryQueue( int value )
{
  dequeueHistory();
  historyQueue[HISTORY_QUEUE_SIZE - 1] = value;
}

int dequeueHistory()
{
  // push new value onto back of queue;
  int front = historyQueue[0];
  int limit = HISTORY_QUEUE_SIZE - 1;

  for ( int i = 0 ; i < limit ; i++ ) {
    historyQueue[i] = historyQueue[i+1];
  }
  
  return front;
}

void processHistoryQueue()
{

  // Scan the queue for min/max values
  int min = historyQueue[0];
  int max = historyQueue[0];
  int currentValue;
  int i;
  for ( i = 0 ; i < HISTORY_QUEUE_SIZE ; i++ ) {
    currentValue = historyQueue[i];

    if ( currentValue < min ) {
      min = currentValue;
    }

    if ( currentValue > max ) {
      max = currentValue;
    }
  }

  int barrier = ( ( max - min ) / 2 ) + min;
  int seqIndex = 0;

  // interpret all even values as bits and check for the pattern
  for ( i = 0 ; i < HISTORY_QUEUE_SIZE ; i += 2 ) {

    currentValue = historyQueue[i];

    if ( currentValue > barrier ) {
      // one
      currentPacket[seqIndex / 8] |=  ( 0x01 << seqIndex % 8 );
    } else {
      // zero
      currentPacket[seqIndex / 8] &= ~( 0x01 << seqIndex % 8 );
    }

    seqIndex += 1;
  }

  if ( checkForPattern( currentPacket, PACKET_SIZE ) ) {
    toggleLock();
    // Don't check the other bits if we already have the pattern
    return;
  }

  seqIndex = 0;
  // interpret all odd values as bytes and check again for a pattern
  for ( i = 1 ; i < HISTORY_QUEUE_SIZE ; i += 2 ) {

    currentValue = historyQueue[i];

    if ( currentValue > barrier ) {
      // one
      currentPacket[seqIndex / 8] |=  ( 0x01 << seqIndex % 8 );
    } else {
      // zero
      currentPacket[seqIndex / 8] &= ~( 0x01 << seqIndex % 8 );
    }
    seqIndex += 1;
  }

  if ( checkForPattern( currentPacket, PACKET_SIZE ) ) {
    toggleLock();
  }
}

int checkForPattern( unit_8 packet[], int packetSize ) {
  int i;
  for ( i = 0 ; i < packetSize ; i++ ) {
    if ( acceptedPattern[i] != packet[i] ) {
      return 0;
    }
  }
  
  return 1;
}

void toggleLock()
{
  sprintf( serialText, "code match" );
  Serial.println( serialText );
  printCurrentPacket();
  resetHistoryQueue();
  
  digitalWrite( notificationLedPin, HIGH );
  // Delay for a bit while we flash the notification pin
  delay( 500 );
  digitalWrite( notificationLedPin, LOW );
  
  // Clear the interrupt flag if it ran during any of this
  interruptFlag = 0;
}

void resetHistoryQueue()
{
  for ( int i = 0 ; i < HISTORY_QUEUE_SIZE ; i++ ) {
    historyQueue[i] = 0;
  }
}

void printCurrentPacket()
{

  // Don't print anything right now
  return; 

  // sprintf( serialText, "\nSample value: %10d", sampleValue );
  // Serial.println( serialText );
  
  // sprintf( serialText, "        Code: %x %x %x %x %x %x",
  //          currentPacket[0],
  //          currentPacket[1], 
  //          currentPacket[2], 
  //          currentPacket[3], 
  //          currentPacket[4], 
  //          currentPacket[5] );

  sprintf( serialText, "code: [ %2x , %2x , %2x , %2x, %2x,  %2x ]",
           currentPacket[0],
           currentPacket[1],
           currentPacket[2],
           currentPacket[3],
           currentPacket[4],
           currentPacket[5] );
  Serial.println( serialText );

  // sprintf( serialText, "code: [ %2x , %2x ] | historyQueue: [ %4d , %4d , %4d , ... , %4d , %4d , %4d ]",
  //          currentPacket[0],
  //          currentPacket[1],           
  //          historyQueue[0],
  //          historyQueue[1],
  //          historyQueue[2],
  //          historyQueue[HISTORY_QUEUE_SIZE - 3], 
  //          historyQueue[HISTORY_QUEUE_SIZE - 2], 
  //          historyQueue[HISTORY_QUEUE_SIZE - 1] ); 
  // Serial.println( serialText );

  // unsigned long diff = microsEnd - microsStart;
  // sprintf( serialText, "Last runTime: %lu | Start: %lu | End: %lu", diff, microsStart, microsEnd );
  // Serial.println( serialText );

}
