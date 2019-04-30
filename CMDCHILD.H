//---------------------------------------------------------------------------
#ifndef CMDChildH
#define CMDChildH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>

#include "CMDDATA.H"

//---------------------------------------------------------------------------
class TfrmChild : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label5;
	TComboBox *cmbCommand;
	TComboBox *cmbCmdShow;
	TEdit *txtDirectory;
	TButton *btnOK;
	TButton *btnCancel;
	TBevel *Bevel1;
	TButton *btnBrowse;
	TOpenDialog *dlgBrowse;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnOKClick(TObject *Sender);
	void __fastcall btnBrowseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmChild(TComponent* Owner);
    bool		fResult;
    CMDChild	cCMDChild;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmChild *frmChild;
//---------------------------------------------------------------------------
#endif
