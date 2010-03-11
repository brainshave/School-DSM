#include "mcs51/8051.h"



void setTimer0(short padding) {
/*
T0_G		EQU	0			; GATE:	do startu potrzeba tylko TR0=1
T0_C		EQU	0			; C/T:	liczymy z zegara
T0_M		EQU	1			; MODE:	tryb 16-bitowy

TMOD_SET	EQU	T0_M+T0_C*4+T0_G*8 	; Wszystkie parametry dla Timera 0

T0_VAR		EQU	65536-922		; ok. 1ms
TH0_SET		EQU	T0_VAR/256		; starsze bity dla TH0
TL0_SET		EQU	T0_VAR%256		; mlodsze bity dla TL0

		MOV	TMOD,#TMOD_SET		; TMOD
		MOV	TH0,#TH0_SET		; starsze bity
		MOV	TL0,#TL0_SET		; mlodsze bity
*/

	#define T0_G 0
	#define T0_C 0
	#define T0_M 1
	#define TMOD_SET T0_M+T0_C*4+T0_G*8
	TMOD = TMOD_SET;

	#define T0_VAR (65536-padding)
	#define TH0_SET T0_VAR/256
	#define TL0_SET T0_VAR%256
	TL0 = TH0_SET;
	TL1 = TL0_SET;
}

short wait_counter = 0;

bit wait(void) {
	wait_counter++;
	if(wait_counter == 0xffff) {
		wait_counter = 0;
		return 1;
	} else {
		return 0;
	}
}

void main(void) {
	while(1) {
		wait() && (P1_7 = !P1_7);
	}
}