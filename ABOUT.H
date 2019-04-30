//---------------------------------------------------------------------------
#ifndef ABOUTH
#define ABOUTH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmAbout : public TForm
{
__published:	// IDE-managed Components
	TImage *imgAbout;
	TBevel *Bevel1;
	TButton *btnOK;
	TLabel *Label1;
	TLabel *Label2;
	TPanel *Panel1;
	TLabel *Label3;
	TLabel *lblUser;
	TLabel *lblOrgnization;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	void __fastcall btnOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmAbout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAbout *frmAbout;
//---------------------------------------------------------------------------
#endif
