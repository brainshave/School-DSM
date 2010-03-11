#include "mcs51/8051.h"

#define T0_G 0
#define T0_C 0
#define T0_M 1
#define TMOD_SET T0_M+T0_C*4+T0_G*8

short th0 = 0;
short tl0 = 0;
void startTimer0(short padding) {
	short t0;
	TMOD = TMOD_SET;

	t0 = 0xffff-padding;
	TH0 = th0 = t0/256;
	TL0 = tl0 = t0%256;
	
	resetTimer0();
	// START
	TR0 = 1;
}

void resetTimer0(void) {
	TF0 = 0;
	TH0 |= th0;
	TL0 |= tl0;
}
	
short wait_counter = 0;

bit wait(void) {
	if(TF0) {
		resetTimer0();
		return 1;
	} else {
		return 0;
	}
}

void main(void) {
	startTimer0(0xffff);
	while(1) {
		wait() && (P1_7 = !P1_7);
	}
}