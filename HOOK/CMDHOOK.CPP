//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be perfomring new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------
#include "CMDBUF.H"

struct CMDHOOKINFO	{
						HHOOK		hKeyHook;
						HWND		hMainWnd;
                        CMDBUFFER	CMDBuffer;
					};

static CMDHOOKINFO *lpHookInfo = NULL;
static HHOOK hKeyHook = NULL;
static HINSTANCE hInstance;

//---------------------------------------------------------------------------
extern __declspec(dllexport) LRESULT CALLBACK KeyboardProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (lpHookInfo == NULL)
    	return 0;
        
	if (nCode <0)
		return CallNextHookEx (lpHookInfo->hKeyHook, nCode, wParam, lParam);

    if ((lParam & 0x80000000L) != NULL && lpHookInfo != NULL && lpHookInfo->hMainWnd != NULL)
    {
    	int	nCmdCount;

        for (nCmdCount = 0; nCmdCount < MAXCMDS; nCmdCount ++)
        {
        	if (wParam == Getvirtualkeymap (lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey.wMainKey && \
				((GetKeyState(VK_SHIFT) & 0xFFFF0000) != NULL) == lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey.fShift &&
                ((GetKeyState(VK_CONTROL) & 0xFFFF0000) != NULL) == lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey.fCtrl &&
                ((GetKeyState(VK_MENU) & 0xFFFF0000) != NULL) == lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey.fAlt)
            {
            	MessageBox (NULL, lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].strCommand, "OK", MB_OK);
                return 1;
            }
        }
            {
            	char	strBuffer [32];

                wsprintf (strBuffer, "%08lx %08lx", wParam, lParam);
            	MessageBox (NULL, strBuffer, "OK", MB_OK);
            }
    }
    return 0;
}

//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hDllInstance, unsigned long lReason, void*)
{
    static HANDLE hMapObject = NULL;
    BOOL fInit;

    hInstance = hDllInstance;
	switch (lReason)
    {
    case DLL_PROCESS_ATTACH:
    	hMapObject = CreateFileMapping((HANDLE) 0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(CMDHOOKINFO), "CMDHOOKINFO");
		if (hMapObject == NULL)
        	return FALSE;

        fInit = (GetLastError() != ERROR_ALREADY_EXISTS);

        lpHookInfo = (CMDHOOKINFO *)MapViewOfFile (hMapObject, FILE_MAP_WRITE, 0, 0, 0);
        if (lpHookInfo == NULL)
        {
	        CloseHandle (hMapObject);
        	return FALSE;
        }

        if (fInit)
        	memset (lpHookInfo, '\0', sizeof(CMDHOOKINFO));
        break;

    case DLL_PROCESS_DETACH:
		UnmapViewOfFile (lpHookInfo);
        CloseHandle (hMapObject);

		if (hKeyHook != NULL)
        {
        	UnhookWindowsHookEx (hKeyHook);
            hKeyHook = NULL;
        }
        break;
    }
	return 1;
}

//---------------------------------------------------------------------------
extern __declspec(dllexport) int InitCMDHook (HWND hMainWnd)
{
	if (hKeyHook != NULL)
    {
    	UnhookWindowsHookEx (hKeyHook);
        hKeyHook = NULL;
    }

    if (lpHookInfo != NULL)
    {
    	memset (lpHookInfo, '\0', sizeof(CMDHOOKINFO));
		hKeyHook = SetWindowsHookEx (WH_KEYBOARD, (HOOKPROC)KeyboardProc, hInstance, 0);
        lpHookInfo->hKeyHook = hKeyHook;
        lpHookInfo->hMainWnd = hMainWnd;
    }
    return (hKeyHook != NULL);
}

//---------------------------------------------------------------------------
extern __declspec(dllexport) void ReleaseCMDHook (void)
{
	if (hKeyHook != NULL)
    {
    	UnhookWindowsHookEx (hKeyHook);
        hKeyHook = NULL;
    }
}

//---------------------------------------------------------------------------
extern __declspec(dllexport) BOOL RegisteCMDInfo (CMDINFO RegCMDInfo)
{
	if (lpHookInfo == NULL || RegCMDInfo.CMDKey.wMainKey == NULL)
    	return FALSE;

	int	nCmdCount;

    for (nCmdCount = 0; nCmdCount < MAXCMDS; nCmdCount ++)
    {
    	if (lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey == RegCMDInfo.CMDKey || \
        	lpHookInfo->CMDBuffer.CMDInfo [nCmdCount].CMDKey.wMainKey == NULL)
        {
        	lpHookInfo->CMDBuffer.CMDInfo [nCmdCount] = RegCMDInfo;
        	return TRUE;
        }
    }
    return FALSE;
}

//---------------------------------------------------------------------------

