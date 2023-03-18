#if !defined(AFX_PICTURE_H__9CE60FA1_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_PICTURE_H__9CE60FA1_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Picture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicture window

class CPicture : public CStatic
{
// Construction
public:
	CPicture();

// Attributes
public:

// Operations
public:
        void ShowBB(const CRect & r);
	void ShowBB(BOOL show);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicture)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicture();
	
	// Generated message map functions
protected:
        CBorder c_Border;
	//{{AFX_MSG(CPicture)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTURE_H__9CE60FA1_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
