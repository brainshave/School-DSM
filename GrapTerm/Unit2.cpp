//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall RecvThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall RecvThread::RecvThread(bool CreateSuspended, TStrings * strs, HANDLE * dev)
        : TThread(CreateSuspended), strings(strs), device(dev)
{
strings->Add("Tworze watek odbierajacy");
Priority = tpNormal;
}
//---------------------------------------------------------------------------
void RecvThread::SetName()
{
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = "ReceiverTH";
        info.dwThreadID = -1;
        info.dwFlags = 0;

        __try
        {
                 RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),(DWORD*)&info );
        }
        __except (EXCEPTION_CONTINUE_EXECUTION)
        {
        }
}
//---------------------------------------------------------------------------
void __fastcall RecvThread::Execute()
{
        strings->Add("Rozpoczynam odbior");
        SetName();
        char buffer[] = " ";
        unsigned long bReceived = 0;
        //---- Place thread code here ----
        while (1) {
            ReadFile(*device, &buffer, 1, &bReceived, NULL);
            char b = buffer[0];
            if (b == '\n') {
                strings->Add("");
            } else {
                int last = strings->Count -1;
                AnsiString lastStr = (*strings)[last] + b;
                strings->Delete(last);
                strings->Add(lastStr);
            }
        }
}
//---------------------------------------------------------------------------
 