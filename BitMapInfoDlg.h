// BitMapInfoDlg.h : header file
//

#if !defined(AFX_BITMAPINFODLG_H__9CE60F96_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
#define AFX_BITMAPINFODLG_H__9CE60F96_1F84_11D7_A083_00095B1A7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBitMapInfoDlg dialog

class CBitMapInfoDlg : public CDialog
{
// Construction
public:
	CBitMapInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBitMapInfoDlg)
	enum { IDD = IDD_BITMAPINFO_DIALOG };
	CStatic	c_PosInfo;
	CStatic	c_Limit;
	CButton	c_Read;
	CBitmapDisplay	c_Data;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitMapInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HACCEL accelerators;
	
	void ShowPixels24(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr);
	void ShowPixels8(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr);
	void ShowPixels4(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr);
	void ShowPixels1(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr);
	CString GetCompressionType(DWORD compression);
	void ShowCIEXYZTRIPLE(CIEXYZTRIPLE & triple);
	void ShowCIEXYZ(CIEXYZ & xyz);
	BOOL DoBrowse(LPCTSTR filename);
	void Skip(CFile & f, BITMAPFILEHEADER & info);

	CShow * c_Viewer;
	
	int PadToDWORD(CFile & f, CArray<BYTE, BYTE> & data, int bytesRead);
	afx_msg LRESULT OnUpdateBB(WPARAM, LPARAM);
	
	void DoRead(const CString & filename);
	CFileStatus timestamp;
	CString path;

	static int inchesToPixels(int distance, CDC & dc);
	static CRect inchesToPixels(const CRect & r, CDC & dc);

	int DoPageHeading(CDC & dc, CRect area, int & pageno);
	
	// Generated message map functions
	//{{AFX_MSG(CBitMapInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileOpen();
	afx_msg void OnFilePrint();
	afx_msg void OnHelpAbout();
	afx_msg void OnViewBitmap();
	afx_msg void OnFileExit();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPINFODLG_H__9CE60F96_1F84_11D7_A083_00095B1A7BE1__INCLUDED_)
