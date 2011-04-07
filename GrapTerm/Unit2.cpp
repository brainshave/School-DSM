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
        : TThread(CreateSuspended), strings(strs), device(dev),toSend(0), state(0xffff),
        malfunction_state(~((1 << 9) | (1 << 12))), malfunction(0), horizPedWanna(false), vertPedWanna(false)
{
strings->Add("Tworze watek odbierajacy");
Priority = tpNormal;
}

// tabela 2-wym. stan swiatel dla samochodu poziomego, dla samochodu pionowego, czas w ms.
static unsigned int carRoutes[][3] = {
{YELLOW, RED, 1000},
{RED, RED | YELLOW, 1000},
{RED, GREEN, 2500},
{RED, GREEN, 2500},
{RED, YELLOW, 1000},
{RED | YELLOW, RED, 1000},
{GREEN, RED, 2500},
{GREEN, RED, 2500}};
static unsigned short carRoutesSize = 8;

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
        int lightConf = 0;
        //---- Place thread code here ----
        while (1) {
            //if(toSend) {
            //    TransmitCommChar(*device, toSend);
            //    toSend = 0;
            //}
            if (malfunction) {                    
                Sleep(500);
                state = horizCar->change(YELLOW, state);
                state = vertCar->change(YELLOW, state);

                TransmitCommChar(*device, (char) 0xff & state);
                TransmitCommChar(*device, (char) 0xff & (state >> 8));

                Sleep(500);
                state = horizCar->change(0, state);
                state = vertCar->change(0, state);
            } else {
                unsigned int horizColor = carRoutes[lightConf][0];
                //horizNextColor = carRoutes[(lightConf + 1) % 8][0];
                unsigned int vertColor = carRoutes[lightConf][1];
                //vertNextColor = carRoutes[(lightConf + 1) % 8][1];
                state = horizCar->change(horizColor, state);
                if(horizPedWanna && horizColor == GREEN) {
                    state = horizPed->change(GREEN, state);
                } else {
                    if(horizPed->actual == GREEN) horizPedWanna = false;
                    state = horizPed->change(RED, state);
                }

                if(vertPedWanna && vertColor == GREEN) {
                    state = vertPed->change(GREEN, state);
                } else {
                    if(vertPed->actual == GREEN) vertPedWanna = false;
                    state = vertPed->change(RED, state);
                }

                state = vertCar->change(carRoutes[lightConf][1], state);
                Sleep(carRoutes[lightConf][2]);
                lightConf++;
                if(lightConf >= 8) lightConf = 0;
            }


            TransmitCommChar(*device, (char) 0xff & state);
            TransmitCommChar(*device, (char) 0xff & (state >> 8));
            ///ReadFile(*device, &buffer, 1, &bReceived, NULL);
            //if(bReceived) {
            //    char b = buffer[0];
            //    if (b == '\n') {
            //        strings->Add("");
            //    } else {
            //        int last = strings->Count -1;
            //        AnsiString lastStr = (*strings)[last] + b;
            //        strings->Delete(last);
            //        strings->Add(lastStr);
            //    }
            //}
        }
}
//---------------------------------------------------------------------------
