//---------------------------------------------------------------------------
#ifndef STARTUPH
#define STARTUPH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmStartup : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TImage *Image1;
    void __fastcall FormCreate(TObject *Sender);
	
private:	// User declarations
public:		// User declarations
	__fastcall TfrmStartup(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmStartup *frmStartup;
//---------------------------------------------------------------------------
#endif
