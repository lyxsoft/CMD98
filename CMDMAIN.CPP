//---------------------------------------------------------------------------
#include <vcl.h>
#include <registry.hpp>
#include <shellapi.h>

#pragma hdrstop

#include "CMDMAIN.h"
#include "CMDDATA.H"
#include "CMDITEM.H"
#include "ABOUT.H"
#include "SHOWWND.H"
#include "CMDCHILD.H"
#include "STARTUP.H"

extern TfrmItem *frmItem;
extern TfrmAbout *frmAbout;
extern TfrmShowWnd *frmShowWnd;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

//-------------------------------------
// Extern variants defined in CMD98.CPP
extern char *strAppName;
extern const char *strHomepage;
extern const char *strRegKey;

extern char *strMSGDelete;
extern char *strMSGSave;
extern char *strMSGErrSave;
extern char *strMSGErrLoad;
extern char *strMSGNotCMDFile;
extern void	DisplayErrMessage (void);
//-------------------------------------
const CST_DBLCLICKTIME = 250;

static CMDItem *lpCMDItemChain = NULL;
static bool	fDirty = false;
static bool	fEnable = true;

//---------------------------------------------------------------------------
bool Key2Str (UINT Key, UINT uMod, char *strBuffer)
{
	Key = Key & 0xFF;

	if ((uMod & MOD_CONTROL) != NULL)
    {
    	lstrcpy (strBuffer, "Ctrl+");
        strBuffer += 5;
    }
	if ((uMod & MOD_ALT) != NULL)
    {
    	lstrcpy (strBuffer, "Alt+");
        strBuffer += 4;
    }
	if ((uMod & MOD_SHIFT) != NULL)
    {
    	lstrcpy (strBuffer, "Shift+");
        strBuffer += 6;
    }

	if ((Key >= 'A' && Key <= 'Z') || (Key >= '0' && Key <= '9'))
    {
    	strBuffer [0] = (char)Key;
        strBuffer [1] = '\0';
    }
    else if (Key >= 0x60 && Key <= 0x69)
    {
    	lstrcpy (strBuffer, "Num-");
    	strBuffer [4] = (char)(Key - 0x30);
        strBuffer [5] = '\0';
    }
    else if (Key >= 0x70 && Key <= 0x7b)
    {
    	wsprintf (strBuffer, "F%d", Key - 0x6f);
    }
    else
    {
    	switch (Key)
        {
        case 0x08:
        	lstrcpy (strBuffer, "Backspace");
            return true;

        case 0x09:
        	lstrcpy (strBuffer, "Tab");
            return true;

        case 0x0c:
        	lstrcpy (strBuffer, "Num^5");
            return true;

        case 0x0d:
        	lstrcpy (strBuffer, "Enter");
            return true;
        case 0x13:
        	lstrcpy (strBuffer, "Break");
            return true;
        case 0x14:
        	lstrcpy (strBuffer, "Caps Lock");
            return true;
        case 0x1b:
        	lstrcpy (strBuffer, "Esc");
            return true;

        case 0x20:
        	lstrcpy (strBuffer, "Space");
            return true;
        case 0x21:
        	lstrcpy (strBuffer, "PageUp");
            return true;
        case 0x22:
        	lstrcpy (strBuffer, "PageDown");
            return true;
        case 0x23:
        	lstrcpy (strBuffer, "End");
            return true;
        case 0x24:
        	lstrcpy (strBuffer, "Home");
            return true;
        case 0x25:
        	lstrcpy (strBuffer, "Left");
            return true;
        case 0x26:
        	lstrcpy (strBuffer, "Up");
            return true;
        case 0x27:
        	lstrcpy (strBuffer, "Right");
            return true;
        case 0x28:
        	lstrcpy (strBuffer, "Down");
            return true;
        case 0x2c:
        	lstrcpy (strBuffer, "PrintScreen");
            return true;
        case 0x2d:
        	lstrcpy (strBuffer, "Insert");
            return true;
        case 0x2e:
        	lstrcpy (strBuffer, "Delete");
            return true;

        case 0x90:
        	lstrcpy (strBuffer, "Num Lock");
            return true;
        case 0x91:
        	lstrcpy (strBuffer, "Scroll Lock");
            return true;

        case 0xba:
        	strBuffer [0] = ';';
        	break;
        case 0xbb:
        	strBuffer [0] = '=';
        	break;
        case 0xbc:
        	strBuffer [0] = ',';
        	break;
        case 0xbd:
        	strBuffer [0] = '-';
        	break;
        case 0xbe:
        	strBuffer [0] = '.';
        	break;
        case 0xbf:
        	strBuffer [0] = '/';
        	break;

        case 0xdb:
        	strBuffer [0] = '[';
        	break;
        case 0xdc:
        	strBuffer [0] = '\\';
        	break;
        case 0xdd:
        	strBuffer [0] = ']';
        	break;

        case 0xe2:
        case 0xde:
        	strBuffer [0] = '\'';
        	break;

        case 0xc0:
        	strBuffer [0] = '`';
        	break;

        case 0x6f:
        	strBuffer [0] = '/';
        	break;
        case 0x6a:
        	strBuffer [0] = '*';
        	break;
        case 0x6d:
        	strBuffer [0] = '-';
        	break;
        case 0x6b:
        	strBuffer [0] = '+';
        	break;
        default:
        	//wsprintf (strBuffer, "%08lx", Key);
        	strBuffer [0] = '\0';
            return true;
        }
        strBuffer [1] = '\0';
    }
    return true;
}
//---------------------------------------------------------------------------
bool KillItems (void)
{
    CMDItem		*lpCMDItem;
	CMDChild	*lpCMDChild;

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	lpCMDItemChain = lpCMDItem->lpNextItem;
		lpCMDChild = lpCMDItem->lpChildItem;
        while (lpCMDChild != NULL)
        {
        	lpCMDItem->lpChildItem = lpCMDChild->lpNextChild;
            delete (lpCMDChild);
            lpCMDChild = lpCMDItem->lpChildItem;
        }
    	delete (lpCMDItem);
	    lpCMDItem = lpCMDItemChain;
    }
    return true;
}

//---------------------------------------------------------------------------
bool WriteRegistry (void)
{
	TRegistry*	lpRegistry = new (TRegistry);
    AnsiString	strSubKey;
    int			nItemCount;
    CMDItem*	lpCMDItem;
    CMDChild*	lpCMDChild;

	nItemCount = 1;
    while (true)
    {
    	strSubKey = strRegKey;
        strSubKey += nItemCount;
        if (!lpRegistry->DeleteKey (strSubKey))
        	break;
        nItemCount ++;
    }

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	strSubKey = strRegKey;
        strSubKey += lpCMDItem->nID + 1;
	    if (!lpRegistry->OpenKey (strSubKey, true))
    		break;

        lpRegistry->WriteString ("Name", lpCMDItem->strName);
        lpRegistry->WriteBool ("Enable", lpCMDItem->fEnable);
        lpRegistry->WriteInteger ("VirtualKey", (int)lpCMDItem->uVirtualKey);
        lpRegistry->WriteBool ("DblClick", lpCMDItem->fDblClick);
        lpRegistry->WriteBool ("EnableFirst", lpCMDItem->fEnableFirst);
        lpRegistry->WriteInteger ("Modifier", (int)lpCMDItem->uMod);
        lpRegistry->WriteString ("Command", lpCMDItem->strCommand);
        lpRegistry->WriteString ("Startup", lpCMDItem->strDirectory);
        lpRegistry->WriteInteger ("CmdShow", (int)lpCMDItem->uCmdShow);
        lpRegistry->WriteInteger ("Function", (int)lpCMDItem->uFunction);

        lpRegistry->CloseKey ();

        //Write children
		nItemCount = 1;
    	while (true)
	    {
    		strSubKey = strRegKey;
        	strSubKey += lpCMDItem->nID + 1;
            strSubKey += "\\";
    	    strSubKey += nItemCount;
	        if (!lpRegistry->DeleteKey (strSubKey))
        		break;
    	    nItemCount ++;
	    }

	    lpCMDChild = lpCMDItem->lpChildItem;
	    while (lpCMDChild != NULL)
	    {
    		strSubKey = strRegKey;
        	strSubKey += lpCMDItem->nID + 1;
            strSubKey += "\\";
            strSubKey += lpCMDChild->nID + 1;

		    if (!lpRegistry->OpenKey (strSubKey, true))
    			break;

    	    lpRegistry->WriteString ("Command", lpCMDChild->strCommand);
	        lpRegistry->WriteString ("Startup", lpCMDChild->strDirectory);
        	lpRegistry->WriteInteger ("CmdShow", (int)lpCMDChild->uCmdShow);
    	    lpRegistry->WriteInteger ("Function", (int)lpCMDChild->uFunction);

	        lpRegistry->CloseKey ();

	        lpCMDChild = lpCMDChild->lpNextChild;
        }
        lpCMDItem = lpCMDItem->lpNextItem;
    }
    fDirty = false;
    delete (lpRegistry);
    return true;
}

//---------------------------------------------------------------------------
bool ReadRegistry (void)
{
	TRegistry*	lpRegistry = new (TRegistry);
	AnsiString	strSubKey;
    int			nItemCount, nChildCount;
    CMDItem		cCMDItem, *lpEndItem;
    CMDChild	cCMDChild, *lpEndChild;


	nItemCount = 0;
    lpEndItem = NULL;
    while (true)
    {
    	nItemCount ++;

    	strSubKey = strRegKey;
        strSubKey += nItemCount;
	    if (!lpRegistry->OpenKey (strSubKey, false))
    		break;

        cCMDItem.nID = nItemCount - 1;
        if (lpRegistry->ValueExists ("Enable"))
	        cCMDItem.fEnable = lpRegistry->ReadBool ("Enable");
        else
        	cCMDItem.fEnable = true;

        if (lpRegistry->ValueExists ("Name"))
	        cCMDItem.strName = lpRegistry->ReadString ("Name");
        else
        	cCMDItem.strName = "No titled";

        if (lpRegistry->ValueExists ("VirtualKey"))
	        cCMDItem.uVirtualKey = (UINT)lpRegistry->ReadInteger ("VirtualKey");
        else
			continue;

        if (lpRegistry->ValueExists ("DblClick"))
    	    cCMDItem.fDblClick = lpRegistry->ReadBool ("DblClick");
        else
        	cCMDItem.fDblClick = false;

        if (lpRegistry->ValueExists ("EnableFirst"))
        	cCMDItem.fEnableFirst = lpRegistry->ReadBool ("EnableFirst");
        else
        	cCMDItem.fEnableFirst = false;

        if (lpRegistry->ValueExists ("Modifier"))
	        cCMDItem.uMod = (UINT)lpRegistry->ReadInteger ("Modifier");
        else
        	cCMDItem.uMod = 0;

        if (lpRegistry->ValueExists ("Command"))
    	    cCMDItem.strCommand = lpRegistry->ReadString ("Command");
        else
        	cCMDItem.strCommand = "";

        if (lpRegistry->ValueExists ("Startup"))
        	cCMDItem.strDirectory = lpRegistry->ReadString ("Startup");
        else
        	cCMDItem.strDirectory = "";

        if (lpRegistry->ValueExists ("CmdShow"))
	        cCMDItem.uCmdShow = (UINT)lpRegistry->ReadInteger ("CmdShow");
        else
        	cCMDItem.uCmdShow = 0;

        if (lpRegistry->ValueExists ("Function"))
    	    cCMDItem.uFunction = (UINT)lpRegistry->ReadInteger ("Function");
        else
        	cCMDItem.uFunction = -1;

        cCMDItem.lpChildItem = NULL;
		cCMDItem.fChildModified = false;

        cCMDItem.lpNextItem = NULL;

        if (!cCMDItem.strName.IsEmpty())
		{
            //Read Child
			nChildCount = 1;
		    lpEndChild = NULL;
            while (true)
            {
		    	strSubKey = strRegKey;
        		strSubKey += nItemCount;
                strSubKey += "\\";
                strSubKey += nChildCount;

			    if (!lpRegistry->OpenKey (strSubKey, false))
    				break;

		        cCMDChild.nID = nChildCount - 1;
        		cCMDChild.strCommand = lpRegistry->ReadString ("Command");
		        cCMDChild.strDirectory = lpRegistry->ReadString ("Startup");
		        cCMDChild.uCmdShow = (UINT)lpRegistry->ReadInteger ("CmdShow");
        		cCMDChild.uFunction = (UINT)lpRegistry->ReadInteger ("Function");
		        cCMDChild.lpNextChild = NULL;
    	    	if (lpEndChild != NULL)
        	    {
            		lpEndChild->lpNextChild = new (CMDChild);
                	lpEndChild = lpEndChild->lpNextChild;
	            }
    	        else
        	    {
	    		    cCMDItem.lpChildItem = new (CMDChild);
                	lpEndChild = cCMDItem.lpChildItem;
	            }
	            *lpEndChild = cCMDChild;
            	nChildCount ++;
            }

            cCMDItem.nChildCount = nChildCount - 1;

            //Add to the chain
        	if (lpEndItem != NULL)
            {
            	lpEndItem->lpNextItem = new (CMDItem);
                lpEndItem = lpEndItem->lpNextItem;
            }
            else
            {
	    	    lpCMDItemChain = new (CMDItem);
                lpEndItem = lpCMDItemChain;
            }
            *lpEndItem = cCMDItem;
        }
    }
    fDirty = false;
    delete (lpRegistry);
    return (nItemCount > 1);
}

//---------------------------------------------------------------------------
bool SetHotKey (HWND hWnd)
{
    CMDItem*	lpCMDItem;

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	UnregisterHotKey (hWnd, lpCMDItem->nID);
    	if (lpCMDItem->fEnable && lpCMDItem->uVirtualKey != NULL)
	    	RegisterHotKey (hWnd, lpCMDItem->nID, lpCMDItem->uMod, lpCMDItem->uVirtualKey & 0xFF);
	    lpCMDItem = lpCMDItem->lpNextItem;
    }
    return true;
}

//---------------------------------------------------------------------------
bool DisableHotKey (HWND hWnd)
{
    CMDItem*	lpCMDItem;

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	UnregisterHotKey ( hWnd, lpCMDItem->nID);
	    lpCMDItem = lpCMDItem->lpNextItem;
    }
    return true;
}
//---------------------------------------------------------------------------
bool RefreshItems (TListView * lvwMain)
{
    CMDItem*	lpCMDItem;
    TListItem	*lpListItem;
    char		strKeyNameBuffer [64];

    lvwMain->Items->Clear ();
	lvwMain->Items->BeginUpdate ();
    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
        lpListItem = lvwMain->Items->Add ();

        lpListItem->Caption = lpCMDItem->strName;

        lpListItem->ImageIndex = lpCMDItem->fEnable ? 4 : 5;

        Key2Str (lpCMDItem->uVirtualKey, lpCMDItem->uMod, strKeyNameBuffer);
        lpListItem->SubItems->Add (strKeyNameBuffer);

        if (lpCMDItem->strCommand.IsEmpty ())
        {
        	frmItem->cmbCommand->ItemIndex = lpCMDItem->uFunction;
        	lpListItem->SubItems->Add (frmItem->cmbCommand->Text);
        }
        else
	        lpListItem->SubItems->Add (lpCMDItem->strCommand);

        frmItem->cmbCmdShow->ItemIndex = lpCMDItem->uCmdShow;
        lpListItem->SubItems->Add (frmItem->cmbCmdShow->Text);

        lpCMDItem->nID = lpListItem->Index;

	    lpCMDItem = lpCMDItem->lpNextItem;
    }
    lvwMain->Items->EndUpdate ();
    return true;
}

//---------------------------------------------------------------------------
bool __fastcall IsMyself (HWND hWindow)
{
	while (hWindow != NULL)
    {
		if (hWindow == frmMain->Handle		||
    		hWindow == frmAbout->Handle		||
            hWindow == frmItem->Handle		||
            hWindow == frmChild->Handle		||
            hWindow == frmShowWnd->Handle)
        	return true;

        hWindow = GetWindow (hWindow, GW_OWNER);
    }

    return false;
}
//---------------------------------------------------------------------------
bool __fastcall RunCommand (AnsiString &strCommand, AnsiString &strDirectory, UINT uCmdShow, int uFunction)
{
	if (!fEnable && uFunction != 4)
    	return false;

	if (uFunction >= 0)
    {
    	//	Functions
    	HWND	hWorkWnd;


        //	Get the window handle for deal with
        hWorkWnd = GetForegroundWindow ();

        switch (uFunction)
        {
        case	0:	//Sizeing
        	if (!IsMyself (hWorkWnd))
            {
        		switch (uCmdShow)
	            {
    	        case	0:	//Normal
        	    	ShowWindowAsync (hWorkWnd, SW_RESTORE);
            	    break;
	            case	1:	//Maximize
    	        	ShowWindowAsync (hWorkWnd, SW_MAXIMIZE);
        	        break;
            	case	2:	//Minimize
            		ShowWindowAsync (hWorkWnd, SW_MINIMIZE);
	                break;
    	        }
            }
            break;

        case	1:	//Hide
        	if (!IsMyself (hWorkWnd))
            {
            	frmShowWnd->Add (hWorkWnd);
            	while (hWorkWnd != NULL)
                {
		            ShowWindowAsync (hWorkWnd, SW_HIDE);
    		        ShowOwnedPopups (hWorkWnd, FALSE);

                    hWorkWnd = GetWindow (hWorkWnd, GW_OWNER);
                }
            }
        	break;

        case	2:	//Show Hide
        	frmShowWnd->ShowLast ();
            break;

        case	3:	//Closing
        	if (!IsMyself (hWorkWnd))
	        	PostMessage (hWorkWnd, WM_CLOSE, NULL, NULL);
            break;

        case	4:	//Enable/Disable Commander
        	frmMain->mnuItemDisableAllClick (NULL);
            break;

        default:
        	return false;
        }
    }
    else
    {
    	//	Command
        INT	nShowCmd;

        switch (uCmdShow)
        {
        case	0:
        	nShowCmd = SW_RESTORE;
            break;
        case	1:
        	nShowCmd = SW_MAXIMIZE;
            break;
        case	2:
            nShowCmd = SW_MINIMIZE;
            break;
        }

        if ((long)ShellExecute (	NULL, NULL,
        				strCommand.c_str(), NULL,
                        (strDirectory.IsEmpty() ? NULL : strDirectory.c_str ()),
						nShowCmd) <= 32)
        {
        	DisplayErrMessage ();
        }
    }
    return true;
}
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	NOTIFYICONDATA	cNotifyIconData;

    cNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    cNotifyIconData.hWnd = Handle;
    cNotifyIconData.uID = 1;
    cNotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    cNotifyIconData.uCallbackMessage = WM_USER;
    cNotifyIconData.hIcon = Icon->Handle;
    lstrcpy (cNotifyIconData.szTip, strAppName);
	Shell_NotifyIcon (NIM_ADD, &cNotifyIconData);

    if (ReadRegistry ())
    	SetHotKey (Handle);

    mnuItemDisableAll->Caption = fEnable ? "Disable all commands" : "Enable all commands";
    mnuPMNDisable->Caption = mnuItemDisableAll->Caption;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::SendKey (UINT uVirtualKey, UINT uMod)
{
	DWORD	dwThisID, dwFocusID;

    dwThisID = GetWindowThreadProcessId (Handle, NULL);
    dwFocusID = GetWindowThreadProcessId (GetForegroundWindow (), NULL);
    if (AttachThreadInput (dwThisID, dwFocusID, TRUE) != FALSE)
	{
    	LPARAM	lParam;

    	lParam = (MapVirtualKey (uVirtualKey, 0) >> 16) | ((uMod & (MOD_ALT | MOD_CONTROL)) != NULL ? 0x1000000 : 0);

        PostMessage (GetFocus(), WM_KEYDOWN, uVirtualKey, lParam);
		AttachThreadInput (dwThisID, dwFocusID, FALSE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::WndProc (Messages::TMessage &Message)
{
	static long		lLastTime;
	static WPARAM	nLastID;

	switch (Message.Msg)
    {
    case	WM_HOTKEY:
    	CMDItem	*lpCMDItem;
        CMDChild *lpCMDChild;

        lpCMDItem = lpCMDItemChain;
        while (lpCMDItem != NULL)
        {
        	if (lpCMDItem->nID == Message.WParam && lpCMDItem->fEnable)
            {
            	if (lpCMDItem->fDblClick)
                {
                	if ((Word)nLastID != Message.WParam ||
                    	(GetMessageTime() - lLastTime) > CST_DBLCLICKTIME)
                    {
		                nLastID = Message.WParam;
    		            lLastTime = GetMessageTime ();
                        if (lpCMDItem->fEnableFirst)
	                        SendKey (Message.LParamHi, Message.LParamLo);
		                Message.Result = 0;
                        return;
                    }
                }
				if (RunCommand (lpCMDItem->strCommand, \
	                			lpCMDItem->strDirectory, \
    	                        lpCMDItem->uCmdShow, \
        	                    lpCMDItem->uFunction))
                {
					// Run childs
    	            lpCMDChild = lpCMDItem->lpChildItem;
        	        while (lpCMDChild != NULL)
            	    {
                		RunCommand (lpCMDChild->strCommand, \
	                				lpCMDChild->strDirectory, \
    	                	        lpCMDChild->uCmdShow, \
        	                	    lpCMDChild->uFunction);

	                    lpCMDChild = lpCMDChild->lpNextChild;
    	            }
                }

                Message.Result = 0;
                nLastID = Message.WParam;
                lLastTime = -1;
            	return;
            }
            lpCMDItem = lpCMDItem->lpNextItem;
        }
        Message.Result = 0;
        nLastID = -1;
        lLastTime = -1;
    	break;

    case	WM_USER:	//NotifyIcon Message
    	if (Message.LParam == WM_LBUTTONUP)
        {
			mnuPMNShowClick (this);
        }
        else if (Message.LParam == WM_RBUTTONUP)
        {
            POINT	CursorPos;

            GetCursorPos (&CursorPos);
            pmnMain->Popup (CursorPos.x, CursorPos.y);
        }
        break;

    case	WM_SYSCOMMAND:
    	if (Message.WParam == SC_MINIMIZE)
        {
        	Hide ();
            Message.Result = 0;
        }
        else
	    	TForm::WndProc (Message);
        break;

    case	WM_GETMINMAXINFO:
		if (Message.LParam != NULL)
        {
			((LPMINMAXINFO)(Message.LParam))->ptMinTrackSize.x = 521 + 2 * GetSystemMetrics (SM_CXSIZEFRAME);
            ((LPMINMAXINFO)(Message.LParam))->ptMinTrackSize.y = 300;
        }
        break;

    default:
    	TForm::WndProc (Message);
        break;
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSystemExitClick(TObject *Sender)
{
	Application->MainForm->Close ();
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemNewClick(TObject *Sender)
{
    frmItem->cCMDItem.strName = "";
    frmItem->cCMDItem.uVirtualKey = 0;
    frmItem->cCMDItem.fDblClick = false;
    frmItem->cCMDItem.fEnableFirst = false;
    frmItem->cCMDItem.uMod = 0;
    frmItem->cCMDItem.strCommand = "";
    frmItem->cCMDItem.strDirectory = "";
    frmItem->cCMDItem.uFunction = -1;
    frmItem->cCMDItem.uCmdShow = 0;
    frmItem->cCMDItem.lpChildItem = 0;
    frmItem->cCMDItem.nChildCount = 0;
    frmItem->cCMDItem.fChildModified = false;
    frmItem->fResult = false;

	if ((Left + (Width - frmItem->Width)/2) < Screen->Width && (Top + (Height - frmItem->Height)/2) < Screen->Height)
		MoveWindow (frmItem->Handle, Left + (Width - frmItem->Width)/2, Top + (Height - frmItem->Height)/2, frmItem->Width, frmItem->Height, FALSE);

	frmItem->ShowModal ();
    if (frmItem->fResult)
    {
    	// Add New
        CMDItem	*lpNewItem;

        lpNewItem = new(CMDItem);
        if (lpNewItem != NULL)
        {
        	*lpNewItem = frmItem->cCMDItem;
            lpNewItem->lpNextItem = lpCMDItemChain;
            lpCMDItemChain = lpNewItem;

            char		strBuffer [64];
            TListItem   *lpListItem;

            lpListItem = lvwMain->Items->Add ();

            lpListItem->Caption = lpNewItem->strName;
            lpListItem->ImageIndex = 4;

            Key2Str (lpNewItem->uVirtualKey, lpNewItem->uMod, strBuffer);
            lpListItem->SubItems->Add (strBuffer);
            lpListItem->SubItems->Add (frmItem->cmbCommand->Text);
            lpListItem->SubItems->Add (frmItem->cmbCmdShow->Text);

			lpNewItem->nID = lpListItem->Index;
            lpNewItem->fEnable = true;

            lvwMain->Selected = lpListItem;
            lpListItem->Focused = true;

            SetHotKey (Handle);
	        fDirty = true;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemEditClick(TObject *Sender)
{
    CMDItem		*lpCMDItem;

    if (lvwMain->Selected == NULL)
    	return;

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	if (lpCMDItem->nID == lvwMain->Selected->Index)
        {
        	frmItem->cCMDItem = *lpCMDItem;
		    frmItem->fResult = false;
			if ((Left + (Width - frmItem->Width)/2) < Screen->Width && (Top + (Height - frmItem->Height)/2) < Screen->Height)
				MoveWindow (frmItem->Handle, Left + (Width - frmItem->Width)/2, Top + (Height - frmItem->Height)/2, frmItem->Width, frmItem->Height, FALSE);
			frmItem->ShowModal ();
		    if (frmItem->fResult && (*lpCMDItem != frmItem->cCMDItem || frmItem->cCMDItem.fChildModified))
		    {
            	char	strBuffer [64];

                *lpCMDItem = frmItem->cCMDItem;
			    lpCMDItem->fChildModified = false;

                lvwMain->Selected->Caption = lpCMDItem->strName;
                Key2Str (lpCMDItem->uVirtualKey, lpCMDItem->uMod, strBuffer);
                lvwMain->Selected->SubItems->Strings [0] = strBuffer;
                lvwMain->Selected->SubItems->Strings [1] = frmItem->cmbCommand->Text;
                lvwMain->Selected->SubItems->Strings [2] = frmItem->cmbCmdShow->Text;

                if (lpCMDItem->fEnable)
					SetHotKey (Handle);
	            fDirty = true;
    	    }
            return;
        }
        lpCMDItem = lpCMDItem->lpNextItem;
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSystemSaveClick(TObject *Sender)
{
	if (fDirty)
//		WriteRegistry ();
	{
		CMDItem		*lpCMDItem;
		TRegistry*	lpRegistry = new (TRegistry);
	    int			nItemCount;

        lpRegistry->OpenKey (strRegKey, true);
		nItemCount = 1;
    	while (true)
	    {
	        if (!lpRegistry->DeleteKey (nItemCount))
    	    	break;
	        nItemCount ++;
    	}

	    lpCMDItem = lpCMDItemChain;
    	while (lpCMDItem != NULL)
	    {
        	lpCMDItem->SaveToRegistry (lpRegistry);
            lpCMDItem = lpCMDItem->lpNextItem;
    	}
        lpRegistry->CloseKey ();
        fDirty = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemDeleteClick(TObject *Sender)
{
    CMDItem		*lpCMDItem;
    CMDItem		*lpLastItem;

    if (lvwMain->Selected == NULL)
    	return;

    lpCMDItem = lpCMDItemChain;
    lpLastItem = NULL;
    while (lpCMDItem != NULL)
    {
    	if (lpCMDItem->nID == lvwMain->Selected->Index)
        {
        	if (Application->MessageBox (strMSGDelete, strAppName, MB_YESNO + MB_ICONQUESTION + MB_DEFBUTTON2) == IDYES)
            {
				DisableHotKey (Handle);
            	if (lpLastItem != NULL)
	            	lpLastItem->lpNextItem = lpCMDItem->lpNextItem;
                else
                	lpCMDItemChain = lpCMDItem->lpNextItem;
                delete (lpCMDItem);

                lpCMDItem = lpCMDItemChain;
                while (lpCMDItem != NULL)
                {
                	if (lpCMDItem->nID >= lvwMain->Selected->Index)
	                	lpCMDItem->nID --;
                	lpCMDItem = lpCMDItem->lpNextItem;
                }
                lvwMain->Selected->Focused = false;
                lvwMain->Items->Delete (lvwMain->Selected->Index);

				SetHotKey (Handle);
                fDirty = true;
	            return;
            }
        }
        lpLastItem = lpCMDItem;
        lpCMDItem = lpCMDItem->lpNextItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if (fDirty)
    {
    	switch (Application->MessageBox (strMSGSave, strAppName, MB_YESNOCANCEL + MB_ICONQUESTION))
        {
        case IDYES:
	    	mnuSystemSaveClick(Sender);
            CanClose = true;
            break;
        case IDCANCEL:
        	CanClose = false;
        	break;
        default:
        	CanClose = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	frmShowWnd->ShowAll ();
	DisableHotKey (Handle);
	KillItems ();

	NOTIFYICONDATA	cNotifyIconData;

    cNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    cNotifyIconData.hWnd = Handle;
    cNotifyIconData.uID = 1;
	Shell_NotifyIcon (NIM_DELETE, &cNotifyIconData);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemDisableAllClick(TObject *Sender)
{
	NOTIFYICONDATA	cNotifyIconData;

	fEnable = !fEnable;

    cNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    cNotifyIconData.hWnd = Handle;
    cNotifyIconData.uID = 1;
    cNotifyIconData.uFlags = NIF_ICON;

	if (!fEnable)
    {
		TIcon	*lpNewIcon = new(TIcon);

	    imgMain->GetIcon (5, lpNewIcon);
	    cNotifyIconData.hIcon = lpNewIcon->Handle;
		Shell_NotifyIcon (NIM_MODIFY, &cNotifyIconData);
	    delete (lpNewIcon);
    }
    else
    {
	    cNotifyIconData.hIcon = Icon->Handle;
		Shell_NotifyIcon (NIM_MODIFY, &cNotifyIconData);
    }
    mnuItemDisableAll->Caption = fEnable ? "Disable all commands" : "Enable all commands";
    mnuPMNDisable->Caption = mnuItemDisableAll->Caption;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::mnuHelpAboutClick(TObject *Sender)
{
	if ((Left + (Width - frmAbout->Width)/2) < Screen->Width && (Top + (Height - frmAbout->Height)/2) < Screen->Height)
		MoveWindow (frmAbout->Handle, Left + (Width - frmAbout->Width)/2, Top + (Height - frmAbout->Height)/2, frmAbout->Width, frmAbout->Height, FALSE);
	frmAbout->ShowModal ();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSystemShowWndClick(TObject *Sender)
{
	frmShowWnd->ShowModal ();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemDisableClick(TObject *Sender)
{
    CMDItem		*lpCMDItem;

    if (lvwMain->Selected == NULL)
    	return;

    lpCMDItem = lpCMDItemChain;
    while (lpCMDItem != NULL)
    {
    	if (lpCMDItem->nID == lvwMain->Selected->Index)
        {
        	lpCMDItem->fEnable = !lpCMDItem->fEnable;

	        lvwMain->Selected->ImageIndex = lpCMDItem->fEnable ? 4 : 5;

            UnregisterHotKey (Handle, lpCMDItem->nID);
        	if (lpCMDItem->fEnable && lpCMDItem->uVirtualKey != NULL)
            	RegisterHotKey (Handle, lpCMDItem->nID, lpCMDItem->uMod, lpCMDItem->uVirtualKey & 0xFF);

            fDirty = true;
            return;
        }
        lpCMDItem = lpCMDItem->lpNextItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuItemClick(TObject *Sender)
{
    if (lvwMain->Selected == NULL)
    {
    	mnuItemDisable->Enabled = false;
        mnuItemEdit->Enabled = false;
        mnuItemDelete->Enabled = false;
        mnuItemDisable->Enabled = false;
    }
    else
    {
	    CMDItem		*lpCMDItem;

	    lpCMDItem = lpCMDItemChain;
    	while (lpCMDItem != NULL)
	    {
    		if (lpCMDItem->nID == lvwMain->Selected->Index)
        	{
		    	mnuItemDisable->Enabled = true;
        		mnuItemEdit->Enabled = true;
		        mnuItemDelete->Enabled = true;
                mnuItemDisable->Enabled = true;

				mnuItemDisable->Caption = lpCMDItem->fEnable ? "D&isable" : "E&nable";
	            break;
    	    }
        	lpCMDItem = lpCMDItem->lpNextItem;
	    }
    }
    mnuItemDisableAll->Caption = fEnable ? "Disable all commands" : "Enable all commands";
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuHelpContentClick(TObject *Sender)
{
	Application->HelpContext (100);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSystemSaveFileClick(TObject *Sender)
{
	try {
		CMDItem		*lpCMDItem;
    	TFileStream	*lpFileStream;

	    if (dlgSaveReg->Execute () && !dlgSaveReg->FileName.IsEmpty())
    	{
			lpFileStream = new(TFileStream)(dlgSaveReg->FileName, fmCreate | fmOpenWrite);

            lpFileStream->WriteBuffer (strAppName, lstrlen(strAppName));

            lpCMDItem = lpCMDItemChain;
            while (lpCMDItem != NULL)
            {
            	lpCMDItem->SaveToStream (lpFileStream);
                lpCMDItem = lpCMDItem->lpNextItem;
            }
            delete lpFileStream;
    	}
    }
    catch (...)
    {
    	Application->MessageBox (strMSGErrSave, strAppName, MB_OK | MB_ICONEXCLAMATION);
    	return;
    }
    return;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnuSystemLoadFileClick(TObject *Sender)
{
	try	{
	    TFileStream	*lpFileStream;
    	char		strBuffer [21];
		CMDItem		*lpCMDItem, *lpLastItem;

    	if (dlgLoadReg->Execute () && !dlgLoadReg->FileName.IsEmpty())
	    {
			lpFileStream = new(TFileStream)(dlgSaveReg->FileName, fmOpenRead | fmShareDenyWrite);

        	lpFileStream->ReadBuffer (strBuffer, lstrlen(strAppName));
	        strBuffer [lstrlen (strAppName)] = '\0';
    	    if (lstrcmp (strBuffer, strAppName) != 0)
        	{
        		delete (lpFileStream);
	            Application->MessageBox (strMSGNotCMDFile, strAppName, MB_OK | MB_ICONEXCLAMATION);
    	        return;
        	}

			DisableHotKey (Handle);
    	    KillItems ();
        	lpLastItem = NULL;
	        while (lpFileStream->Position < lpFileStream->Size)
    	    {
        		lpCMDItem = new(CMDItem);
            	if (lpCMDItem == NULL)
            		break;

	            if (!lpCMDItem->LoadFromStream (lpFileStream))
    	        {
        	    	delete (lpCMDItem);
            		break;
	            }
    	        lpCMDItem->lpNextItem = NULL;

        	    if (lpLastItem == NULL)
            		lpCMDItemChain = lpCMDItem;
	            else
    	        	lpLastItem->lpNextItem = lpCMDItem;
        	    lpLastItem = lpCMDItem;
	        }
    	    delete (lpFileStream);

        	RefreshItems (lvwMain);
	        SetHotKey (Handle);
    	    fDirty = true;
	    }
    }
    catch (...)
    {
    	Application->MessageBox (strMSGErrLoad, strAppName, MB_OK | MB_ICONEXCLAMATION);
    	return;
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::mnuPMNShowClick(TObject *Sender)
{
//	WindowState = wsNormal;
	Show ();
//	Application->Restore ();
    SetForegroundWindow (Handle);
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormShow(TObject *Sender)
{
    RefreshItems (lvwMain);
    OnShow = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnuHelpHomepageClick(TObject *Sender)
{
    ShellExecute (NULL, NULL, strHomepage, NULL, NULL, SW_MAXIMIZE);
}
//---------------------------------------------------------------------------

