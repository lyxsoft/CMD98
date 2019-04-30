//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "STARTUP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmStartup *frmStartup;
//---------------------------------------------------------------------------
__fastcall TfrmStartup::TfrmStartup(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmStartup::FormCreate(TObject *Sender)
{
	Height = 64;
    Width = 304;
}
//---------------------------------------------------------------------------


