// BitMapInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BitmapDisplay.h"
#include "Border.h"
#include "Picture.h"
#include "Show.h"
#include "BitMapInfoDlg.h"
#include "pal.h"
#include "dib.h"

#include "msg.h"

// These are required in case someone is compiling with an obsolete
// platform SDK
#ifndef BI_JPEG
#define BI_JPEG 4L
#endif
#ifndef BI_PNG
#define BI_PNG 5L
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString indent(_T("          "));

DECLARE_MESSAGE(UWM_SHOW_FILE)
DECLARE_MESSAGE(UWM_UPDATE_BB)

#define IDT_UPDATE 1

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitMapInfoDlg dialog

CBitMapInfoDlg::CBitMapInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBitMapInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBitMapInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	c_Viewer = NULL;
}

void CBitMapInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBitMapInfoDlg)
	DDX_Control(pDX, IDC_POS, c_PosInfo);
	DDX_Control(pDX, IDC_LIMIT, c_Limit);
	DDX_Control(pDX, IDC_DATA, c_Data);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBitMapInfoDlg, CDialog)
        ON_REGISTERED_MESSAGE(UWM_UPDATE_BB, OnUpdateBB)
	//{{AFX_MSG_MAP(CBitMapInfoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
	ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDM_FILE_PRINT, OnFilePrint)
	ON_COMMAND(IDM_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(IDM_VIEW_BITMAP, OnViewBitmap)
	ON_COMMAND(IDM_FILE_EXIT, OnFileExit)
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitMapInfoDlg message handlers

/****************************************************************************
*                        CBitMapInfoDlg::OnInitDialog
* Result: void
*       
* Effect: 
*       Initialization
****************************************************************************/

BOOL CBitMapInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	c_Viewer = new CShow;
	c_Viewer->Create(CShow::IDD, this);
	c_Viewer->ShowWindow(SW_HIDE);

	accelerators = ::LoadAccelerators(AfxGetResourceHandle(),
					  MAKEINTRESOURCE(IDR_ACCELERATOR));

	SetTimer(IDT_UPDATE, 100, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/****************************************************************************
*                        CBitMapInfoDlg::OnSysCommand
* Inputs:
*       UINT nID: ID
*	LPARAM lParam:
* Result: void
*       
* Effect: 
*       Handles the dialog about-box
****************************************************************************/

void CBitMapInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBitMapInfoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBitMapInfoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/****************************************************************************
*                          CBitMapInfoDlg::OnCancel
* Result: void
*       
* Effect: 
*       Does nothing; keeps ESC from closing the app
****************************************************************************/

void CBitMapInfoDlg::OnCancel() 
{
}

/****************************************************************************
*                            CBitMapInfoDlg::OnOK
* Result: void
*       
* Effect: 
*       Nothing. Keeps <enter> from closing the dialog
****************************************************************************/

void CBitMapInfoDlg::OnOK() 
{
}

/****************************************************************************
*                           CBitMapInfoDlg::OnClose
* Result: void
*       
* Effect: 
*       Closes the dialog
****************************************************************************/

void CBitMapInfoDlg::OnClose() 
{
	CDialog::OnOK();
}

/****************************************************************************
*                                 ErrorString
* Inputs:
*       DWORD err: Error code
* Result: CString
*       String message
****************************************************************************/

CString ErrorString(DWORD err)
   {
    CString Error;
    LPTSTR s;
    if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		       FORMAT_MESSAGE_FROM_SYSTEM,
		       NULL,
		       err,
		       0,
		       (LPTSTR)&s,
		       0,
		       NULL) == 0)
       { /* failed */
	 // Unknown error code %08x (%d)
	CString fmt;
	CString t;
	fmt.LoadString(IDS_UNKNOWN_ERROR);
	t.Format(fmt, err, LOWORD(err));
	Error = t;
       } /* failed */
    else
       { /* success */
	LPTSTR p = _tcsrchr(s, _T('\r'));
	if(p != NULL)
	   { /* lose CRLF */
	    *p = _T('\0');
	   } /* lose CRLF */
	Error = s;
	::LocalFree(s);
       } /* success */
    return Error;
   } // ErrorString

/****************************************************************************
*                         CBitMapInfoDlg::ShowPixels8
* Inputs:
*       CFile & f: File to read from
*	BITMAPINFOHEADER & info: Header info
*	BITMAPFILEHEADER & hdr: File hader
* Result: void
*       
* Effect: 
*       Reads 8-bit pixel data
****************************************************************************/

void CBitMapInfoDlg::ShowPixels8(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr)
    {
     LPRGBQUAD palette = NULL;
     DWORD offset;
     CString s;

     int colors = GetPaletteSize(info);
     
     if(colors > 0)
	{ /* has palette */
	 palette = new RGBQUAD[colors];
	 offset = f.GetPosition();
	 s.Format(_T("%06d PALETTE"), offset);
	 c_Data.AddString(s);

	 f.Read(palette, colors * sizeof(RGBQUAD));
	 for(int i = 0; i < colors; i++)
	    { /* show palette */
	     c_Data.AddString(new BitmapPalette(i, palette[i]));
	    } /* show palette */
	} /* has palette */

     Skip(f, hdr);
     
     offset = f.GetPosition();
     s.Format(_T("%06d BITMAP DATA (8-bit)"), offset);
     c_Data.AddString(s);
     
     if(info.biCompression != BI_RGB)
	{ /* unsupported */
	 s.Format(_T("Only BI_RGB (uncompressed) files supported"));
	 c_Data.AddString(s);
	 return;
	} /* unsupported */

     CRefPalette * refpal = new CRefPalette(palette);
     refpal->AddRef();

     for(int y = 0; y < abs(info.biHeight); y++)
	{ /* read one row */
	 CArray<BYTE, BYTE> data;
	 data.SetSize(info.biWidth);
	 int bytesRead = 0;
	 int line = info.biHeight > 0 ? info.biHeight - y : y; // handle top-to-bottom case
	 for(int x = 0; x < info.biWidth; x++)
	    { /* read one tuple */
	     f.Read(data.GetData() + x, 1);
	     bytesRead++;
	    } /* read one tuple */
	 int padding = PadToDWORD(f, data, bytesRead);
	 c_Data.AddString(new Bitmap8(data.GetData(), data.GetSize(), info.biWidth, line, padding, refpal));
	} /* read one row */

     refpal->Release();
    } // CBitMapInfoDlg::ShowPixels8

/****************************************************************************
*                         CBitMapInfoDlg::PadToDWORD
* Inputs:
*       CFile & f: File to read
*	CArray<BYTE, BYTE> & data: Current data array
*	int bytesRead: Number of bytes read thus far
* Result: int
*       Number of additional bytes read
* Effect: 
*       Reads data from the file to pad out to a DWORD boundary
****************************************************************************/

int CBitMapInfoDlg::PadToDWORD(CFile & f, CArray<BYTE, BYTE> & data, int bytesRead)
    {
     int delta = (4 - bytesRead % 4) % 4;
     if(delta != 0)
	{ /* read last few */
	 int spare = data.GetSize();
	 data.SetSize(data.GetSize() + delta);
	 for(int n = 0; n < delta; n++)
	    { /* read last */
	     f.Read(data.GetData() + spare + n, 1);
	    } /* read last */
	} /* read last few */
     return delta;
    } // CBitMapInfoDlg::PadToDWORD

/****************************************************************************
*                         CBitMapInfoDlg::ShowPixels1
* Inputs:
*       CFile & f: File to read
*	BITMAPINFOHEADER & info: Header
*	BITMAPFILEHEADER & hdr: File header
* Result: void
*       
* Effect: 
*       Shows a 1-bit bitmap
****************************************************************************/

void CBitMapInfoDlg::ShowPixels1(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr)
    {
     LPRGBQUAD palette = NULL;
     DWORD offset;
     CString s;

     int colors = GetPaletteSize(info);

     if(colors > 0)
	{ /* has palette */
	 palette = new RGBQUAD[colors];
	 offset = f.GetPosition();
	 s.Format(_T("%06d PALETTE"), offset);
	 c_Data.AddString(s);

	 f.Read(palette, colors * sizeof(RGBQUAD));
	 for(int i = 0; i < colors; i++)
	    { /* show palette */
	     c_Data.AddString(new BitmapPalette(i, palette[i]));
	    } /* show palette */
	} /* has palette */
     else
	{ /* default system palette */
	 palette = new RGBQUAD[2];
	 if(palette != NULL)
	    { /* fill it in */
	     // Default is [0] is white, [1] is black
	     palette[0].rgbRed = 255;
	     palette[0].rgbGreen = 255;
	     palette[0].rgbBlue = 255;
	     palette[0].rgbReserved = 0;
	     palette[1].rgbRed = 0;
	     palette[1].rgbGreen = 0;
	     palette[1].rgbBlue = 0;
	     palette[1].rgbReserved = 0;
	    } /* fill it in */
	} /* default system palette */

     Skip(f, hdr);

     offset = f.GetPosition();
     s.Format(_T("%06d BITMAP DATA (1-bit)"), offset);
     c_Data.AddString(s);

     if(info.biCompression != BI_RGB)
	{ /* unsupported */
	 s.Format(_T("Only BI_RGB (uncompressed) files supported"));
	 c_Data.AddString(s);
	 return;
	} /* unsupported */

     CRefPalette * refpal = new CRefPalette(palette);
     refpal->AddRef();

     for(int y = 0; y < abs(info.biHeight); y++)
	{ /* read one row */
	 CArray<BYTE, BYTE> data;
	 data.SetSize((info.biWidth + 8) / 8);
	 int bytesRead = 0;
	 int line = info.biHeight > 0 ? info.biHeight - y : y; // handle top-to-bottom case
	 
	 for(int x = 0; x < info.biWidth; x += 8)
	    { /* read one tuple */
	     f.Read(data.GetData() + x / 8, 1);
	     bytesRead++;
	    } /* read one tuple */
	 int padding = PadToDWORD(f, data, bytesRead);
	 c_Data.AddString(new Bitmap1(data.GetData(), data.GetSize(), info.biWidth, line, padding, refpal));
	} /* read one row */

     refpal->Release();
    } // CBitMapInfoDlg::ShowPixels1

/****************************************************************************
*                         CBitMapInfoDlg::ShowPixels4
* Inputs:
*       CFile & f: File to read
*	BITMAPINFOHEADER & info: Header
*	BITMAPFILEHEADER & hdr: File header
* Result: void
*       
* Effect: 
*       Shows a 4-bit bitmap
****************************************************************************/

void CBitMapInfoDlg::ShowPixels4(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr)
   {
    LPRGBQUAD palette = NULL;
    DWORD offset;
    CString s;

    int colors = GetPaletteSize(info);

    if(colors > 0)
       { /* has palette */
	palette = new RGBQUAD[colors];
	offset = f.GetPosition();
	s.Format(_T("%06d PALETTE"), offset);
	c_Data.AddString(s);

	f.Read(palette, colors * sizeof(RGBQUAD));
	for(int i = 0; i < colors; i++)
	   { /* show palette */
	    c_Data.AddString(new BitmapPalette(i, palette[i]));
	   } /* show palette */
       } /* has palette */
    else
       { /* default system palette */
	palette = new RGBQUAD[2];
	if(palette != NULL)
	   { /* fill it in */
	    // Default is [0] is white, [1] is black
	    palette[0].rgbRed = 255;
	    palette[0].rgbGreen = 255;
	    palette[0].rgbBlue = 255;
	    palette[0].rgbReserved = 0;
	    palette[1].rgbRed = 0;
	    palette[1].rgbGreen = 0;
	    palette[1].rgbBlue = 0;
	    palette[1].rgbReserved = 0;
	   } /* fill it in */
       } /* default system palette */
    
    Skip(f, hdr);

    offset = f.GetPosition();
    s.Format(_T("%06d BITMAP DATA (4-bit)"), offset);
    c_Data.AddString(s);

    if(info.biCompression != BI_RGB)
       { /* unsupported */
	s.Format(_T("Only BI_RGB (uncompressed) files supported"));
	c_Data.AddString(s);
	return;
       } /* unsupported */

    CRefPalette * refpal = new CRefPalette(palette);
    refpal->AddRef();

    for(int y = 0; y < abs(info.biHeight); y++)
       { /* read one row */
	CArray<BYTE, BYTE> data;
	data.SetSize((info.biWidth + 2) / 2);
	int bytesRead = 0;
	int line = info.biHeight > 0 ? info.biHeight - y : y; // handle top-to-bottom case

	for(int x = 0; x < info.biWidth; x += 2)
	   { /* read one tuple */
	    f.Read(data.GetData() + x / 2, 1);
	    bytesRead++;
	   } /* read one tuple */
	int padding = PadToDWORD(f, data, bytesRead);
	c_Data.AddString(new Bitmap4(data.GetData(), data.GetSize(), info.biWidth, line, padding, refpal));
       } /* read one row */

    refpal->Release();
   } // CBitMapInfoDlg::ShowPixels4

/****************************************************************************
*                         CBitMapInfoDlg::ShowPixels24
* Inputs:
*       CFile & f: File to read from
*	BITMAPINFOHEADER & info: Header
*	BITMAPFILEHADER & hdr: File header
* Result: void
*       
* Effect: 
*       Reads and displays the pixels
****************************************************************************/

void CBitMapInfoDlg::ShowPixels24(CFile & f, BITMAPINFOHEADER & info, BITMAPFILEHEADER & hdr)
    {
     Skip(f, hdr);

     CString s;
     DWORD offset = f.GetPosition();
     s.Format(_T("%06d BITMAP DATA (24-bit)"), offset);
     c_Data.AddString(s);

     if(info.biCompression != BI_RGB)
	{ /* unsupported */
	 s.Format(_T("Only BI_RGB (uncompressed) files supported"));
	 c_Data.AddString(s);
	 return;
	} /* unsupported */
     
     for(int y = 0; y < abs(info.biHeight); y++)
	{ /* read one row */
	 CArray<BYTE, BYTE> data;
	 int bytesRead = 0;
	 int line = info.biHeight > 0 ? info.biHeight - y : y; // handle top-to-bottom case!

	 data.SetSize(3 * info.biWidth);

	 for(int x = 0; x < info.biWidth; x++)
	    { /* read one tuple */
	     f.Read(data.GetData() + 3 * x, 1);
	     f.Read(data.GetData() + 3 * x + 1, 1);
	     f.Read(data.GetData() + 3 * x + 2, 1);
	     bytesRead += 3;
	    } /* read one tuple */
	 int padding = PadToDWORD(f, data, bytesRead);
	 c_Data.AddString(new BitmapRGB24(data.GetData(), data.GetSize(), info.biWidth, line, padding));
	} /* read one row */
    } // CBitMapInfoDlg::ShowPixels24

/****************************************************************************
*		       CBitMapInfoDlg::GetCompressionType
* Inputs:
*       DWORD compression: Compression type
* Result: CString
*       String explaining the type
****************************************************************************/

CString CBitMapInfoDlg::GetCompressionType(DWORD compression)
    {
     switch(compression)
	{ /* compression */
	 case BI_RGB:
	    return CString(_T("BI_RGB"));
	    break;
	 case BI_RLE8:
	    return CString(_T("BI_RLE8"));
	    break;
	 case BI_RLE4:
	    return CString(_T("BI_RLE4"));
	    break;
	 case BI_BITFIELDS:
	    return CString(_T("BI_BITFIELDS"));
	    break;
	 case BI_JPEG:
	    return CString(_T("BI_JPEG"));
	    break;
	 case BI_PNG:
	    return CString(_T("BI_PNG"));
	    break;
	 default:
	    return CString(_T("unknown"));
	    break;
	} /* compression */
    } // CBitMapInfoDlg::GetCompressionType

/****************************************************************************
*                         CBitMapInfoDlg::ShowCIEXYZ
* Inputs:
*       CIEXYZ & xyz: xyz tuple
* Result: void
*       
* Effect: 
*       Shows the XYZ triple
****************************************************************************/

void CBitMapInfoDlg::ShowCIEXYZ(CIEXYZ & xyz)
    {
     CString s;
     s.Format(_T("%s%sFXPT2DOT30 ciexyzX = %d"), xyz.ciexyzX);
     c_Data.AddString(s);

     s.Format(_T("%s%sFXPT2DOT30 ciexyzY = %d"), xyz.ciexyzY);
     c_Data.AddString(s);
     
     s.Format(_T("%s%sFXPT2DOT30 ciexyzZ = %d"), xyz.ciexyzZ);
     c_Data.AddString(s);
    } // CBitMapInfoDlg::ShowCIEXYZ

/****************************************************************************
*                      CBitMapInfoDlg::ShowCIEXYZTRIPLE
* Inputs:
*       CIEXYZTRIPLE & triple: 
* Result: void
*       
* Effect: 
*       Shows the CIEXYZTRIPLE
****************************************************************************/

void CBitMapInfoDlg::ShowCIEXYZTRIPLE(CIEXYZTRIPLE & triple)
    {
     CString s;
     s.Format(_T("%s    ciexyzRed"));
     c_Data.AddString(s);
     ShowCIEXYZ(triple.ciexyzRed);

     s.Format(_T("%s    ciexyzGreen"));
     c_Data.AddString(s);
     ShowCIEXYZ(triple.ciexyzGreen);

     s.Format(_T("%s    ciexyzBlue"));
     c_Data.AddString(s);
     ShowCIEXYZ(triple.ciexyzBlue);
    } // CBitMapInfoDlg::ShowCIEXYZTRIPLE

/****************************************************************************
*                            CBitMapInfoDlg::Skip
* Inputs:
*       CFile & f: File to read from
*	BITMAPFILEHEADER & info: Header to use
* Result: void
*       
* Effect: 
*       Skips and logs any bits that are filler
****************************************************************************/

void CBitMapInfoDlg::Skip(CFile & f, BITMAPFILEHEADER & hdr)
    {
     // Now position to the current file position
     DWORD offset = f.GetPosition();
     CString s;

     if(hdr.bfOffBits != 0 && offset != hdr.bfOffBits)
	{ /* skip data */
	 s.Format(_T("Skipping %d bytes (from %d to %d) to start of bitmap"), 
		  hdr.bfOffBits - offset,
		  offset, hdr.bfOffBits - 1);
	 c_Data.AddString(s);
	 CString data;
	 DWORD start = offset;
	 start &= ~0xF; // zero out to 16-byte boundary
	 for(DWORD i = start; i < offset; i++)
	    { /* pad left */
	     data += _T(".. ");
	    } /* pad left */
	 for(i = offset; i < hdr.bfOffBits; i++)
	    { /* read bytes */
	     if( i > offset && (i & 0x0F) == 0)
		{ /* break */
		 s.Format(_T("%06d |%s|"), start, data);
		 c_Data.AddString(new BitmapFixedText(s));
		 start += 16;
		 data = _T("");
		} /* break */
	     BYTE b;
	     f.Read(&b, 1);
	     CString t;
	     t.Format(_T("%02x "), b);
	     data += t;
	    } /* read bytes */
	 if(!data.IsEmpty())
	    { /* show last */
	     while( (i & 0xF) != 0)
		{ /* pad right */
		 data += _T(".. ");
		 i++;
		} /* pad right */

	     s.Format(_T("%06d |%s|"), start, data);
	     c_Data.AddString(new BitmapFixedText(s));
	    } /* show last */
	} /* skip data */
    } // CBitMapInfoDlg::Skip

/****************************************************************************
*                           CBitMapInfoDlg::DoRead
* Result: void
*       
* Effect: 
*       Reads the bitmap file
****************************************************************************/

void CBitMapInfoDlg::DoRead(const CString & filename) 
   {
    c_Data.ResetContent();
    CFile f;

    if(!f.Open(filename, CFile::modeRead | CFile::typeBinary))
       { /* open failed */
	DWORD err = ::GetLastError();
	CString msg;
	msg.Format(_T("%s\n%s"), ErrorString(err), filename);
	AfxMessageBox(msg);
	c_Data.AddString(ErrorString(err));
	c_Data.AddString(filename);
	return;
       } /* open failed */
    
    f.GetStatus(timestamp);  // save for later printout

    DWORD offset;
    offset = f.GetPosition();

    BITMAPFILEHEADER hdr;
    f.Read(&hdr, sizeof(hdr));
    CString s;

    s.Format(_T("%06d BITMAPFILEHEADER"), offset);
    c_Data.AddString(s);
    
    s.Format(_T("%sWORD bfType = 0x%04x (\"%2.2s\") // Signature"), indent, hdr.bfType, (LPCSTR)&hdr.bfType);
    c_Data.AddString(s);

    s.Format(_T("%sDWORD bfSize = 0x%08x (%d) // File size"), indent, hdr.bfSize, hdr.bfSize);
    c_Data.AddString(s);

    if(hdr.bfSize != (DWORD)timestamp.m_size)
       { /* sizes wrong */
	s.Format(_T("%s******ERROR! Actual file size is %d!"), indent, timestamp.m_size);
       } /* sizes wrong */

    s.Format(_T("%sWORD bfReserved1 = 0x%04x"), indent, hdr.bfReserved1);
    c_Data.AddString(s);

    s.Format(_T("%sWORD bfReserved2 = 0x%04x"), indent, hdr.bfReserved2);
    c_Data.AddString(s);

    s.Format(_T("%sDWORD bfOffBits = 0x%08x (%d) // offset to bitmap data"), indent, hdr.bfOffBits, hdr.bfOffBits);
    c_Data.AddString(s);
    
    BITMAPINFOHEADER info;
    BITMAPV4HEADER info4;
    BITMAPV5HEADER info5;
    offset = f.GetPosition();
    
    f.Read(&info, sizeof(info));

    switch(info.biSize)
       { /* info.biSize */
	case sizeof(BITMAPINFOHEADER):
	   //****************************************************************
	   // Normal Bitmap header
	   //****************************************************************
	   s.Format(_T("%06d BITMAPINFOHEADER"), offset);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD biSize = %d // size of this header"), indent, info.biSize);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG biWidth = %d // image width in pixels"), indent, info.biWidth);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG biHeight = %d // image height in pixels"), indent, info.biHeight);
	   c_Data.AddString(s);

	   s.Format(_T("%sWORD biPlanes = %d // number of planes"), indent, info.biPlanes);
	   c_Data.AddString(s);

	   s.Format(_T("%sWORD biBitCount = %d // bits per pixel"), indent, info.biBitCount);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD biCompression = %d (%s) // compression method"), indent, info.biCompression, GetCompressionType(info.biCompression));
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD biSizeImage = %d // number of bytes in bitmap"), indent, info.biSizeImage);
	   if(info.biSizeImage == 0)
	      s += _T(" (0 for BI_RGB (size implied))");

	   c_Data.AddString(s);

	   s.Format(_T("%sLONG biXPelsPerMeter = %d // horizontal resolution"), indent, info.biXPelsPerMeter);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG biYPelsPerMeter = %d // vertical resolution"), indent, info.biYPelsPerMeter);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD biClrUsed = %d // colors used in the image"), indent, info.biClrUsed);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD biClrImportant = %d // important colors"), indent, info.biClrImportant);
	   c_Data.AddString(s);
	   break;

	case sizeof(BITMAPV4HEADER):
	   //****************************************************************
	   // V4 HEADER
	   //****************************************************************
	   f.Seek(offset, CFile::begin);
	   f.Read(&info4, sizeof(BITMAPV4HEADER));
	   s.Format(_T("%06d BITMAPV4HEADER"), offset);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4Size = %d // size of this header"), indent, info4.bV4Size);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG bV4Width = %d // image width in pixels"), indent, info4.bV4Width);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG bV4Height = %d // image height in pixels"), indent, info4.bV4Height);
	   c_Data.AddString(s);

	   s.Format(_T("%sWORD bV4Planes = %d // number of planes"), indent, info4.bV4Planes);
	   c_Data.AddString(s);

	   s.Format(_T("%sWORD bV4BitCount = %d // bits per pixel"), indent, info4.bV4BitCount);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4Compression = %d (%s) // compression method"), indent, info4.bV4V4Compression, GetCompressionType(info4.bV4V4Compression));
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4SizeImage = %d // number of bytes in bitmap"), indent, info4.bV4SizeImage);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG bV4XPelsPerMeter = %d // horizontal resolution"), indent, info4.bV4XPelsPerMeter);
	   c_Data.AddString(s);

	   s.Format(_T("%sLONG bV4YPelsPerMeter = %d // vertical resolution"), indent, info4.bV4YPelsPerMeter);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4ClrUsed = %d // colors used in the image"), indent, info4.bV4ClrUsed);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4ClrImportant = %d // important colors"), indent, info4.bV4ClrImportant);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4RedMask = 0x%08x //"), indent, info4.bV4RedMask);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4GreenMask = 0x%08x //"), indent, info4.bV4GreenMask);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4BlueMask = 0x%08x //"), indent, info4.bV4BlueMask);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4AlphaMask = 0x%08x //"), indent, info4.bV4AlphaMask);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4CSType = 0x%08x //"), indent, info4.bV4CSType);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4Endpoints //"), indent);
	   c_Data.AddString(s);

	   ShowCIEXYZTRIPLE(info4.bV4Endpoints);

	   s.Format(_T("%sDWORD bV4GammaRed = 0x%08x //"), indent, info4.bV4GammaRed);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4GammaGreen = 0x%08x //"), indent, info4.bV4GammaGreen);
	   c_Data.AddString(s);

	   s.Format(_T("%sDWORD bV4GammaBlue = 0x%08x //"), indent, info4.bV4GammaBlue);
	   c_Data.AddString(s);
	   
	   break;
	case sizeof(BITMAPV5HEADER):
	   //*****************************************************************************
	   // V5 HEADER
	   //*****************************************************************************
	   f.Seek(offset, CFile::begin);
	   f.Read(&info5, sizeof(BITMAPV5HEADER));
	   s.Format(_T("%06d BITMAPV5HEADER"), offset);
	   break;
       } /* info.biSize */

#define WAIT_THRESHOLD 150000

    if(hdr.bfSize > WAIT_THRESHOLD)
       BeginWaitCursor();
    
    if(info.biHeight < 0)
       { /* top-to-bottom */
	s.Format(_T("******This program currently only supports bottom-to-top bitmaps"));
	c_Data.AddString(s);
	s.Format(_T("******No guarantees that the displays will make sense"));
	c_Data.AddString(s);
       } /* top-to-bottom */

    switch(info.biBitCount)
       { /* count */
	case 1: // monochrome
	   ShowPixels1(f, info, hdr);
	   break;
	case 4: // 4-bit color
	   ShowPixels4(f, info, hdr);
	   break;
	case 8: // 256-color
	   ShowPixels8(f, info, hdr);
	   break;
	case 24:
	   ShowPixels24(f, info, hdr);
	   break;
	case 16: // 16-bit color
	case 32: // 32-bit color
	default:
	   s.Format(_T("Currently %d-bit color depth is not supported"), info.biBitCount);
	   c_Data.AddString(s);
	   break;
       } /* count */

    f.SeekToBegin();
    
    if(c_Viewer != NULL)
       { /* show it */
	c_Viewer->SendMessage(UWM_SHOW_FILE, (WPARAM)&f, (LPARAM)(LPCTSTR)filename);
       } /* show it */

    if(hdr.bfSize > WAIT_THRESHOLD)
       EndWaitCursor();

    f.Close();
   }

/****************************************************************************
*                           CBitMapInfoDlg::OnSize
* Result: void
*       
* Effect: 
*       Resizes the listbox
****************************************************************************/

void CBitMapInfoDlg::OnSize(UINT nType, int cx, int cy) 
   {
    CDialog::OnSize(nType, cx, cy);
	
    if(nType == SIZE_MINIMIZED)
       return; // don't bother if going to icon size

    if(c_Data.GetSafeHwnd() != NULL)
       { /* resize */
	CRect r;
	c_Data.GetWindowRect(&r);
	ScreenToClient(&r);
	c_Data.SetWindowPos(NULL, 0, 0,
			    cx, cy - r.top,
			    SWP_NOMOVE | SWP_NOZORDER);
       } /* resize */
   }

/****************************************************************************
*                       CBitMapInfoDlg::OnGetMinMaxInfo
* Inputs:
*       MINMAXINFO FAR * lpMMI: Pointer to structure
* Result: void
*       
* Effect: 
*       Limits the minimum size of the resized dialog
****************************************************************************/

void CBitMapInfoDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
   {
    CDialog::OnGetMinMaxInfo(lpMMI);    

    if(c_Limit.GetSafeHwnd() != NULL)
       { /* use limit */
	CRect r;
	c_Limit.GetWindowRect(&r);
	ScreenToClient(&r);
	
	lpMMI->ptMinTrackSize.x = r.right + 2 * ::GetSystemMetrics(SM_CXFRAME);
	lpMMI->ptMinTrackSize.y = r.bottom + ::GetSystemMetrics(SM_CYCAPTION)
				           + ::GetSystemMetrics(SM_CYMENU)
				           + 2 * ::GetSystemMetrics(SM_CXFRAME);
       } /* use limit */
   }

/****************************************************************************
*                         CBitMapInfoDlg::OnUpdateBB
* Inputs:
*       WPARAM: ignored
*	LPARAM: ignored
* Result: LRESULT
*       Logically void, 0, always
* Effect: 
*       Updates the bounding box
****************************************************************************/

LRESULT CBitMapInfoDlg::OnUpdateBB(WPARAM, LPARAM)
    {
     if(c_Data.GetCount() == 0)
	return 0;

     CString old;
     c_PosInfo.GetWindowText(old);

     CRect r;
     if(c_Data.GetBoundingRect(r))
	{ /* valid data */
	 CString s;
	 s.Format(_T("%d %d %d %d"), r.left, r.top, r.right, r.bottom);
	 if(s != old)
	    c_PosInfo.SetWindowText(s);
	 c_Viewer->ShowBB(r);
	 c_Viewer->ShowBB(TRUE);
	} /* valid data */
     else
	{ /* no data */
	 if(!old.IsEmpty())
	    { /* clear it */
	     c_PosInfo.SetWindowText(_T(""));
	    } /* clear it */
         c_Viewer->ShowBB(FALSE);
	} /* no data */
     return 0;
    } // CBitMapInfoDlg::OnUpdateBB

/****************************************************************************
*                           CBitMapInfoDlg::OnTimer
* Inputs:
*       UINT nIDEvent
* Result: void
*       
* Effect: 
*       Handles the one-and-only timer event which is to poll the listbox
*	to see what the current selection area should be
****************************************************************************/

void CBitMapInfoDlg::OnTimer(UINT nIDEvent) 
   {
    PostMessage(UWM_UPDATE_BB);
	
    CDialog::OnTimer(nIDEvent);
   }

/****************************************************************************
*                         CBitMapInfoDlg::OnFileOpen
* Result: void
*       
* Effect: 
*       Opens the file
****************************************************************************/

void CBitMapInfoDlg::OnFileOpen() 
   {
    CString filefilter;
    filefilter.LoadString(IDS_BMP_FILTER);
    CFileDialog dlg(TRUE, _T(".bmp"), NULL, OFN_HIDEREADONLY, filefilter);

    switch(dlg.DoModal())
       { /* DoModal */
	case 0:
	case IDCANCEL:
	   return;
       } /* DoModal */
    path = dlg.GetPathName();
    CString caption;

    caption = AfxGetAppName();
    caption += _T(" - ");
    caption += path;
    SetWindowText(caption);
    DoRead(path);
   }

/****************************************************************************
*			 CBitmapInfoDlg::inchesToPixels
* Inputs:
*       int distance: Distance expressed in hundredths of an inch
*	CDC & dc: DC
* Result: int
*       Distance expressed in pixels
****************************************************************************/

int CBitMapInfoDlg::inchesToPixels(int distance, CDC & dc)
   {
    return (dc.GetDeviceCaps(LOGPIXELSX) * distance) / 100;
   } // CBitmapInfoDlg::inchesToPixels

/****************************************************************************
*			 CBitmapInfoDlg::inchesToPixels
* Inputs:
*       const CRect & r: Rectangle to convert
*	CDC & dc: DC
* Result: CRect
*       Converted rectangle
* Effect: 
*       Converts the dimensions of the rectangle from inches to pixels
****************************************************************************/

CRect CBitMapInfoDlg::inchesToPixels(const CRect & r, CDC & dc)
   {
    CRect result;
    result.left = inchesToPixels(r.left, dc);
    result.right = inchesToPixels(r.right, dc);
    result.top = inchesToPixels(r.top, dc);
    result.bottom = inchesToPixels(r.bottom, dc);
    return result;
   } // CBitmapInfoDlg::inchesToPixels

/****************************************************************************
*                        CBitMapInfoDlg::DoPageHeading
* Inputs:
*       CDC & dc: DC to use
*	CRect area: Margin values
*	int & pageno: Page # of last page printed (start at 0
* Result: int
*       Height of the heading
* Effect: 
*       Prints the heading
****************************************************************************/

int CBitMapInfoDlg::DoPageHeading(CDC & dc, CRect area, int & pageno)
    {
     // filename timestamp                Page nn
     pageno++;
     CString leftText;
     leftText = path;
     leftText += _T("     ");
     leftText += timestamp.m_ctime.Format(_T("%A, %d-%b-%Y %H:%M:%S"));
     dc.TextOut(area.left, area.top, leftText);


     CString s;
     s.Format(_T("Page %d"), pageno);
     CSize sz = dc.GetTextExtent(s);

     int x = area.right - sz.cx;
     dc.TextOut(x, area.top, s);
     return sz.cy;
    } // CBitMapInfoDlg::DoPageHeading

/****************************************************************************
*                         CBitMapInfoDlg::OnFilePrint
* Result: void
*       
* Effect: 
*       Prints the bitmap
****************************************************************************/

void CBitMapInfoDlg::OnFilePrint() 
   {
    CPrintDialog dlg(FALSE);
    dlg.m_pd.Flags |= PD_RETURNDC; // get the DC

    switch(dlg.DoModal())
       { /* DoModal */
	case 0:
	case IDCANCEL:
	   return;
	case IDOK:
	   break;
	default:
	   ASSERT(FALSE);
       } /* DoModal */

    CDC dc;
    dc.Attach(dlg.m_pd.hDC);

    // Can this printer print bitmaps?
    DWORD caps = dc.GetDeviceCaps(RASTERCAPS);
    if((caps & RC_BITBLT) == 0)
       { /* cannot print bitmaps */
	AfxMessageBox(IDS_NO_BITMAPS);
	return;
       } /* cannot print bitmaps */

    CString name;
    GetWindowText(name);
    if(dc.StartDoc(name) < 0)
       { /* printer failed */
	DWORD err = ::GetLastError();
	CString msg;
	msg.Format(_T("Unable to start document\n%s"), ErrorString(err));
	AfxMessageBox(msg, MB_ICONERROR | MB_OK);
	return;
       } /* printer failed */

    if(dc.StartPage() < 0)
       { /* page failed */
	// NYI: Error Message
	return;
       } /* page failed */

#define PT_LEFTMARGINX 30 // in hundredths of an inch
#define PT_TOPMARGINY  30 // in hundredths of an inch

    CSize pixels(dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
    CPoint margin(inchesToPixels(PT_TOPMARGINY, dc), inchesToPixels(PT_LEFTMARGINX, dc));
    CRect area(margin.x, margin.y, pixels.cx - margin.x, pixels.cy - margin.y);

    // First, put out all the statistics and the palette
    // information

    int y = area.top;
    
    int lineheight = dc.GetTextExtent(CString(_T("X"))).cy;

    int pageno = 0;
    y += DoPageHeading(dc, area, pageno);

    BOOL anydata = FALSE;
    for(int i =0; i < c_Data.GetCount(); i++)
       { /* read one line */
	BitmapDisplayInfo * data = (BitmapDisplayInfo *)c_Data.GetItemDataPtr(i);
	if(data->GetLine() >= 0)
	   break; // that's all, folks!

	// If we are at the end of the page, start a new page
	if(y + lineheight > area.bottom)
	   { /* next page */
	    dc.EndPage();
	    dc.StartPage();
	    y = area.top;
	    y += DoPageHeading(dc, area, pageno);
	   } /* next page */

	CRect r(area.left, y, area.right, y + lineheight);
	data->Draw(&dc, r);
	anydata = TRUE;
	y += lineheight;
       } /* read one line */

    if(anydata)
       dc.EndPage();

    CBitmap * bmp;
    bmp = c_Viewer->GetBitmap();
    if((HBITMAP)bmp == NULL)
       { /* not found */
	ASSERT(FALSE);
	// NYI: error
	return;
       } /* not found */

    HANDLE dib = MakeDIB(bmp);

    y = area.top;
    y += DoPageHeading(dc, area, pageno);

    if(dib != NULL)
       { /* image it */
	int saved = dc.SaveDC();
	BITMAPINFOHEADER bmi;
	DibInfo(dib, &bmi);
	double scalingx = (double)(area.Width()) / (double)bmi.biWidth;
	double scalingy = (double)(area.Height()) / (double)abs(bmi.biHeight);
	double scaling = min(scalingx, scalingy);

	// Do not overscale. We assume the resolution of a monitor is 90dpi
	// Reset the scaling to not exceed 90 points per inch
	// Note that we could have done this by selecting the mapping mode
	// MM_LOENGLISH but this illustrates how to do the computations
	// without that
	scaling = min(scaling, (double)dc.GetDeviceCaps(LOGPIXELSX) / 90.0f);
	int width = (int)(scaling * (double)bmi.biWidth);
	int height = (int)(scaling * (double)abs(bmi.biHeight));

	PrintDIB(&dc, dib, area.left, y, width, height);
	FreeDIB(dib);
       } /* image it */

    dc.EndPage();
    dc.EndDoc();
   }

/****************************************************************************
*                         CBitMapInfoDlg::OnHelpAbout
* Result: void
*       
* Effect: 
*       Shows the About box
****************************************************************************/

void CBitMapInfoDlg::OnHelpAbout() 
   {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
   }

/****************************************************************************
*                        CBitMapInfoDlg::OnViewBitmap
* Result: void
*       
* Effect: 
*       If the viewer is iconized or invisible, make it visible and normal
****************************************************************************/

void CBitMapInfoDlg::OnViewBitmap() 
   {
    if(c_Viewer == NULL || !::IsWindow(c_Viewer->GetSafeHwnd()))
       { /* creation error */
	ASSERT(FALSE);
       } /* creation error */
    else
       { /* show it */
	if(c_Viewer->IsIconic())
	   { /* maximize */
	    c_Viewer->ShowWindow(SW_RESTORE);
	   } /* maximize */
	if(!c_Viewer->IsWindowVisible())
	   { /* show it */
	    c_Viewer->ShowWindow(SW_SHOW);
	   } /* show it */
       } /* show it */
   }

/****************************************************************************
*                     CBitMapInfoDlg::PreTranslateMessage
* Inputs:
*       MSG * pMsg: Message reference
* Result: BOOL
*       TRUE if message is OK
*	FALSE if message should be ignored
* Effect: 
*       Handles the accelerator keys
* Notes:
*	This is from Paul DiLascia's article on using accelerators in a
*	dialog
****************************************************************************/

BOOL CBitMapInfoDlg::PreTranslateMessage(MSG* pMsg) 
   {
    if (WM_KEYFIRST <= pMsg->message && 
	pMsg->message <= WM_KEYLAST) 
       { 
	if (accelerators != NULL && 
	 ::TranslateAccelerator(m_hWnd, accelerators, pMsg)) 
	   return TRUE; 
       }
    return CDialog::PreTranslateMessage(pMsg);
}

/****************************************************************************
*                         CBitMapInfoDlg::OnFileExit
* Result: void
*       
* Effect: 
*       Terminates the app by calling CDialog::OnOK
****************************************************************************/

void CBitMapInfoDlg::OnFileExit() 
   {
    CDialog::OnOK();
   }

/****************************************************************************
*                       CBitMapInfoDlg::OnInitMenuPopup
* Inputs:
*       CMenu * pPopupMenu: ignored
*	UINT nIndex: ignored
*	BOOL bSysMenu: ignored
* Result: void
*       
* Effect: 
*       Enables/disables menu items (we can't use ON_UPDATE_COMMAND_UI in
*	a dialog)
****************************************************************************/

void CBitMapInfoDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
   {
    CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
    CBitmap * bmp = c_Viewer->GetBitmap();
    GetMenu()->EnableMenuItem(IDM_FILE_PRINT, bmp == NULL ? MF_GRAYED : MF_ENABLED);
   }
