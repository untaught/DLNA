
// DLNA_P.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDLNA_PApp:
// See DLNA_P.cpp for the implementation of this class
//

class CDLNA_PApp : public CWinApp
{
public:
	CDLNA_PApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDLNA_PApp theApp;