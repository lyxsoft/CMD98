//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CMDChild.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmChild *frmChild;

extern char *strAppName;
extern char *strMSGCommand;
extern char *strMSGDelete;


//---------------------------------------------------------------------------
__fastcall TfrmChild::TfrmChild(TComponent* Owner)
	: TForm(Owner)
{
	fResult = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChild::FormShow(TObject *Sender)
{
    if (cCMDChild.uFunction >= 0)
        cmbCommand->ItemIndex = cCMDChild.uFunction;
    else
    {
        cmbCommand->ItemIndex = -1;
	    cmbCommand->Text = cCMDChild.strCommand;
    }
	txtDirectory->Text = cCMDChild.strDirectory;

    cmbCmdShow->ItemIndex = cCMDChild.uCmdShow;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChild::btnCancelClick(TObject *Sender)
{
	Close ();
}
//---------------------------------------------------------------------------

void __fastcall TfrmChild::btnOKClick(TObject *Sender)
{
    if (cmbCommand->Text.IsEmpty ())
    {
    	Application->MessageBox (strMSGCommand, strAppName, MB_OK + MB_ICONEXCLAMATION);
        cmbCommand->SetFocus ();
    	return;
	}

    if (cmbCommand->ItemIndex >=0)
    {
	    cCMDChild.strCommand = "";
        cCMDChild.strDirectory = "";
        cCMDChild.uFunction = cmbCommand->ItemIndex;
    }
    else
    {
	    cCMDChild.strCommand = cmbCommand->Text;
        cCMDChild.strDirectory = txtDirectory->Text;
        cCMDChild.uFunction = -1;
    }
    cCMDChild.uCmdShow = cmbCmdShow->ItemIndex;
    fResult = true;
    Close ();
}
//---------------------------------------------------------------------------

void __fastcall TfrmChild::btnBrowseClick(TObject *Sender)
{
	if (cmbCommand->ItemIndex < 0)
		dlgBrowse->FileName = cmbCommand->Text;
	if (dlgBrowse->Execute())
    {
    	cmbCommand->ItemIndex = -1;
        cmbCommand->Text = dlgBrowse->FileName;
    }
}
//---------------------------------------------------------------------------

