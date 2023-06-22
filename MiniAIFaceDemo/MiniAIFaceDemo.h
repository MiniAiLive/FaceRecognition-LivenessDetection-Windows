
// MiniAIFaceDemo.h : PROJECT_NAME Application's main header file
//

#pragma once

#ifndef __AFXWIN_H__
	#error "Include 'stdafx.h' before including this file to generate a PCH file"
#endif

#include "resource.h"		// primary symbol


// CMiniAIFaceDemoApp: 
// For an implementation of this class, see MiniAIFaceDemo.cpp
//

class CMiniAIFaceDemoApp : public CWinApp
{
public:
	CMiniAIFaceDemoApp();

// rewrite
public:
	virtual BOOL InitInstance();

// accomplish
	ULONG_PTR m_gdiplusToken; //GDI Declare member variables

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CMiniAIFaceDemoApp theApp;