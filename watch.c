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
volatile unsigned char count_4ms = 0;

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

volatile unsigned char counters[6] = {0,0,0,0,0,0};
volatile unsigned char flags[6] = {0,0,0,0,0,0};
volatile unsigned char maxes[6] = {4,250,60,60,100,1};

void checkEvents() {
	unsigned char i;
	unsigned char tmp;
	if(was_1ms) {
		for(i = 0; i < 6; ++i) flags[i] = 0;
		was_1ms = 0;
		for(i = 0; i < 6; ++i) {
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
	}
}

unsigned char seg_num = 0;

__xdata unsigned char * __data csds = 0xff30;
__xdata unsigned char * __data csdb = 0xff38;

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

unsigned char disBuff[6] = 
{0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f};

bit setNum(unsigned char block, unsigned char num) {
	unsigned char digit1, digit2;
	digit1 = num % 10;
	digit2 = num / 10;
	disBuff[block] = bcodes[digit1];
	disBuff[block+1] = bcodes[digit2];
	return 1;
}

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


void main(void) {
	startInterrupts();
	startTimer0(922);
	while(1) {
		/*if(check_4ms()) rotateDisplay();
		if(check_1s()) { 
			P1_7 = !P1_7;
			setNum(0, count_60s);
		}
		if(check_1m()) setNum(2, count_60m);
		if(check_1h()) setNum(4, count_99h);
		check_99h();*/

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
