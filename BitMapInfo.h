// BitMapInfo.h : main header file for the BITMAPINFO application
//

#if !defined(AFX_BITMAPINFO_H__9CE60F94_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_BITMAPINFO_H__9CE60F94_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBitMapInfoApp:
// See BitMapInfo.cpp for the implementation of this class
//

class CBitMapInfoApp : public CWinApp
{
public:
	CBitMapInfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitMapInfoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBitMapInfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPINFO_H__9CE60F94_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
