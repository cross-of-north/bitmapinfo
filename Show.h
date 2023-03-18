#if !defined(AFX_SHOW_H__9CE60FA0_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_SHOW_H__9CE60FA0_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Show.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShow dialog

class CShow : public CDialog
{
// Construction
public:
	CShow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShow)
	enum { IDD = IDD_SHOW };
	CPicture c_Image;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
	public:
	   void ShowBB(const CRect & r);
	   void ShowBB(BOOL show);
	   CBitmap * GetBitmap();
protected:
        //****************************************************************
	// File properties
	//****************************************************************
        BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER info;
	BITMAPINFO * bi;
	
	CBitmap TheBitmap;
	
        afx_msg LRESULT OnShowFile(WPARAM, LPARAM);
	CPalette palette;
	HBITMAP ReadDibInfo(CFile & f);
	void SetScrollers();
	// Generated message map functions
	//{{AFX_MSG(CShow)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOW_H__9CE60FA0_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
