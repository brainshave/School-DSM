//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "Unit3.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TButton *Button1;
        TShape *Shape1;
        TShape *Shape2;
        TShape *Shape3;
        TShape *Shape4;
        TShape *Shape5;
        TShape *Shape6;
        TShape *Shape7;
        TShape *Shape8;
        TShape *Shape9;
        TShape *Shape10;
        TShape *Shape11;
        TShape *Shape12;
        TShape *Shape13;
        TShape *Shape14;
        TShape *Shape15;
        TShape *Shape16;
        TShape *Shape17;
        TShape *Shape18;
        TShape *Shape19;
        TShape *Shape20;
        TShape *Shape21;
        TShape *Shape22;
        TShape *Shape23;
        TShape *Shape27;
        TShape *Shape28;
        TShape *Shape30;
        TShape *Shape31;
        TShape *Shape32;
        TShape *Shape33;
        TShape *Shape34;
        TShape *Shape35;
        TShape *Shape36;
        TShape *Shape37;
        TShape *Shape42;
        TShape *Shape43;
        TShape *Shape44;
        TShape *Shape45;
        TButton *Button3;
        TButton *Button4;
        TButton *Button5;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Memo1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);

        Light * vertCar; //cars
        Light * vertPed; //pedestrians
        Light * horizCar;
        Light * horizPed;

        bool direction; // true == vertical
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
