//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SHOWWND.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmShowWnd *frmShowWnd;
struct HWNDINFO {	HWND		hWnd;
					HWNDINFO	*lpNextItem;
				};

static HWNDINFO	*lpHWndChain=NULL;

//---------------------------------------------------------------------------
__fastcall TfrmShowWnd::TfrmShowWnd(TComponent* Owner)
	: TForm(Owner)
{
	lpHWndChain = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmShowWnd::Add (HWND hAddWnd)
{
	if (hAddWnd != NULL)
    {
		HWNDINFO	*lpNewItem;

    	lpNewItem = new(HWNDINFO);
	    lpNewItem->hWnd = hAddWnd;

    	lpNewItem->lpNextItem = lpHWndChain;
	    lpHWndChain = lpNewItem;
    }
}
void __fastcall TfrmShowWnd::ShowLast (void)
{
	HWNDINFO	*lpItem;
    HWND		hWorkWnd;

    if (lpHWndChain != NULL)
    {
        lpItem = lpHWndChain;
        hWorkWnd = lpItem->hWnd;
        lpHWndChain = lpItem->lpNextItem;
		delete (lpItem);

        while (hWorkWnd != NULL)
        {
		    ShowWindowAsync (hWorkWnd, SW_SHOW);
    	    ShowOwnedPopups (hWorkWnd, TRUE);
        	SetForegroundWindow (hWorkWnd);
            hWorkWnd = GetWindow (hWorkWnd, GW_OWNER);
        }
    }
}

void __fastcall TfrmShowWnd::ShowAll (void)
{
	HWNDINFO	*lpItem;

    lpItem = lpHWndChain;
    while (lpItem != NULL)
    {
	    ShowWindowAsync (lpItem->hWnd, SW_SHOW);
        ShowOwnedPopups (lpItem->hWnd, TRUE);
        SetForegroundWindow (lpItem->hWnd);
        lpHWndChain = lpItem->lpNextItem;
		delete (lpItem);
	    lpItem = lpHWndChain;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmShowWnd::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	HWNDINFO	*lpNextItem;

	while (lpHWndChain != NULL)
    {
		lpNextItem = lpHWndChain->lpNextItem;
        delete (lpHWndChain);
        lpHWndChain = lpNextItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmShowWnd::FormShow(TObject *Sender)
{
	HWNDINFO	*lpItem;
    HWNDINFO	*lpLastItem;
	char		strBuffer [256];

	lstWindow->Items->Clear ();
    lpItem = lpHWndChain;
    lpLastItem = NULL;
    while (lpItem != NULL)
    {
    	if (IsWindow(lpItem->hWnd) == NULL || IsWindowVisible (lpItem->hWnd) != NULL)
        {
        	//Window is visible or not exist

            if (lpLastItem != NULL)
            {
            	lpLastItem->lpNextItem = lpItem->lpNextItem;
                delete (lpItem);
                lpItem = lpLastItem;
            }
            else
            {
            	lpHWndChain = lpItem->lpNextItem;
	            delete (lpItem);
    	        lpItem = lpHWndChain;
                lpLastItem = NULL;
                continue;
            }
        }
        else
        {
			GetWindowText (lpItem->hWnd, strBuffer, 255);
            lstWindow->Items->Add (strBuffer);
        }
        lpLastItem = lpItem;
    	lpItem = lpItem->lpNextItem;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmShowWnd::lstWindowDblClick(TObject *Sender)
{
    int			nItemCount;
    HWNDINFO	*lpLastItem;
	HWNDINFO	*lpItem;

    if (lstWindow->ItemIndex < 0)
    	return;

    lpLastItem = NULL;
    lpItem = lpHWndChain;
    for (nItemCount = 1; nItemCount <= lstWindow->ItemIndex; nItemCount++)
    {
    	if (lpItem == NULL)
        	return;
		lpLastItem = lpItem;
    	lpItem = lpItem->lpNextItem;
    }
    if (lpItem != NULL)
    {
	    ShowWindowAsync (lpItem->hWnd, SW_SHOW);
        ShowOwnedPopups (lpItem->hWnd, TRUE);
        SetForegroundWindow (lpItem->hWnd);
    	if (lpLastItem == NULL)
    		lpHWndChain = lpItem->lpNextItem;
	    else
        	lpLastItem->lpNextItem = lpItem->lpNextItem;
        delete (lpItem);
	}
    Close ();
}
//---------------------------------------------------------------------------


