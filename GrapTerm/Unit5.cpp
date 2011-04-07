//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit5.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TimedChanger::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TimedChanger::TimedChanger(Light * light,
                                        int color, TimedChanger * next)
        : TThread(true), light(light), color(color), next(next)
{
}
//---------------------------------------------------------------------------
void TimedChanger::SetName()
{
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = "TimedChanger";
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
void __fastcall TimedChanger::Execute()
{
        SetName();
        //---- Place thread code here ----
        Sleep(300);
        //light->change(color);
        //if (next) next->Resume();
        delete this;
}
//---------------------------------------------------------------------------
 