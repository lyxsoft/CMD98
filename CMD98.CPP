//---------------------------------------------------------------------------
#include <vcl.h>
#include <registry.hpp>

#include "CMDMAIN.H"
#include "STARTUP.H"
#pragma hdrstop

USEFORM("CMDMAIN.CPP", frmMain);
USERES("CMD98.res");
USEFORM("CMDITEM.CPP", frmItem);
USEFORM("STARTUP.CPP", frmStartup);
USEFORM("ABOUT.CPP", frmAbout);
USEFORM("SHOWWND.CPP", frmShowWnd);
USEFORM("CMDChild.cpp", frmChild);
USEUNIT("CMDDATA.CPP");
//---------------------------------------------------------------------------
extern char *strAppName = "LyxSoft Commander 98";
extern const char *strHomepage = "http://www.lyxsoft.f2s.com";
extern const char *strRegKey = "\\Software\\LyxSoft Software Inc.\\LyxSoft Commander 98\\";
extern char *strMSGName = "Please give a name to this item!";
extern char *strMSGKey = "Please set a hot-key for this item!\nPress the keys you want in the command key box.";
extern char *strMSGCommand = "Please input the running command of this item or choose a predefined command!";
extern char *strMSGDelete = "Are you sure you want to delete this item?";
extern char *strMSGSave = "Do you want to save the changes?";
extern char *strMSGRunning = "LyxSoft Commander 98 is already running.\nPlease click on the icon in the notify area of the taskbar to activate it.";
extern char *strMSGErrSave = "Error when save to the file!";
extern char *strMSGErrLoad = "Error when load from the file!";
extern char *strMSGNotCMDFile = "The file is not a Commander 98 file!";

//---------------------------------------------------------------------------
extern void	DisplayErrMessage (void)
{
	char	*strErrorMessage;

    FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM |  FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,
    				GetLastError (), NULL, (LPTSTR) &strErrorMessage, 0, NULL);
    Application->MessageBox (strErrorMessage, strAppName, MB_OK | MB_ICONEXCLAMATION);
    LocalFree (strErrorMessage);
}
//---------------------------------------------------------------------------
bool	UpgradeApp (void)
{
	char *strOldRegKey = "\\Software\\LyxSoft Software Inc.\\Commander\\";
	TRegistry	*lpRegistry = new (TRegistry);

    if (!lpRegistry->KeyExists (strRegKey) && lpRegistry->KeyExists (strOldRegKey))
    {
    	//Moving the key
        lpRegistry->MoveKey (strOldRegKey, strRegKey, true);
    }
    delete (lpRegistry);
	return true;
}
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "LyxSoft Commander 98";
    	Application->HelpFile = "Cmd98.hlp";
		Application->ShowMainForm = false;

//		while (GlobalFindAtom (strAppName) != NULL)
//        	GlobalDeleteAtom (GlobalFindAtom (strAppName));

		if (GlobalFindAtom (strAppName) != NULL)
        {
			Application->MessageBox (strMSGRunning, strAppName, MB_OK | MB_ICONEXCLAMATION);
        	return 0;
        }

		GlobalAddAtom (strAppName);
        if (!UpgradeApp ())
        	return 0;

		Application->CreateForm(__classid(TfrmStartup), &frmStartup);
        frmStartup->Show();
        frmStartup->Update();

		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmItem), &frmItem);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmShowWnd), &frmShowWnd);
		Application->CreateForm(__classid(TfrmChild), &frmChild);

        Sleep (1000);

        frmStartup->Hide();

		Application->Run();

        GlobalDeleteAtom (GlobalFindAtom (strAppName));
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
