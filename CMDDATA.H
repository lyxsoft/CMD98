//---------------------------------------------------------------------------
#ifndef CMDDATAH
#define CMDDATAH

#include <registry.hpp>

//---------------------------------------------------------------------------
class	CMDChild {
				public:
					int			nID;
					AnsiString	strCommand;
                    AnsiString	strDirectory;
                    UINT		uCmdShow;
                    int			uFunction;
                    CMDChild	*lpNextChild;
                    inline operator !=(CMDChild &cCmdItem)
                    {
                    	return (nID != cCmdItem.nID ||
                                strCommand != cCmdItem.strCommand ||
                                strDirectory != cCmdItem.strDirectory ||
                                uCmdShow != cCmdItem.uCmdShow ||
                                uFunction != cCmdItem.uFunction);
                    };
                    bool __fastcall SaveToRegistry (TRegistry *);
                    bool __fastcall LoadFromRegistry (TRegistry *);
                    bool __fastcall	SaveToStream (TStream *);
                    bool __fastcall	LoadFromStream (TStream *);
				};

class	CMDItem	{
				public:
					int			nID;
					bool		fEnable;
					AnsiString	strName;
					UINT		uVirtualKey;
                    bool		fDblClick;
                    bool		fEnableFirst;
					UINT		uMod;
                    TTime		tTime;

                    AnsiString	strCommand;
                    AnsiString	strDirectory;
                    UINT		uCmdShow;
                    int			uFunction;

                    bool		fChildModified;
                    int			nChildCount;
                    CMDChild	*lpChildItem;
                    CMDItem		*lpNextItem;

                    inline operator !=(CMDItem &cCmdItem)
                    {
                    	return (nID != cCmdItem.nID ||
                        		fEnable != cCmdItem.fEnable ||
                                strName != cCmdItem.strName ||
                                uVirtualKey != cCmdItem.uVirtualKey ||
                                fDblClick != cCmdItem.fDblClick ||
                                fEnableFirst != cCmdItem.fEnableFirst ||
                                uMod != cCmdItem.uMod ||
                                tTime != cCmdItem.tTime ||
                                strCommand != cCmdItem.strCommand ||
                                strDirectory != cCmdItem.strDirectory ||
                                uCmdShow != cCmdItem.uCmdShow ||
                                uFunction != cCmdItem.uFunction ||
                                lpChildItem != cCmdItem.lpChildItem);
                    };
                    bool __fastcall SaveToRegistry (TRegistry *);
                    bool __fastcall LoadFromRegistry (TRegistry *);
                    bool __fastcall	SaveToStream (TStream *);
                    bool __fastcall	LoadFromStream (TStream *);
				};

#endif
