#if !defined(AFX_BORDER_H__9CE60FA2_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_BORDER_H__9CE60FA2_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Border.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBorder window

class CBorder : public CWnd
{
// Construction
public:
	CBorder();

// Attributes
public:

// Operations
public:
        BOOL Create(CWnd * parent, UINT id = IDC_STATIC);
	void ShowBB(const CRect & r);
	void ShowBB(BOOL show);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBorder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBorder();

	// Generated message map functions
protected:
        BOOL showing;
	//{{AFX_MSG(CBorder)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BORDER_H__9CE60FA2_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
