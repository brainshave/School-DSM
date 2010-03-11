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
volatile bit was_4ms = 0;
volatile unsigned char count_250x4ms = 0;
volatile bit was_1s = 0;

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

bit check_4ms(void) {
	if(was_1ms) {
		was_1ms = 0;
		++count_4ms;
		if(count_4ms == 4) {
			count_4ms = 0;
			was_4ms = 1;
			return 1;
		}
	}
	return 0;
}

bit check_1s(void) {
	if(was_4ms) {
		was_4ms = 0;
		++count_250x4ms;
		if(count_250x4ms == 250) {
			count_250x4ms = 0;
			was_1s = 1;
			return 1;
		}
	}
	was_1s = 0;
	return 0;
}

unsigned char seg_num = 0;

__xdata unsigned char * __data csds = 0xff30;
__xdata unsigned char * __data csdb = 0xff38;

unsigned char* bcodes = {
  0x3f, //0
  0x06, //1
  0x53, //2
  0x4f, //3
  0x66, //4
  0x6c, //5
  0x7c, //6
  0x07, //7
  0x7f, //8
  0x5f  //9
};


bit changeNum(void) {
	P1_6 = 1;
	*csds = 1 << seg_num;
	*csdb = 0xff;
	++seg_num;
	if(seg_num == 6) {
		seg_num = 0;
	}
	P1_6 = 0;
	return 1;
}


void main(void) {
	startInterrupts();
	startTimer0(922);
	while(1) {
		check_4ms() && changeNum();
		check_1s() && (P1_7 = !P1_7);
	}
}
