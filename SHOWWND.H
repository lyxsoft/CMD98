//---------------------------------------------------------------------------
#ifndef SHOWWNDH
#define SHOWWNDH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmShowWnd : public TForm
{
__published:	// IDE-managed Components
	TListBox *lstWindow;void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall lstWindowDblClick(TObject *Sender);
	
private:	// User declarations
public:		// User declarations
	__fastcall TfrmShowWnd(TComponent* Owner);
    void __fastcall Add (HWND);
	void __fastcall ShowLast (void);
	void __fastcall ShowAll (void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmShowWnd *frmShowWnd;
//---------------------------------------------------------------------------
#endif
