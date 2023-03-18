// BitmapDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BitmapDisplay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapDisplay

CBitmapDisplay::CBitmapDisplay()
{
}

CBitmapDisplay::~CBitmapDisplay()
{
}


BEGIN_MESSAGE_MAP(CBitmapDisplay, CListBox)
	//{{AFX_MSG_MAP(CBitmapDisplay)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapDisplay message handlers

void CBitmapDisplay::DrawItem(LPDRAWITEMSTRUCT dis) 
   {
    CDC * dc = CDC::FromHandle(dis->hDC);

    if(dis->itemID == (UINT)-1)
       { /* no items */
	CBrush bg(::GetSysColor(COLOR_WINDOW));
	dc->FillRect(&dis->rcItem, &bg);
	if(dis->itemState & ODS_FOCUS)
	   { /* selected */
	    dc->DrawFocusRect(&dis->rcItem);
	   } /* selected */
	return;
       } /* no items */

    BitmapData * data = (BitmapData *)dis->itemData;

    int saved = dc->SaveDC();

    COLORREF txcolor;
    COLORREF bkcolor;

    if(dis->itemState & ODS_SELECTED)
       { /* selected */
	if(::GetFocus() == m_hWnd)
	   { /* has focus */
	    bkcolor = ::GetSysColor(COLOR_HIGHLIGHT);
	    txcolor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	   } /* has focus */
	else
	   { /* no focus */
	    bkcolor = ::GetSysColor(COLOR_BTNFACE);
	    txcolor = ::GetSysColor(COLOR_BTNTEXT);
	   } /* no focus */
       } /* selected */
    else
       { /* not selected */
	txcolor = ::GetSysColor(COLOR_WINDOWTEXT);
	bkcolor = ::GetSysColor(COLOR_WINDOW);
       } /* not selected */
    {
     CBrush bg(bkcolor);
     dc->FillRect(&dis->rcItem, &bg);
    }

    dc->SetTextColor(txcolor);
    dc->SetBkMode(TRANSPARENT);
    
    // This adjusts the rectangle from the rectangle proposed by the
    // listbox drawing logic to a correct one that handles the
    // client size that is visible. This is needed because of
    // how we compute bounding boxes.
    CPoint wo = dc->GetWindowOrg();
    CRect client;
    GetClientRect(&client);
    client.left = dis->rcItem.left;
    client.top = dis->rcItem.top;
    client.bottom = dis->rcItem.bottom;
    //client.right += wo.x;

    int width = data->Draw(dc, CRect(client));
    int w = GetHorizontalExtent();
    if(width > w)
       SetHorizontalExtent(width);

    if(dis->itemState & ODS_FOCUS)
       dc->DrawFocusRect(&dis->rcItem);

    dc->RestoreDC(saved);
   }

void CBitmapDisplay::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	
}

void CBitmapDisplay::DeleteItem(LPDELETEITEMSTRUCT dis) 
   {
    BitmapData * data = (BitmapData *)dis->itemData;
    delete data;
   }

/****************************************************************************
*                       CBitmapDisplay::GetBoundingRect
* Inputs:
*       CRect & r: A rectangle to be filled in
* Result: BOOL
*       TRUE if r is filled in, something is visible
*	FALSE if nothing is visible; r is undefined
****************************************************************************/

BOOL CBitmapDisplay::GetBoundingRect(CRect & r)
    {
     // This iterates from the top line to the bottom line of
     // what is visible on the screen
     // It asks each line "are you a bitmap scan line?" and
     // if the answer is yes, it retrieves the left and right
     // information
     int topline = GetTopIndex();
     int h = GetItemHeight(topline);
     BOOL valid = FALSE;
     CRect view;
     GetClientRect(&view);
     int pos = 0;
     BOOL seen = FALSE;

     for(int i = topline; i < GetCount(); i++)
	{ /* scan for info */
	 pos += h;
	 if(pos > view.bottom)
	    break;

	 BitmapDisplayInfo * info = (BitmapDisplayInfo *)GetItemDataPtr(i);
	 int line = info->GetLine();
	 if(line < 0)
	    continue; // not a bitmap information line

	 valid = TRUE; // we have seen at least one

	 if(!seen)
	    { /* first line */
	     r.bottom = line;
	     r.top = line; // in case it is the only one
	     r.left = info->GetLeft();
	     r.right = info->GetRight();
	     seen = TRUE;
	    } /* first line */
	 else
	    { /* other line */
	     r.top = line;
	    } /* other line */
	} /* scan for info */
     return valid;
    } // CBitmapDisplay::GetBoundingRect
