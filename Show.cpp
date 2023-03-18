// Show.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Border.h"
#include "picture.h"
#include "Show.h"
#include "msg.h"
#include "pal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DECLARE_MESSAGE(UWM_SHOW_FILE)
/////////////////////////////////////////////////////////////////////////////
// CShow dialog


CShow::CShow(CWnd* pParent /*=NULL*/)
	: CDialog(CShow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bi = NULL;
}


void CShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShow)
	DDX_Control(pDX, IDC_IMAGE, c_Image);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShow, CDialog)
        ON_REGISTERED_MESSAGE(UWM_SHOW_FILE, OnShowFile)
	//{{AFX_MSG_MAP(CShow)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShow message handlers

/****************************************************************************
*                               CShow::OnCancel
* Result: void
*       
* Effect: 
*       The ESC key just hides the window, it doesn't kill it
****************************************************************************/

void CShow::OnCancel() 
   {
    ShowWindow(SW_HIDE);
   }

/****************************************************************************
*                                 CShow::OnOK
* Result: void
*       
* Effect: 
*       Does nothing. Keeps Enter key from closing viewer
****************************************************************************/

void CShow::OnOK() 
   {
   }

/****************************************************************************
*                               CShow::OnClose
* Result: void
*       
* Effect: 
*       Hides the window
****************************************************************************/

void CShow::OnClose() 
   {
    ShowWindow(SW_HIDE);
   }

/****************************************************************************
*                              CShow::OnShowFile
* Inputs:
*       WPARAM: (WPARAM)(CFile *): File, positioned to its start
*	LPARAM: (LPARAM)(LPCTSTR) filename to use
* Result: LRESULT
*       (LRESULT)(BOOL) TRUE if successful
*	FALSE if error
* Effect: 
*       Reads the bitmap file in and displays it
****************************************************************************/

LRESULT CShow::OnShowFile(WPARAM wParam, LPARAM lParam)
    {
     c_Image.SetBitmap(NULL); // remove from display
			      // If the bitmap handle we've saved has a bitmap in it,
			      // free that bitmap. Note that if we have an error
			      // creating the bitmap, below, the current bitmap will
			      // be gone
     if((HBITMAP)TheBitmap != NULL)
	TheBitmap.DeleteObject();

     LPCTSTR filename = (LPCTSTR)lParam;
     SetWindowText(filename);
     CFile & f = *(CFile *)wParam;

     // Read the DIB and create an HBITMAP object
     HBITMAP bm = ReadDibInfo(f);
     if(bm == NULL)
	return FALSE;

     // Now attach the new HBITMAP to the object
     TheBitmap.Attach(bm);

     c_Image.SetBitmap((HBITMAP)TheBitmap);

     SetScrollers();
     return TRUE;

    } // CShow::OnShowFile

/****************************************************************************
*                             CShow::ReadDibInfo
* Inputs:
*       CFile & f: File to read
* Result: HBITMAP
*       Bitmap handle if valid
*	NULL if error
* Effect: 
*       Reads the DIB data
****************************************************************************/

HBITMAP CShow::ReadDibInfo(CFile & f)
    {
     CBitmap bmp;

     f.Read(&hdr, sizeof(hdr));

     f.Read(&info, sizeof(info));

     LOGPALETTE * lp;

     int colors = GetPaletteSize(info);

     lp = (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE) + colors * sizeof(PALETTEENTRY)];
     lp->palNumEntries = (WORD)colors;

#define PALVERSION 0x0300

     lp->palVersion = PALVERSION;

     // Fill in the palette entries
     if(bi != NULL)
	delete bi;

     bi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO) + colors * sizeof(RGBQUAD)];
     if((HPALETTE)palette != NULL)
	palette.DeleteObject();

     if(colors > 0)
	{ /* read quads */
	 f.Read(&bi->bmiColors, sizeof(RGBQUAD) * colors);
	 for(int i = 0; i < colors; i++)
	    { /* convert */
	     lp->palPalEntry[i].peRed = bi->bmiColors[i].rgbRed;
	     lp->palPalEntry[i].peGreen = bi->bmiColors[i].rgbGreen;
	     lp->palPalEntry[i].peBlue = bi->bmiColors[i].rgbBlue;
	     lp->palPalEntry[i].peFlags = 0;
	    } /* convert */
	 VERIFY(palette.CreatePalette(lp));
	} /* read quads */


     DWORD start = f.GetPosition();

     f.SeekToEnd();
     DWORD end = f.GetPosition();

     if(hdr.bfOffBits != 0)
	{ /* seek */
	 f.Seek(hdr.bfOffBits, CFile::begin);
	 start = hdr.bfOffBits;
	} /* seek */

     LPBYTE bits = new BYTE[end - start + 1];
     f.Read(bits, end - start + 1);

     CClientDC dc(NULL);
     if((HPALETTE)palette != NULL)
	{ /* select */
	 dc.SelectObject(&palette);
	} /* select */

     bmp.CreateCompatibleBitmap(&dc, info.biWidth, info.biHeight);
     bi->bmiHeader = info;
     ::SetDIBits((HDC)dc, (HBITMAP)bmp, 0, info.biHeight, bits, bi, DIB_RGB_COLORS);

     delete bits;

     return (HBITMAP)bmp.Detach();
    } // CShow::ReadDibInfo

/****************************************************************************
*                             CShow::SetScrollers
* Result: void
*       
* Effect: 
*       Sets the scroll ranges
* Notes:
*	The funny computations dealing with setting the page size deal with
*	the situation where the image is partially scrolled off, but the
*	window has gotten large enough. It turns out that when the page size
*	equals the range, the scrollbar is hidden (or disabled, had we set
*	it that way) making it impossible to scroll the image back on. The
*	funny computations about page size ensure that as the window grows,
*	the scrollbars always remain active as long as any part of the image
*	is scrolled off the view. The +1 fudge factor is critical because
*	otherwise the scrollbar, after being reset, allows one pixel of
*	scrolling in the off-direction.
****************************************************************************/

void CShow::SetScrollers()
    {
     CRect image;
     c_Image.GetWindowRect(&image);
     ScreenToClient(&image);

     CRect view;
     GetClientRect(&view);

     SCROLLINFO info;
     ::ZeroMemory(&info, sizeof(info));
     info.cbSize = sizeof(info);

     if(image.top == 0 && image.Height() < view.Height())
	{ /* no vert scrolling */
	 info.nMin = 0;
	 info.nMax = 0;
	 info.fMask = SIF_RANGE;
	} /* no vert scrolling */
     else
	{ /* vert scrolling */
	 info.nMin = 0;
	 info.nMax = image.Height(); // - view.Height();
	 if(image.top < 0 && view.Height() >= image.Height())
	    { /* need more */
	     info.nPage = image.Height() + image.top + 1;
	    } /* need more */
	 else
	    { /* just fine */
	     info.nPage = view.Height();
	    } /* just fine */

	 info.fMask = SIF_RANGE | SIF_PAGE;
	} /* vert scrolling */
     SetScrollInfo(SB_VERT, &info);
     
     ::ZeroMemory(&info, sizeof(info));
     info.cbSize = sizeof(info);
     if(image.left == 0 && image.Width() < view.Width())
	{ /* no horz scrolling */
	 info.nMin = 0;
	 info.nMax = 0;
	 info.fMask = SIF_RANGE;
	} /* no horz scrolling */
     else
	{ /* horz scrolling */
	 info.nMin = 0;
	 info.nMax = image.Width(); // - view.Width();
	 if(image.top < 0 && view.Width() >= image.Width())
	    { /* need more */
	     info.nPage = image.Width() + image.top + 1;
	    } /* need more */
	 else
	    { /* just fine */
	     info.nPage = view.Width();
	    } /* just fine */
	 info.fMask = SIF_RANGE | SIF_PAGE;
	} /* horz scrolling */
     SetScrollInfo(SB_HORZ, &info);
    } // CShow::SetScrollers

void CShow::OnSize(UINT nType, int cx, int cy) 
   {
    CDialog::OnSize(nType, cx, cy);
	
    if(c_Image.GetSafeHwnd() != NULL)
       { /* handle scrolling */
	SetScrollers();
       } /* handle scrolling */
   }

void CShow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
   {
    int newpos = GetScrollPos(SB_HORZ);
    SCROLLINFO info;
    ::ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo(SB_HORZ, &info);

    int low = info.nMin;
    int high = info.nMax - info.nPage;

    CRect view;
    GetClientRect(&view);

    CRect image;
    c_Image.GetWindowRect(&image);
    ScreenToClient(&image);
    
    switch(nSBCode)
       { /* nSBCode */
	case SB_LINEUP:
	   newpos -= ::GetSystemMetrics(SM_CXVSCROLL);
	   if(newpos < low)
	      newpos = low;
	   break;
	case SB_LINEDOWN:
	   newpos += ::GetSystemMetrics(SM_CXVSCROLL);
	   if(newpos > high)
	      newpos = high;
	   break;
	case SB_PAGEUP:
	   newpos -= view.Height();
	   if(newpos < low)
	      newpos = low;
	   break;
	case SB_PAGEDOWN:
	   newpos += view.Height();
	   if(newpos > high)
	      newpos = high;
	   break;
	case SB_THUMBTRACK:
	   newpos = info.nTrackPos;
	   break;
	case SB_THUMBPOSITION:
	   newpos = info.nTrackPos;
	   break;
       } /* nSBCode */
    SetScrollPos(SB_HORZ, newpos);
    c_Image.SetWindowPos(NULL,
			 -newpos,
			 image.top,
			 0, 0,
			 SWP_NOSIZE | SWP_NOZORDER);

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
   }

void CShow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
   {
    SCROLLINFO info;
    ::ZeroMemory(&info, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo(SB_VERT, &info);

    int low = info.nMin;
    int high = info.nMax - info.nPage;
    int newpos = info.nPos;

    CRect view;
    GetClientRect(&view);

    CRect image;
    c_Image.GetWindowRect(&image);
    ScreenToClient(&image);
    
    switch(nSBCode)
       { /* nSBCode */
	case SB_LINEUP:
	   newpos -= ::GetSystemMetrics(SM_CYHSCROLL);
	   if(newpos < low)
	       newpos = low;
	   break;
	case SB_LINEDOWN:
	   newpos += ::GetSystemMetrics(SM_CYHSCROLL);
	   if(newpos > high)
	       newpos = high;
	   break;
	case SB_PAGEUP:
	   newpos -= view.Height();
	   if(newpos < low)
	       newpos = low;
	   break;
	case SB_PAGEDOWN:
	   newpos += view.Height();
	   if(newpos > high)
	       newpos = high;
	   break;
	case SB_THUMBTRACK:
	   newpos = info.nTrackPos;
	   break;
	case SB_THUMBPOSITION:
	    newpos = info.nTrackPos;
	    break;
       } /* nSBCode */
    SetScrollPos(SB_VERT, newpos);
    c_Image.SetWindowPos(NULL,
			 image.left,
			 -newpos,
			 0, 0,
			 SWP_NOSIZE | SWP_NOZORDER);
    
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
   }

/****************************************************************************
*                                CShow::ShowBB
* Inputs:
*       const CRect & r: rectangle to show
* Result: void
*       
* Effect: 
*       Updates the bounding box
****************************************************************************/

void CShow::ShowBB(const CRect & r)
    {
     c_Image.ShowBB(r);
    } // CShow::ShowBB

/****************************************************************************
*                                CShow::ShowBB
* Inputs:
*       BOOL show: Show/hide
* Result: void
*       
* Effect: 
*       Shows or hides the bounding box
****************************************************************************/

void CShow::ShowBB(BOOL show)
    {
     c_Image.ShowBB(show);
    } // CShow::ShowBB

/****************************************************************************
*                              CShow::GetBitmap
* Result: CBitmap *
*       Bitmap object
****************************************************************************/

CBitmap * CShow::GetBitmap()
    {
    return CBitmap::FromHandle(c_Image.GetBitmap());
    } // CShow::GetBitmap

/****************************************************************************
*                            CShow::PostNcDestroy
* Result: void
*       
* Effect: 
*       Deletes the current object
****************************************************************************/

void CShow::PostNcDestroy() 
   {
    CDialog::PostNcDestroy();
    delete this;
   }
