/*
  Szymon Witamborski
*/

#include "mcs51/8051.h"


void startInterrupts(void) {
  ET0 = 1;
  ES = 1;
  EA = 1;
}

/****************************
 TIMER
****************************/

#define T0_G 0
#define T0_C 0
#define T0_M 1
#define T1_G 0
#define T1_C 0
#define T1_M 2
#define T0_SET (T0_M + (T0_C << 2) + (T0_G << 3))
#define T1_SET (T1_M + (T1_C << 2) + (T1_G << 3))
#define TMOD_SET ((T1_SET << 4) | T0_SET)

#define T0_VAL (0xffff - 922)
#define TH0_SET (T0_VAL >> 8)
#define TL0_SET (T0_VAL & 0xff)

// 57600/4800 = 12
#define T1_VAL (256-12)

unsigned char th0 = 0;
unsigned char tl0 = 0;
volatile bit was_1ms = 0;

void startTimers(void) {
  TMOD = TMOD_SET;
	
  th0 = TH0 = TH0_SET;
  tl0 = TL0 = TL0_SET;
	
  TH1 = T1_VAL;
  TL1 = T1_VAL;
  // START
  TR0 = 1;
  TR1 = 1;
}


void timer0int (void) __interrupt(TF0_VECTOR) {
  was_1ms = 1;
  TH0 |= th0;
  TL0 |= tl0;
}

/**************************************
 PORT SZEREGOWY
*************************************/

#define SM_SET 1
#define REN_SET 1
#define SCON_SET ((SM_SET << 6) | (REN_SET << 4))

void startSerial(void) {
  SCON = SCON_SET;
  PCON |= 0x80;
  //TI = 1;
  //SBUF = 0xff;
}

volatile unsigned char recvChar;
volatile unsigned char sendChar;
volatile bit received = 0;
volatile bit send = 0;

void serial_int (void) __interrupt(4) {
  if(TI) {
    TI = 0;
    send = 1;
  }
	
  if(RI) {
    RI = 0;
    recvChar = SBUF;
    //SBUF = recvChar;
    send = 0;
    received = 1;
  }
}

/**************************************
 OBSLUGA 8255
**************************************/

__xdata __at (0xff28) unsigned char CS55A;
__xdata __at (0xff29) unsigned char CS55B;
__xdata __at (0xff2a) unsigned char CS55C;
__xdata __at (0xff2b) unsigned char CS55D;

/* tryb portow: A i B wyjsciowe w tr. 0.
   wniosek: tylko 7-bit ustawiony na 1, reszta 0 */
#define CS55D_SET 128;

void start8255() {
  CS55D = CS55D_SET;
  CS55A = 255;
  CS55B = 255;
}

// WIZYTOWKA

__code char bcard[] = "\n\rSzymon Witamborski\n\r  III rok inf.\n\r  santamon@gmail.com\n\r";
char* bcardptr = bcard;

void main(void) {
  bit run = 1;
  bit car = 1;
  char a = 0;
  startInterrupts();
  startSerial();
  startTimers();
  start8255();
  TI = 1;
  while(1) {
    if(received) {
      received = 0;
      if(recvChar == 's') {
	bcardptr = bcard;
	send = 1;
      } else {
        if (car) {
       	  CS55A = recvChar;
       	} else {
          CS55B = recvChar;
        }
        car = 1 - car;
	SBUF = recvChar;
      }
    }
		
    if(send) {
      send = 0;
      a = *bcardptr;
      if(a) {
	SBUF = a;
	++bcardptr;
      }
    }
  }
}
