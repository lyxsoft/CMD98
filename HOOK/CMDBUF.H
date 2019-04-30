/*
	CMDBuf.h

    LyxSoft 1998
*/

#include <vcl.h>
#if !defined(__CMDBUF_DEFINED__)

#define	__CMDBUF_DEFINED__

#define MAXCMDCHARS 255

struct CMDKEY {		WPARAM	wMainKey;
					BOOL	fShift;
                    BOOL	fAlt;
                    BOOL	fCtrl;

                    inline operator== (CMDKEY SecondValue)
                    {
                    	return (wMainKey == SecondValue.wMainKey && \
                        		fShift == SecondValue.fShift && \
                                fAlt == SecondValue.fAlt && \
                                fCtrl == SecondValue.fCtrl);
                    }
				};

struct CMDINFO {	CMDKEY	CMDKey;
                    char	strCommand [MAXCMDCHARS + 1];
				};

#define	MAXCMDS	16

struct CMDBUFFER {	int		nBufSize;
					CMDINFO CMDInfo [MAXCMDS];
				};
#endif

/*
	End of File
*/

