/********************************************
  Szymon Witamborski
********************************************/

#include "mcs51/8051.h"

#define SM_SET 1
#define REN_SET 1
#define SCON_SET ((SM_SET << 6) | (REN_SET << 4))

void startSerial(void) {
	SCON = SCON_SET;
	PCON |= 0x80;
	//TI = 1;
	//SBUF = 0xff;
}

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

void startInterrupts(void) {
	ET0 = 1;
	ES = 1;
	EA = 1;
}

void timer0int (void) __interrupt(TF0_VECTOR) {
	was_1ms = 1;
	TH0 |= th0;
	TL0 |= tl0;
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
		received = 1;
	}
}

#define COUNTERS 5
// liczniki:
volatile unsigned char counters[COUNTERS] = {0,0,0,0,0};
// wartosci przepelnienia dla odpowiednich licznikow:
__code volatile unsigned char maxes[COUNTERS] = {4,250,60,60,100};
// flagi przepelnienia dla licznikow:
volatile unsigned char flags[COUNTERS] = {0,0,0,0,0};

void checkEvents() {
	unsigned volatile char i, tmp;
	for(i = 0; i < COUNTERS; ++i) {
		tmp = counters[i];
		++tmp;
		if(tmp == maxes[i]) {
			counters[i] = 0;
			flags[i] = 1;
		} else {
			counters[i] = tmp;
			flags[i] = 0;
			break;
		}
	}
	// czyszczenie nastepnych elementow
	for(++i; i < COUNTERS; ++i) flags[i] = 0;
}

// adresy buforu wyboru wyswietlacza
__xdata __at (0xff30) unsigned char csds;
//    i buforu wartosci wyswietlacza
__xdata __at (0xff38) unsigned char csdb;

// kody kolejnych liczb
__code unsigned char bcodes[10] = {
  0x3f, //0
  0x06, //1
  0x5b, //2
  0x4f, //3
  0x66, //4
  0x6d, //5
  0x7d, //6
  0x07, //7
  0x7f, //8
  0x6f  //9
};

// poczatkowy stan bufora wyswietlacza
unsigned char disBuff[6] = 
	{0x3f | 0x80, 0x3f, 0x3f | 0x80, 0x3f, 0x3f | 0x80, 0x3f};

// aktywny wyswieltacz
unsigned char seg_num = 0;

__code unsigned char segs[6] = {1, 2, 4, 8, 16, 32};

void rotateDisplay(void) {
	P1_6 = 1;
	csds = segs[seg_num];
	csdb = disBuff[seg_num];
	++seg_num;
	if(seg_num == 6) {
		seg_num = 0;
	}
	P1_6 = 0;
}

void setNum(unsigned char block, unsigned char num) {
	disBuff[block] = bcodes[num % 10] | 0x80;
	disBuff[block+1] = bcodes[num / 10];
}

__code char bcard[] = "\n\rSzymon Witamborski\n\r  III rok inf.\n\r  santamon@gmail.com\n\r";
char* bcardptr = bcard;

void main(void) {
	bit run = 1;
	char a = 0;
	
	startInterrupts();
	startSerial();
	startTimers();
	TI = 1;
	while(1) {
		if(was_1ms) {
			was_1ms = 0;
			checkEvents();
		}
		if(flags[0]) {
			flags[0] = 0;
			rotateDisplay();
		}
		if(flags[1]) {
			flags[1] = 0;
			P1_7 = !P1_7;
			setNum(0, counters[2]);
		}
		if(flags[2]) {
			flags[2] = 0;
			setNum(2, counters[3]);
		}
		if(flags[3]) {
			flags[3] = 0;
			setNum(4, counters[4]);
		}

		if(received) {
			received = 0;
			if(recvChar == 's') {
				bcardptr = bcard;
				send = 1;
			} else {
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
