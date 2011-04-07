//---------------------------------------------------------------------------


#pragma hdrstop

#include "Unit3.h"


Light::Light(TShape * red, unsigned short redmask, TShape * yellow,
unsigned short yellowmask, TShape * green, 
unsigned short greenmask)
: red(red), yellow(yellow), green(green), redmask(redmask),
yellowmask(yellowmask), greenmask(greenmask) {
        pedestrian = yellow ? true : false;
        //change(RED, 0);
}


int Light::which() {
        return actual;
}

unsigned short Light::change(int state, unsigned short oldState) {
        // logic
        actual = state;
        if(red) red->Brush->Color = state & RED ? clRed : clMaroon;
        if(yellow) yellow->Brush->Color = state & YELLOW ? clYellow : clOlive;
        if(green) green->Brush->Color = state & GREEN ? clLime : clGreen;


          if(state & RED) {
            return oldState & ~redmask;
          } else {
            return oldState | redmask;
          }
          if(state & YELLOW) {
            return oldState & ~yellowmask;
          } else {
            return oldState | yellowmask;
          }
          if(state & GREEN) {
            return oldState & ~greenmask;
          } else {
            return oldState | greenmask;
          }

}

//---------------------------------------------------------------------------

#pragma package(smart_init)
 