//---------------------------------------------------------------------------
#ifndef CMDMAINH
#define CMDMAINH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TListView *lvwMain;
	TToolBar *tbrMain;
	TStatusBar *stbMain;
	TToolButton *btnNew;
	TMainMenu *mnuMain;
	TMenuItem *mnuSystem;
	TMenuItem *mnuSystemExit;
	TMenuItem *mnuHelp;
	TMenuItem *mnuHelpAbout;
	TMenuItem *mnuItemNew;
	TMenuItem *N1;
	TMenuItem *mnuItemEdit;
	TMenuItem *mnuItemDelete;
	TImageList *imgMain;
	TToolButton *btnEdit;
	TToolButton *btnSave;
	TToolButton *btnDelete;
	TToolButton *ToolButton4;
	TMenuItem *mnuSystemSave;
	TMenuItem *mnuItemDisableAll;
	TMenuItem *mnuItemDisable;
	TMenuItem *mnuSystemShowWnd;
	TMenuItem *N2;
	TMenuItem *mnuHelpContent;
	TMenuItem *N3;
	TToolButton *ToolButton1;
	TMenuItem *N4;
	TMenuItem *mnuItem;
	TMenuItem *mnuSystemSaveFile;
	TMenuItem *mnuSystemLoadFile;
	TSaveDialog *dlgSaveReg;
	TOpenDialog *dlgLoadReg;
	TToolButton *btnDisable;
	TPopupMenu *pmnMain;
	TMenuItem *mnuPMNShow;
	TMenuItem *mnuPMNSep1;
	TMenuItem *mnuPMNDisable;
	TMenuItem *mnuPMNShowHide;
	TMenuItem *mnuPMNSep2;
	TMenuItem *mnuPMNExit;
    TMenuItem *mnuHelpHomepage;
    TMenuItem *N5;
    void __fastcall mnuSystemExitClick(TObject *Sender);
	void __fastcall WndProc (Messages::TMessage &Message);

	void __fastcall mnuItemNewClick(TObject *Sender);
	void __fastcall mnuItemEditClick(TObject *Sender);
	
	void __fastcall mnuSystemSaveClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall mnuItemDeleteClick(TObject *Sender);
	void __fastcall mnuItemDisableAllClick(TObject *Sender);
	void __fastcall mnuHelpAboutClick(TObject *Sender);
	
	void __fastcall mnuSystemShowWndClick(TObject *Sender);
	
	void __fastcall mnuItemDisableClick(TObject *Sender);
	void __fastcall mnuHelpContentClick(TObject *Sender);
	void __fastcall mnuSystemSaveFileClick(TObject *Sender);
	void __fastcall mnuItemClick(TObject *Sender);
	
	void __fastcall mnuSystemLoadFileClick(TObject *Sender);
	
	void __fastcall mnuPMNShowClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
    void __fastcall mnuHelpHomepageClick(TObject *Sender);
private:	// User declarations
    void __fastcall SendKey (UINT, UINT);
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
