//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
HANDLE hCommDev;
DCB dcb;

COMMTIMEOUTS timeouts = { 20, 50, 100, 20, 50 }; 

#define BUFF_SIZE 128
char buffer[BUFF_SIZE];

char *lpFileName = "COM1";
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#include "Unit5.h"

RecvThread * recvTh = NULL;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

bool COMnected = false;

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
        vertCar = new Light(Shape32, 1 << 11, Shape33, 1 << 12, Shape34, 1 << 13);
        vertPed = new Light(Shape36, 1,  0, 0, Shape37, 1 << 1);
        horizCar = new Light(Shape43, 1 << 8, Shape45, 1 << 9, Shape44, 1 << 10);
        horizPed = new Light(Shape28, 1 << 2, 0, 0, Shape30, 1 << 3);
        direction = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
	if(!COMnected) {
        hCommDev = CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE,
                0, NULL, OPEN_EXISTING, 0, NULL);
        if(hCommDev != INVALID_HANDLE_VALUE) {
                dcb.DCBlength = sizeof(dcb);
                GetCommState(hCommDev, &dcb);
                dcb.BaudRate = CBR_4800;
                dcb.fParity = TRUE;
                dcb.Parity = NOPARITY;
                dcb.StopBits = ONESTOPBIT;
                dcb.ByteSize = 8;
                dcb.fDtrControl = DTR_CONTROL_DISABLE;
                dcb.fRtsControl = RTS_CONTROL_DISABLE;

		dcb.fOutxCtsFlow     = FALSE;
		dcb.fOutxDsrFlow     = FALSE;
		dcb.fDsrSensitivity   = FALSE;
		dcb.fAbortOnError    = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.fErrorChar = FALSE;
		dcb.fNull = FALSE;
                SetCommState(hCommDev, &dcb);
		Button1->Caption = "Roz³¹cz" ;
		COMnected = true;

                SetCommTimeouts(hCommDev, &timeouts);

        	Memo1->Lines->Add("Po³¹czono");
                recvTh = new RecvThread(false, Memo1->Lines, &hCommDev);
                //vertCar->thread = recvTh;
                recvTh->vertCar = vertCar;
                //vertPed->thread = recvTh;
                recvTh->vertPed = vertPed;
                //horizCar->thread = recvTh;
                recvTh->horizCar = horizCar;
                 //horizPed->thread = recvTh;
                recvTh->horizPed = horizPed;
        } else {
                //std::cout << "blad\n";
        }
	} else {
		Button1->Caption = "Po³¹cz";
		CloseHandle(&dcb);
		COMnected = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Memo1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        static int diff = 'A' - 'a';
        static char * sendbuff = " ";
        //sendbuff[0] = Key - diff;
        //sendbuff[0] = Key - diff;
        //WriteFile(hCommDev, sendbuff, 1, &sentBytes, 0);

        //recvTh->toSend = (char) Key - diff;

}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button3Click(TObject *Sender)
{
        recvTh->malfunction = 1 - recvTh->malfunction;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
        recvTh->horizPedWanna = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
        recvTh->vertPedWanna = true;        
}
//---------------------------------------------------------------------------

