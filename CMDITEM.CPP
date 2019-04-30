//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CMDITEM.h"
#include "CMDChild.H"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmItem *frmItem;

extern char *strAppName;
extern char *strMSGName;
extern char *strMSGKey;
extern char *strMSGCommand;
extern char *strMSGDelete;
extern char *strMSGSave;

//---------------------------------------------------------------------------
bool RefreshItems (CMDItem &cCMDItem, TListView * lvwChild)
{
    CMDChild	*lpCMDChild;
    TListItem	*lpListItem;

    lvwChild->Items->Clear ();
	lvwChild->Items->BeginUpdate ();
    lpCMDChild = cCMDItem.lpChildItem;
    while (lpCMDChild != NULL)
    {
        lpListItem = lvwChild->Items->Add ();

        if (lpCMDChild->strCommand.IsEmpty ())
        {
        	frmChild->cmbCommand->ItemIndex = lpCMDChild->uFunction;
        	lpListItem->Caption = frmChild->cmbCommand->Text;
        }
        else
	        lpListItem->Caption = lpCMDChild->strCommand;

        frmChild->cmbCmdShow->ItemIndex = lpCMDChild->uCmdShow;
        lpListItem->SubItems->Add (frmChild->cmbCmdShow->Text);

        lpCMDChild->nID = lpListItem->Index;

	    lpCMDChild = lpCMDChild->lpNextChild;
    }
    lvwChild->Items->EndUpdate ();
    return true;
}
//---------------------------------------------------------------------------
__fastcall TfrmItem::TfrmItem(TComponent* Owner)
	: TForm(Owner)
{
	fResult = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmItem::btnCancelClick(TObject *Sender)
{
	Close ();
}
//---------------------------------------------------------------------------
void __fastcall TfrmItem::btnOKClick(TObject *Sender)
{
	if (txtName->Text.IsEmpty ())
    {
    	Application->MessageBox (strMSGName, strAppName, MB_OK + MB_ICONEXCLAMATION);
    	txtName->SetFocus ();
    	return;
    }

    if (hkyCmdKey->HotKey == NULL)
    {
    	Application->MessageBox (strMSGKey, strAppName, MB_OK + MB_ICONEXCLAMATION);
        hkyCmdKey->SetFocus ();
    	return;
    }

    if (cmbCommand->Text.IsEmpty ())
    {
    	Application->MessageBox (strMSGCommand, strAppName, MB_OK + MB_ICONEXCLAMATION);
        cmbCommand->SetFocus ();
    	return;
	}

	cCMDItem.strName = txtName->Text;
    cCMDItem.uVirtualKey = hkyCmdKey->HotKey;
    cCMDItem.fDblClick = (chkDblClick->State == cbChecked);
    cCMDItem.fEnableFirst = (chkEnableFirst->State == cbChecked);
    cCMDItem.uMod = (hkyCmdKey->Modifiers.Contains(hkAlt) ? MOD_ALT : 0) | \
    				(hkyCmdKey->Modifiers.Contains(hkShift) ? MOD_SHIFT : 0) | \
                    (hkyCmdKey->Modifiers.Contains(hkCtrl) ? MOD_CONTROL : 0);
    cCMDItem.tTime = dtkAutoStart->Time;
    if (cmbCommand->ItemIndex >=0)
    {
	    cCMDItem.strCommand = "";
        cCMDItem.strDirectory = "";
        cCMDItem.uFunction = cmbCommand->ItemIndex;
    }
    else
    {
	    cCMDItem.strCommand = cmbCommand->Text;
        cCMDItem.strDirectory = txtDirectory->Text;
        cCMDItem.uFunction = -1;
    }
    cCMDItem.uCmdShow = cmbCmdShow->ItemIndex;
    fResult = true;
    Close ();
}

//---------------------------------------------------------------------------
void __fastcall TfrmItem::FormShow(TObject *Sender)
{
	txtName->Text = cCMDItem.strName;
    hkyCmdKey->HotKey = (WORD)cCMDItem.uVirtualKey;
	chkDblClick->State = (cCMDItem.fDblClick ? cbChecked : cbUnchecked);
    chkEnableFirst->State = (cCMDItem.fEnableFirst ? cbChecked : cbUnchecked);
    if (cCMDItem.uFunction >= 0)
        cmbCommand->ItemIndex = cCMDItem.uFunction;
    else
    {
        cmbCommand->ItemIndex = -1;
	    cmbCommand->Text = cCMDItem.strCommand;
    }
	txtDirectory->Text = cCMDItem.strDirectory;

    cmbCmdShow->ItemIndex = cCMDItem.uCmdShow;

	RefreshItems (cCMDItem, lvwChild);

    txtName->SetFocus ();
}
//---------------------------------------------------------------------------




void __fastcall TfrmItem::btnNewClick(TObject *Sender)
{
    frmChild->cCMDChild.strCommand = "";
    frmChild->cCMDChild.strDirectory = "";
    frmChild->cCMDChild.uFunction = -1;
    frmChild->cCMDChild.uCmdShow = 0;
    frmChild->fResult = false;

	if ((Left + (Width - frmChild->Width)/2) < Screen->Width && (Top + (Height - frmChild->Height)/2) < Screen->Height)
		MoveWindow (frmChild->Handle, Left + (Width - frmChild->Width)/2, Top + (Height - frmChild->Height)/2, frmChild->Width, frmChild->Height, FALSE);
	frmChild->ShowModal ();

    if (frmChild->fResult)
    {
    	// Add New
        CMDChild	*lpNewChild;

        lpNewChild = new(CMDChild);
        if (lpNewChild != NULL)
        {
            TListItem   *lpListItem;
            CMDChild	*lpEndItem;

        	*lpNewChild = frmChild->cCMDChild;
            lpListItem = lvwChild->Items->Add ();

            lpListItem->Caption = frmChild->cmbCommand->Text;
            lpListItem->SubItems->Add (frmChild->cmbCmdShow->Text);

			lpNewChild->nID = lpListItem->Index;

            lpNewChild->lpNextChild = NULL;
            if (cCMDItem.lpChildItem != NULL)
            {
	            lpEndItem = cCMDItem.lpChildItem;
    	        while (lpEndItem->lpNextChild != NULL)
                	lpEndItem = lpEndItem->lpNextChild;
                lpEndItem->lpNextChild = lpNewChild;
            }
            else
             	cCMDItem.lpChildItem = lpNewChild;

            cCMDItem.nChildCount = cCMDItem.nChildCount + 1;

            lvwChild->Selected = lpListItem;
            lpListItem->Focused = true;

	        cCMDItem.fChildModified = true;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmItem::btnEditClick(TObject *Sender)
{
    CMDChild	   *lpCMDChild;

    if (lvwChild->Selected == NULL)
    	return;

    lpCMDChild = cCMDItem.lpChildItem;
    while (lpCMDChild != NULL)
    {
    	if (lpCMDChild->nID == lvwChild->Selected->Index)
        {
        	frmChild->cCMDChild = *lpCMDChild;
		    frmChild->fResult = false;
			if ((Left + (Width - frmChild->Width)/2) < Screen->Width && (Top + (Height - frmChild->Height)/2) < Screen->Height)
				MoveWindow (frmChild->Handle, Left + (Width - frmItem->Width)/2, Top + (Height - frmChild->Height)/2, frmChild->Width, frmChild->Height, FALSE);
			frmChild->ShowModal ();
		    if (frmChild->fResult && *lpCMDChild != frmChild->cCMDChild)
		    {
                *lpCMDChild = frmChild->cCMDChild;

                lvwChild->Selected->Caption = frmChild->cmbCommand->Text;
                lvwChild->Selected->SubItems->Strings [0] = frmChild->cmbCmdShow->Text;

                cCMDItem.fChildModified = true;
    	    }
            return;
        }
        lpCMDChild = lpCMDChild->lpNextChild;
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmItem::btnDeleteClick(TObject *Sender)
{
    CMDChild		*lpCMDChild;
    CMDChild		*lpLastChild;

    if (lvwChild->Selected == NULL)
    	return;

    lpCMDChild = cCMDItem.lpChildItem;
    lpLastChild = NULL;
    while (lpCMDChild != NULL)
    {
    	if (lpCMDChild->nID == lvwChild->Selected->Index)
        {
        	if (Application->MessageBox (strMSGDelete, strAppName, MB_YESNO + MB_ICONQUESTION + MB_DEFBUTTON2) == IDYES)
            {
            	if (lpLastChild != NULL)
	            	lpLastChild->lpNextChild = lpCMDChild->lpNextChild;
                else
                	cCMDItem.lpChildItem = lpCMDChild->lpNextChild;
                delete (lpCMDChild);

                lpCMDChild = cCMDItem.lpChildItem;
                while (lpCMDChild != NULL)
                {
                	if (lpCMDChild->nID >= lvwChild->Selected->Index)
	                	lpCMDChild->nID --;
                	lpCMDChild = lpCMDChild->lpNextChild;
                }
                lvwChild->Selected->Focused = false;
                lvwChild->Items->Delete (lvwChild->Selected->Index);

	            cCMDItem.nChildCount = cCMDItem.nChildCount - 1;
                cCMDItem.fChildModified = true;
	            return;
            }
        }
        lpLastChild = lpCMDChild;
        lpCMDChild = lpCMDChild->lpNextChild;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmItem::btnBrowseClick(TObject *Sender)
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


