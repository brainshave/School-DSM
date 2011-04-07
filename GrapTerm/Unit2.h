//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "Unit3.h"
//class Light;
//---------------------------------------------------------------------------




class RecvThread : public TThread
{
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // must be 0x1000
    LPCSTR szName;    // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags;    // reserved for future use, must be zero
  } THREADNAME_INFO;

private:
  void SetName();
  TStrings * strings;
  HANDLE * device;
protected:

        void __fastcall Execute();
public:           
  unsigned short state;
  Light * horizCar;
  Light * vertCar;
  Light * horizPed;
  Light * vertPed;
  bool vertPedWanna;
  bool horizPedWanna;
  const short malfunction_state;
  bool malfunction; 
        __fastcall RecvThread(bool CreateSuspended, TStrings * strs, HANDLE * dev);
        volatile char toSend;

};
//---------------------------------------------------------------------------
#endif
 