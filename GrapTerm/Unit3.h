//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------


#include <ExtCtrls.hpp> 
//#include "Unit2.h"

static const unsigned int RED = 1;
static const unsigned int YELLOW = 2;
static const unsigned int GREEN = 4;

class Light {

TShape * red;
TShape * yellow;
TShape * green;
unsigned short redmask;
unsigned short yellowmask;
unsigned short greenmask;

bool pedestrian;

public:         
//RecvThread * thread;
Light(TShape * red, unsigned short redmask, TShape * yellow,
unsigned short yellowmask, TShape * green,
unsigned short greenmask);

int which();
unsigned short change(int state, unsigned short oldState);

int actual;

};


#endif
