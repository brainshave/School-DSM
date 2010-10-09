//---------------------------------------------------------------------------

#include <vcl.h>
#include <iostream>
#include <cstdio>
#pragma hdrstop

HANDLE hCommDev;
DCB dcb;
#define BUFF_SIZE 128
char buffer[BUFF_SIZE];

char *lpFileName = "COM1";

//---------------------------------------------------------------------------

#pragma argsused
int main(int argc, char* argv[])
{
		unsigned long bReceived = 0;
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
        } else {
                std::cout << "blad\n";
        }
		bool work;
        do{
				work = 1;
                ReadFile(hCommDev, &buffer, 1, &bReceived, NULL);

				for(int i = 0; i < bReceived; ++i) {
					putchar(buffer[i]);
				}

                //c = getch(hCommDev);
        } while(work);
		CloseHandle(&dcb);
        return 0;
}
//---------------------------------------------------------------------------
 