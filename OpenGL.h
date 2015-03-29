
// OpenGL.h : main header file for the OpenGL application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// COpenGLApp:
// See OpenGL.cpp for the implementation of this class
//

class COpenGLApp : public CWinApp
{
public:
	COpenGLApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COpenGLApp theApp;
