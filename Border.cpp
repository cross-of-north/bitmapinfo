// Border.cpp : implementation file
//

#include "stdafx.h"
#include "bitmapinfo.h"
#include "Border.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_BLINK 1

/////////////////////////////////////////////////////////////////////////////
// CBorder

CBorder::CBorder()
   {
    showing = FALSE;
   }

CBorder::~CBorder()
   {
   }


BEGIN_MESSAGE_MAP(CBorder, CWnd)
	//{{AFX_MSG_MAP(CBorder)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBorder message handlers

void CBorder::OnPaint() 
   {
    CPaintDC dc(this); // device context for painting
	
    dc.SetBkColor(RGB(0,0,0));
    CPen pen(PS_DOT, 0, RGB(255,255,255));
    dc.SelectObject(&pen);
    dc.SelectStockObject(HOLLOW_BRUSH);

    CRect r;
    GetClientRect(&r);
    dc.Rectangle(r);
    
    // Do not call CWnd::OnPaint() for painting messages
}

/****************************************************************************
*                               CBorder::Create
* Inputs:
*       CWnd * parent: Parent window
*	UINT id = IDC_STATIC: control ID
* Result: BOOL
*       TRUE if successful
*	FALSE if error
* Effect: 
*       Creates a window of nearly-zero size, which is invisible
****************************************************************************/

BOOL CBorder::Create(CWnd * parent, UINT id /* = IDC_STATIC */)
    {
     BOOL result =  CWnd::Create(NULL,  // class name
				 NULL,  // title
				 WS_CHILD | WS_CLIPSIBLINGS, // absent: WS_VISIBLE!
				 CRect(0,0,1,1),
				 parent,
				 id);
     if(!result)
	return FALSE;
     SetTimer(IDT_BLINK, ::GetCaretBlinkTime(), NULL);
     return TRUE;
    } // CBorder::Create

/****************************************************************************
*                               CBorder::ShowBB
* Inputs:
*       const CRect & r: Rectangle
* Result: void
*       
* Effect: 
*       Sets the window size
****************************************************************************/

void CBorder::ShowBB(const CRect & r)
    {
     CRect w = r;
     w.top--;    // move up 1 so area enclosed is selected area
     w.bottom--; // move down 1 so area enclosed is selected area
     w.right++;  // encompass right end
     SetWindowPos(NULL, w.left, w.top, w.Width(), w.Height(),
		  SWP_NOZORDER);
    } // CBorder::ShowBB

/****************************************************************************
*                               CBorder::ShowBB
* Inputs:
*       BOOL show: TRUE to show, FALSE to hide
* Result: void
*       
* Effect: 
*       Shows/hides the window
****************************************************************************/

void CBorder::ShowBB(BOOL show)
    {
     if(showing && show)
	return;
     showing = show;
     ShowWindow(show ? SW_SHOW : SW_HIDE);
    } // CBorder::ShowBB

/****************************************************************************
*                              CBorder::OnTimer
* Inputs:
*       UINT nIDEvent: Event ID, ignored (there is only one timer)
* Result: void
*       
* Effect: 
*       Blinks the window
****************************************************************************/

void CBorder::OnTimer(UINT nIDEvent) 
   {
    if(!showing)
       return;    // do nothing

    if(IsWindowVisible())
       ShowWindow(SW_HIDE);
    else
       ShowWindow(SW_SHOW);
	
    CWnd::OnTimer(nIDEvent);
   }
