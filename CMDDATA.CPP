//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CMDDATA.H"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
bool __fastcall StringToStream (TStream *lpStream, AnsiString &cString)
{
	int		nBufSize;
    wchar_t	*lpBuffer;

    nBufSize = cString.WideCharBufSize ();
    if (nBufSize > 0)
    {
    	lpBuffer = new (wchar_t [nBufSize]);
        cString.WideChar (lpBuffer, nBufSize);
        lpStream->WriteBuffer (&nBufSize, sizeof(nBufSize));
        lpStream->WriteBuffer (lpBuffer, nBufSize * sizeof(wchar_t));
        delete (lpBuffer);
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall StringFromStream (TStream *lpStream, AnsiString &cString)
{
	try {
		int		nBufSize;
    	wchar_t	*lpBuffer;

        lpStream->ReadBuffer (&nBufSize, sizeof(nBufSize));
	    if (nBufSize > 0)
    	{
    		lpBuffer = new (wchar_t [nBufSize]);
            if (lpBuffer != NULL)
            {
		        lpStream->ReadBuffer (lpBuffer, nBufSize * sizeof(wchar_t));
                cString = lpBuffer;
		        delete (lpBuffer);
            }
        }
    }
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall CMDItem::SaveToRegistry (TRegistry 	*lpRegistry)
{
	int			nItemCount;
    CMDChild	*lpCMDChild;
    AnsiString	strItemRegKey;

	try {
        strItemRegKey = "\\" + lpRegistry->CurrentPath;
	    if (!lpRegistry->OpenKey ((nID + 1), true))
    		return false;

		lpRegistry->WriteString ("Name", strName);
		lpRegistry->WriteBool ("Enable", fEnable);
		lpRegistry->WriteInteger ("VirtualKey", (int)uVirtualKey);
    	lpRegistry->WriteBool ("DblClick", fDblClick);
	    lpRegistry->WriteBool ("EnableFirst", fEnableFirst);
    	lpRegistry->WriteInteger ("Modifier", (int)uMod);
	    lpRegistry->WriteString ("Command", strCommand);
    	lpRegistry->WriteString ("Startup", strDirectory);
	    lpRegistry->WriteInteger ("CmdShow", (int)uCmdShow);
    	lpRegistry->WriteInteger ("Function", (int)uFunction);
        lpRegistry->WriteTime ("AutoRun", tTime);

    	//Clear old children
		nItemCount = 1;
    	while (true)
	    {
	        if (!lpRegistry->DeleteKey (nItemCount))
        		break;
    	    nItemCount ++;
	    }

        //Write children
	    lpCMDChild = lpChildItem;
	    while (lpCMDChild != NULL)
	    {
            lpCMDChild->SaveToRegistry (lpRegistry);
	        lpCMDChild = lpCMDChild->lpNextChild;
        }

        //Restore the registry key
	    lpRegistry->OpenKey (strItemRegKey, true);
	}
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall CMDItem::SaveToStream (TStream *lpStream)
{
	try {
		lpStream->WriteBuffer (&nID, sizeof(nID));
		lpStream->WriteBuffer (&fEnable, sizeof(fEnable));

	    StringToStream (lpStream, strName);

		lpStream->WriteBuffer (&uVirtualKey, sizeof(uVirtualKey));
		lpStream->WriteBuffer (&fDblClick, sizeof(fDblClick));
		lpStream->WriteBuffer (&fEnableFirst, sizeof(fEnableFirst));
		lpStream->WriteBuffer (&uMod, sizeof(uMod));
		lpStream->WriteBuffer (&tTime, sizeof(tTime));

	    StringToStream (lpStream, strCommand);
    	StringToStream (lpStream, strDirectory);

		lpStream->WriteBuffer (&uCmdShow, sizeof(uCmdShow));
		lpStream->WriteBuffer (&uFunction, sizeof(uFunction));
	    //Children
		lpStream->WriteBuffer (&nChildCount, sizeof(nChildCount));
		if (nChildCount > 0)
    	{
        	CMDChild	*lpCMDChild;

	        lpCMDChild = lpChildItem;
    	    while (lpCMDChild != NULL)
        	{
        		lpCMDChild->SaveToStream (lpStream);
	            lpCMDChild = lpCMDChild->lpNextChild;
    	    }
	    }
	}
    catch (...)
    {
    	return false;
    }
    return true;
}

//---------------------------------------------------------------------------
bool __fastcall CMDItem::LoadFromStream (TStream *lpStream)
{
	try {
		lpStream->ReadBuffer (&nID, sizeof(nID)) ;
		lpStream->ReadBuffer (&fEnable, sizeof(fEnable));

    	StringFromStream (lpStream, strName);

		lpStream->ReadBuffer (&uVirtualKey, sizeof(uVirtualKey));
		lpStream->ReadBuffer (&fDblClick, sizeof(fDblClick));
		lpStream->ReadBuffer (&fEnableFirst, sizeof(fEnableFirst));
		lpStream->ReadBuffer (&uMod, sizeof(uMod));
		lpStream->ReadBuffer (&tTime, sizeof(tTime));

    	StringFromStream (lpStream, strCommand);
	    StringFromStream (lpStream, strDirectory);

		lpStream->ReadBuffer (&uCmdShow, sizeof(uCmdShow));
		lpStream->ReadBuffer (&uFunction, sizeof(uFunction));
		lpStream->ReadBuffer (&nChildCount, sizeof(nChildCount));

        lpChildItem = NULL;
        fChildModified = false;
        if (nChildCount >0)
        {
        	CMDChild	*lpCMDChild, *lpLastChild;
            int			nCount;

            lpLastChild = NULL;
            for (nCount = 0; nCount < nChildCount; nCount++)
            {
            	lpCMDChild = new(CMDChild);
                if (lpCMDChild == NULL)
                	break;

            	if (!lpCMDChild->LoadFromStream (lpStream))
                {
                	delete (lpCMDChild);
                	break;
                }

                lpCMDChild->lpNextChild = NULL;
                if (lpLastChild == NULL)
                	lpChildItem = lpCMDChild;
                else
                	lpLastChild->lpNextChild = lpCMDChild;
                lpLastChild = lpCMDChild;
            }
        }
	}
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall CMDChild::SaveToRegistry (TRegistry *lpRegistry)
{
    AnsiString	strItemRegKey;

	try {
        strItemRegKey = "\\" + lpRegistry->CurrentPath;
	    if (!lpRegistry->OpenKey ((nID + 1), true))
    		return false;

		lpRegistry->WriteString ("Command", strCommand);
    	lpRegistry->WriteString ("Startup", strDirectory);
	    lpRegistry->WriteInteger ("CmdShow", (int)uCmdShow);
    	lpRegistry->WriteInteger ("Function", (int)uFunction);

		//Restore registry key
	    lpRegistry->OpenKey (strItemRegKey, true);
	}
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall CMDChild::SaveToStream (TStream *lpStream)
{
	try {
		lpStream->WriteBuffer (&nID, sizeof(nID));

    	StringToStream (lpStream, strCommand);
	    StringToStream (lpStream, strDirectory);

		lpStream->WriteBuffer (&uCmdShow, sizeof(uCmdShow));
		lpStream->WriteBuffer (&uFunction, sizeof(uFunction));
	}
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall CMDChild::LoadFromStream (TStream *lpStream)
{
	try	{
		lpStream->ReadBuffer (&nID, sizeof(nID));

	    if (!StringFromStream (lpStream, strCommand))
        	return false;
    	if (!StringFromStream (lpStream, strDirectory))
        	return false;

		lpStream->ReadBuffer (&uCmdShow, sizeof(uCmdShow));
		lpStream->ReadBuffer (&uFunction, sizeof(uFunction));
    }
    catch (...)
    {
    	return false;
    }
    return true;
}
//---------------------------------------------------------------------------

