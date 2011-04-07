//---------------------------------------------------------------------------

#ifndef Unit5H
#define Unit5H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "Unit3.h"
//---------------------------------------------------------------------------
class TimedChanger : public TThread
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
  Light * light;
  int color;
  TimedChanger * next;
protected:
        void __fastcall Execute();
public:
        __fastcall TimedChanger(Light * light, int color, TimedChanger * next);
};
//---------------------------------------------------------------------------
#endif
 