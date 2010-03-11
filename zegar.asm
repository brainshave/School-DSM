;	-------------------------------------------------------------------------------------------
;	Szymon Witamborski
;	Program dziala jako zegarek
;	-------------------------------------------------------------------------------------------

;		Stale, dla obslugi wysietlacza
;		------------------------------
DZIEWIEC	EQU	01101111B	; wyswietli 9
ZERO		EQU	00111111B	; wyswietli 0

;		Stale, dla Timera
;		-----------------
T0_G		EQU	0			; GATE:	do startu potrzeba tylko TR0=1
T0_C		EQU	0			; C/T:	liczymy z zegara
T0_M		EQU	1			; MODE:	tryb 16-bitowy

TMOD_SET	EQU	T0_M+T0_C*4+T0_G*8 	; Wszystkie parametry dla Timera 0

T0_VAR		EQU	65536-922		; ok. 1ms
TH0_SET		EQU	T0_VAR/256		; starsze bity dla TH0
TL0_SET		EQU	T0_VAR%256		; mlodsze bity dla TL0

LICZNIK		EQU	40
LICZ5		EQU	41

DANE		EQU	42
CYFRY		EQU	48

LICZ200		EQU	59

SEKUNDY		EQU	60
MINUTY		EQU	61
GODZINY		EQU	62
KLAWTMP		EQU	63	; bufor do zliczania nac. klawiszy
KLAW		EQU	64	; gotowy stan klawiatury
STOS		EQU	80


BYLO4		EQU 	0
BYLO1		EQU	1
BYLO250		EQU	2

BYLAMINUTA	EQU	3
BYLAGODZINA	EQU	4

KLAWGOT		EQU	5			; klawiatura gotowa

;		Koniec danych
;		-------------


;		Skok do poczatku programu
;		-------------------------
		LJMP 	START
		
;		Przerwanie Timera 0
;		-------------------
		ORG	0BH
		ORL	TL0,#TL0_SET		; mlodsze bity
		MOV	TH0,#TH0_SET		; starsze bity
		SETB 	BYLO1
		RETI


;		Reszta programow
;		----------------
		ORG 	100H



;		###################################################################################
;		################    S  T  A  R  T     P  R  O  G  R  A  M  U    ###################
;		###################################################################################

START:
		MOV	SP,#STOS
		
;		Ustawienie Timera 0
;		-------------------
		MOV	TMOD,#TMOD_SET		; TMOD
		MOV	TH0,#TH0_SET		; starsze bity
		MOV	TL0,#TL0_SET		; mlodsze bity
		
		
		MOV	LICZNIK,#0
		CLR	BYLO4
		CLR	BYLO1
		CLR	BYLO250
		CLR	BYLAMINUTA
		CLR	BYLAGODZINA
		CLR	KLAWGOT
		MOV	LICZ5,#0
		MOV	LICZ200,#0
		
		MOV	SEKUNDY,#0
		MOV	MINUTY,#0
		MOV	GODZINY,#0
		MOV 	KLAW,#0
		MOV	KLAWTMP,#0
		
		MOV	CYFRY,  #00111111B ; zero
		MOV	CYFRY+1,#00000110B ; jeden
		MOV	CYFRY+2,#01011011B ; dwa
		MOV	CYFRY+3,#01001111B ; trzy
		MOV	CYFRY+4,#01100110B ; cztery
		MOV	CYFRY+5,#01101101B ; piec
		MOV	CYFRY+6,#01111101B ; szesc
		MOV	CYFRY+7,#00000111B ; siedem
		MOV	CYFRY+8,#01111111B ; osiem
		MOV	CYFRY+9,#01101111B ; dziewiec

		MOV	DANE,CYFRY
		ORL	DANE,#128 	; kropka
		MOV	DANE+1,CYFRY
		MOV	DANE+2,CYFRY
		ORL	DANE+2,#128
		MOV	DANE+3,CYFRY
		MOV	DANE+4,CYFRY
		ORL	DANE+4,#128
		MOV	DANE+5,CYFRY

;		Ustawienie przerwan
;		-------------------
		SETB	ET0			; Przerwanie Timera 0
		SETB 	EA			; Przerwania w ogole

		SETB	TR0			; start Timera 0


PETLA:
;		Zdarzenie 1ms
;		-------------
		JNB	BYLO1,PTL4MS
		CLR	BYLO1
		LCALL	ZD1MS

PTL4MS:
;		Zdarzenie 4ms
;		-------------
		JNB	BYLO4,PTL1S
		CLR	BYLO4
		LCALL	KLAWIATURA
		LCALL	WYSWIETL
		LCALL	ZD4MS

		
PTL1S:
;		Zdarzenie 1s
;		-----------------
		JNB	BYLO250,PTLMINUTA
		CLR 	BYLO250
		LCALL	ZD1SEK

		
PTLMINUTA:
;		Zdarzenie minuta
;		----------------
		JNB	BYLAMINUTA,PTLGODZINA
		CLR	BYLAMINUTA
		LCALL	ZDMIN
		
PTLGODZINA:
;		Zdarzenie godzina
;		----------------
		JNB	BYLAGODZINA,PTLKLAW
		CLR	BYLAGODZINA
		LCALL	ZDGODZ
		
PTLKLAW:
		JNB	KLAWGOT,PTLKON
		CLR	KLAWGOT
		MOV	A,KLAW
		CJNE	A,#0,PTLKON
		CPL	P1.7
PTLKON:
		LJMP	PETLA


;		Procedury
;		---------

;		Zmiana cyfry
;		Potrzebuje:
;		R2: liczba
;		R1: offset segmentu wyswietlacza
;		--------------------------------
				
ZMIEN_CYFRE:
		MOV	A,R1
		ADD	A,#DANE
		MOV	R1,A		; W R1 siedzi teraz adres danej dla segmentu
		
		MOV	A,R2
		;DA	A
		ANL	A,#00001111B	; wyciagamy jednosci
		ADD	A,#CYFRY
		MOV	R0,A
		MOV	A,@R0
		ORL	A,#128
		MOV	@R1,A
		
;		Dziesiatki
;		----------
		INC 	R1		; W R1 siedzi teraz adres danej dla segmentu
		
		MOV	A,R2		; pobieramy cyfre
		;DA	A
		SWAP	A
		ANL	A,#00001111B	; wyciagamy dziesiatki
		ADD	A,#CYFRY
		MOV	R0,A
		MOV	A,@R0
		MOV	@R1,A	
		
		
		RET

KLAWIATURA:
;		Obsluga klawiatury
;		------------------
		JNB	P3.5,KLAWSUM
		MOV	DPTR,#SEGMENTY
		MOV	A,LICZNIK
		MOVC	A,@A+DPTR
		ORL	A,KLAWTMP
		MOV	KLAWTMP,A
KLAWSUM:
	;	JB	KLAWGOT,KLAWKON
	;	MOV	A,LICZNIK
	;	CJNE	A,#5,KLAWKON
	;	MOV	KLAW,KLAWTMP
	;	MOV	KLAWTMP,#0
	;	SETB	KLAWGOT
KLAWKON:
		RET

WYSWIETL:
		SETB	P1.6			; zgaszenie wyswietlacza
		
		MOV	DPTR,#SEGMENTY
		MOV	A,LICZNIK
		MOVC	A,@A+DPTR
		MOV 	DPTR,#0FF30H
		MOVX	@DPTR,A
		
		MOV	A,#DANE
		ADD	A,LICZNIK
		MOV	R0,A
		MOV	A,@R0
		MOV	DPTR,#0FF38H
		MOVX	@DPTR,A

		INC	LICZNIK
		MOV	A,LICZNIK
		CJNE	A,#6,WYS_KONIEC
		
		MOV	LICZNIK,#0
		JB	KLAWGOT,WYS_KONIEC
		MOV	KLAW,KLAWTMP
		MOV	KLAWTMP,#0
		SETB	KLAWGOT

WYS_KONIEC:
		CLR	P1.6			; wlaczenie wyswietlacza
		RET


ZD1MS:
;		Zdarzenie 1ms
;		-------------
;		Wywoluje zdarzenie 4ms
;		----------------------
		INC 	LICZ5
		MOV	A,LICZ5
		CJNE	A,#4,ZD1MSKON
		SETB	BYLO4
		MOV	LICZ5,#0
ZD1MSKON:
		RET

ZD4MS:		
;		Zdarzenie 4ms
;		-------------
		INC 	LICZ200

;		Wywoluje zdarzenie 4*250ms
;		--------------------------
		MOV	A,LICZ200
		CJNE	A,#2,ZD4MSKON
		MOV	LICZ200,#0
		SETB	BYLO250
ZD4MSKON:
		RET

ZD1SEK:
;		Zdarzenie 1s
;		-----------------
		MOV	A,SEKUNDY
		INC	A
		DA	A
		MOV	SEKUNDY,A
		CJNE	A,#96,ZD1SEKUST
		
;		Wywoluje zdarzenie minuty
;		-------------------------
		SETB	BYLAMINUTA
		MOV	SEKUNDY,#0
ZD1SEKUST:
;		Zmienia sekundy na zegarze
;		-------------------------
		MOV	R2,SEKUNDY
		MOV	R1,#0
		LCALL	ZMIEN_CYFRE

		RET

ZDMIN:
;		Zdarzenie minuta
;		----------------
		MOV	A,MINUTY
		INC	A
		DA	A
		MOV	MINUTY,A
		CJNE	A,#96,ZDMINUST	; 96 = 60 w BCD
		
;		Wywoluje zdarzenie godziny
;		--------------------------
		SETB	BYLAGODZINA
		MOV	MINUTY,#0
ZDMINUST:
;		Zmienia minuty na zegarze
;		-------------------------
		MOV	R2,MINUTY
		MOV	R1,#2
		LCALL	ZMIEN_CYFRE
		
		RET

ZDGODZ:
;		Zdarzenie godzina
;		----------------
		MOV	A,GODZINY
		INC	A
		DA	A
		MOV	GODZINY,A
		CJNE	A,#96,ZDGODZUST
		
;		Kasuje licznik
;		--------------------------
		MOV	GODZINY,#0
		
ZDGODZUST:
;		Zmienia minuty na zegarze
;		-------------------------
		MOV	R2,GODZINY
		MOV	R1,#4
		LCALL	ZMIEN_CYFRE

		RET

;		Wskazniki do wyboru wyswietlacza
;		--------------------------------
SEGMENTY:
		DB 	00000001B
		DB 	00000010B
		DB 	00000100B
		DB 	00001000B
		DB 	00010000B
		DB 	00100000B
		DB 	01000000B
		DB 	10000000B

