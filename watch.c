/********************************************
  Szymon Witamborski
********************************************/

#include "mcs51/8051.h"

#define T0_G 0
#define T0_C 0
#define T0_M 1
#define TMOD_SET T0_M+T0_C*4+T0_G*8

short th0 = 0;
short tl0 = 0;
volatile bit was_1ms = 0;

void startTimer0(short padding) {
	short t0;
	TMOD = TMOD_SET;

	t0 = 0xffff-padding;
	TH0 = th0 = t0/256;
	TL0 = tl0 = t0%256;
	
	TH0 = th0;
	TL0 = tl0;
	// START
	TR0 = 1;
}

void startInterrupts(void) {
	ET0 = 1;
	EA = 1;
}

void timer0int (void) __interrupt(TF0_VECTOR) {
	was_1ms = 1;
	TH0 |= th0;
	TL0 |= tl0;
}

#define COUNTERS 5
// liczniki:
volatile unsigned char counters[COUNTERS] = {0,0,0,0,0};
// wartosci przepelnienia dla odpowiednich licznikow:
volatile unsigned char maxes[COUNTERS] = {4,250,60,60,100};
// flagi przepelnienia dla licznikow:
volatile unsigned char flags[COUNTERS] = {0,0,0,0,0};

void checkEvents() {
	unsigned char i, tmp;
	if(was_1ms) {
		//for(i = 0; i < COUNTERS; ++i) flags[i] = 0;
		was_1ms = 0;
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
}

// adresy buforu wyboru wyswietlacza
__xdata unsigned char * __data csds = 0xff30;
//    i buforu wartosci wyswietlacza
__xdata unsigned char * __data csdb = 0xff38;

// kody kolejnych liczb
unsigned char bcodes[10] = {
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
	{0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f};

// aktywny wyswieltacz
unsigned char seg_num = 0;

void rotateDisplay(void) {
	P1_6 = 1;
	*csds = 1 << seg_num;
	*csdb = disBuff[seg_num];
	++seg_num;
	if(seg_num == 6) {
		seg_num = 0;
	}
	P1_6 = 0;
}

void setNum(unsigned char block, unsigned char num) {
	disBuff[block] = bcodes[num % 10];
	disBuff[block+1] = bcodes[num / 10];
}

void main(void) {
	startInterrupts();
	startTimer0(922);
	while(1) {
		checkEvents();
		if(flags[0]) rotateDisplay();
		if(flags[1]) {
			P1_7 = !P1_7;
			setNum(0, counters[2]);
		}
		if(flags[2]) setNum(2, counters[3]);
		if(flags[3]) setNum(4, counters[4]);
	}
}
