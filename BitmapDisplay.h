#if !defined(AFX_BITMAPDISPLAY_H__9CE60F9F_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_BITMAPDISPLAY_H__9CE60F9F_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapDisplay.h : header file
//
#include "Data.h"

/////////////////////////////////////////////////////////////////////////////
// CBitmapDisplay window

class CBitmapDisplay : public CListBox
{
// Construction
public:
	CBitmapDisplay();

// Attributes
public:

// Operations
public:
    int AddString(BitmapDisplayInfo * data) { return CListBox::AddString((LPCTSTR)data); }
    int AddString(LPCTSTR s) { return AddString(new BitmapText(s)); }
    int InsertString(LPCTSTR s) { ASSERT(FALSE); return LB_ERR; } // NOT SUPPORTED!
    void ResetContent() { CListBox::ResetContent(); SetHorizontalExtent(0); }
    BOOL GetBoundingRect(CRect & r);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapDisplay)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapDisplay();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapDisplay)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPDISPLAY_H__9CE60F9F_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
