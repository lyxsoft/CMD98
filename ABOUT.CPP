//---------------------------------------------------------------------------
#include <vcl.h>
#include <registry.hpp>
#pragma hdrstop

#include "ABOUT.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmAbout *frmAbout;

extern const char *strRegKey;

//---------------------------------------------------------------------------
__fastcall TfrmAbout::TfrmAbout(TComponent* Owner)
	: TForm(Owner)
{
	TRegistry*	lpRegistry = new (TRegistry);

    lpRegistry->RootKey = HKEY_LOCAL_MACHINE;
    if (!lpRegistry->OpenKey (strRegKey, false))
    	return;

	lblUser->Caption = lpRegistry->ReadString ("RegUser");
    lblOrgnization->Caption = lpRegistry->ReadString ("RegOrg");
}
//---------------------------------------------------------------------------
void __fastcall TfrmAbout::btnOKClick(TObject *Sender)
{
	Close ();
}
//---------------------------------------------------------------------------
