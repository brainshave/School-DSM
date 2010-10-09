//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
HANDLE hCommDev;
DCB dcb;
#define BUFF_SIZE 128
char buffer[BUFF_SIZE];

char *lpFileName = "COM1";
#include "Unit1.h"
#include "Unit2.h"

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

        	Memo1->Lines->Add("Po³¹czono");
                recvTh = new RecvThread(false, Memo1->Lines, &hCommDev);
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
        sendbuff[0] = Key - diff;
        unsigned long sentBytes = 0;
        WriteFile(hCommDev, sendbuff, 1, &sentBytes, 0);
}
//---------------------------------------------------------------------------




