//---------------------------------------------------------------------------
#ifndef CMDITEMH
#define CMDITEMH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>

#include "CMDDATA.H"
#include <Dialogs.hpp>


//---------------------------------------------------------------------------
class TfrmItem : public TForm
{
__published:	// IDE-managed Components
	THotKey *hkyCmdKey;
	TLabel *Label1;
	TComboBox *cmbCommand;
	TLabel *Label2;
	TComboBox *cmbCmdShow;
	TLabel *Label3;
	TBevel *Bevel1;
	TButton *btnOK;
	TButton *btnCancel;
	TLabel *Label4;
	TEdit *txtName;
	TEdit *txtDirectory;
	TLabel *Label5;
	TCheckBox *chkDblClick;
	TListView *lvwChild;
	TToolBar *tbrChild;
	TToolButton *btnNew;
	TToolButton *btnEdit;
	TToolButton *btnDelete;
	TImageList *imgMain;
	TButton *btnBrowse;
	TOpenDialog *dlgBrowse;
	TLabel *Label6;
	TCheckBox *chkEnableFirst;
	TDateTimePicker *dtkAutoStart;
	TLabel *Label7;
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnOKClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	
	
	
	
	void __fastcall btnNewClick(TObject *Sender);
	void __fastcall btnEditClick(TObject *Sender);
	void __fastcall btnDeleteClick(TObject *Sender);
	void __fastcall btnBrowseClick(TObject *Sender);
	
private:	// User declarations
public:		// User declarations
	__fastcall TfrmItem(TComponent* Owner);
    bool	fResult;
    CMDItem	cCMDItem;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmItem *frmItem;
//---------------------------------------------------------------------------
#endif
