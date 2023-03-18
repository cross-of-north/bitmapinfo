#include "stdafx.h"
#include "Data.h"

static int DrawTupleItem(CDC * dc,
			 int left,
			 const CRect & r,
			 BYTE d,
			 int fieldwidth,
			 BitmapData::CLIPTYPE & clipped,
			 COLORREF bkcolor,
			 COLORREF textcolor,
			 COLORREF pure = RGB(0,0,0),
			 BOOL stripes = FALSE);

static BYTE complement(BYTE color);
static COLORREF complement(COLORREF color);

/****************************************************************************
*                              BitmapText::Draw
* Inputs:
*       CDC * dc: DC to draw in
*	CRect & r: Rectangle for drawing
* Result: int
*       Width of text string drawn
* Effect: 
*       Draws the text string
****************************************************************************/

int BitmapText::Draw(CDC * dc, CRect & r)
   {
    dc->TextOut(r.left, r.top, text);
    return dc->GetTextExtent(text).cx;
   } // BitmapText::Draw

/****************************************************************************
*                            BitmapFixedText::Draw
* Inputs:
*       CDC * dc: DC
*	CRect & r:
* Result: int
*       
* Effect: 
*       Draws the data as fixed-pitch text
****************************************************************************/

int BitmapFixedText::Draw(CDC * dc, CRect & r)
    {
     int saved = dc->SaveDC();
     LOGFONT lf;
     ::ZeroMemory(&lf, sizeof(lf));
     lf.lfHeight = r.Height();
     lf.lfWeight = FW_NORMAL;
     lstrcpy(lf.lfFaceName, _T("Courier New"));

     CFont fixed;
     fixed.CreateFontIndirect(&lf);
     dc->SelectObject(&fixed);

     dc->TextOut(r.left, r.top, text);
     int result = dc->GetTextExtent(text).cx;

     dc->RestoreDC(saved);
     return result;
    } // BitmapFixedText::Draw

/****************************************************************************
*                         BitmapRGB24::DrawTupleItem
* Inputs:
*       int left: Place to start drawing
*	CRect r: Rectangle info
*       BYTE d: Data byte to display
*       int fieldwidth: Desired field width, in character positions
*	COLORREF bkcolor: Color to use for background
*       COLORREF textcolor: Color to use for text
*	COLORREF pure: Pure color, only for 24-bit mode
*	BitmapData::CLIPTYPE & clipped: Set to the result of the clipping
*	BOOL stripes: TRUE for 24-bit-color striping
* Result: int
*       Width of item drawn
* Effect: 
*       Draws the tuple item
****************************************************************************/

static int DrawTupleItem(CDC * dc, int left, const CRect & r, BYTE d, int fieldwidth, BitmapData::CLIPTYPE & clipped, COLORREF bkcolor, COLORREF textcolor, COLORREF pure /* = RGB(0,0,0) */, BOOL stripes /* = FALSE */)
    {
     // This code optimizes the drawing speed by not drawing anything
     // that is clipped to the left or right
     int column = dc->GetTextExtent(CString(_T('0'), fieldwidth)).cx;
     CPoint wo = dc->GetWindowOrg();
     if(left + column < wo.x)
	{ /* clip left */
	 clipped = BitmapData::CLIP_LEFT;
	 return column;          // don't try to draw anything clipped to the left
	} /* clip left */
     if(left > wo.x + r.Width())
	{ /* clip right */
	 clipped = BitmapData::CLIP_RIGHT;
	 return column;          // don't try to draw anything clipped to the right
	} /* clip right */
     clipped = BitmapData::CLIP_NONE;

     int saved = dc->SaveDC();

     CString s;
     s.Format(_T("%d"), d);
     int width = dc->GetTextExtent(s).cx;

     if(!stripes)
	{ /* no stripes */
	 CRect fill(CPoint(left, r.top), CSize(column, r.Height()));
	 dc->FillSolidRect(&fill, bkcolor);
	 dc->SetTextColor(textcolor);
	 dc->TextOut(left + (column - width) / 2, r.top, s);
	} /* no stripes */
     else
	{ /* stripes */
	 CRect fill(CPoint(left, r.top), CSize(column, r.Height()));
	 dc->FillSolidRect(&fill, pure);
	 dc->SetTextColor(complement(pure));
	 dc->TextOut(left + (column - width) / 2, r.top, s);
	 CPen pen(PS_SOLID, 0, bkcolor);
	 dc->SelectObject(&pen);
	 dc->MoveTo(left, r.bottom - 1);
	 dc->LineTo(left + column, r.bottom - 1);
	} /* stripes */

     dc->RestoreDC(saved);
     return column;
    } // DrawTupleItem

/****************************************************************************
*                                 complement
* Inputs:
*       COLORREF color: Color desired
* Result: COLORREF
*       Complementary color
****************************************************************************/

static COLORREF complement(COLORREF color)
    {
     return RGB(complement(GetRValue(color)),
		complement(GetGValue(color)),
		complement(GetBValue(color)));
    } // complement

/****************************************************************************
*                                 complement
* Inputs:
*       BYTE color: Color to complement
* Result: BYTE
*       New color
* Notes: 
*       If a color is complemented and is within a specified delta of the
*	original color, it is set to either 0 or 255 based on the
*	current color
****************************************************************************/

static BYTE complement(BYTE color)
    {
#define DELTA 20
     BYTE newcolor = color ^ 0xFF;
     if( abs((int)newcolor - (int)color) < DELTA)
	{ /* reset */
	 if(color < 128)
	    newcolor = 255;
	 else
	    newcolor = 0;
	} /* reset */
     return newcolor;
    } // complement

/****************************************************************************
*                                Bitmap::Draw
* Inputs:
*       CDC * dc: DC to draw to
*	CRect & r: Drawing rectangle
* Result: int
*       Width if item drawn
* Effect: 
*       Draws a palette entry
*	offset	color-sample	B  G  R
****************************************************************************/

int BitmapPalette::Draw(CDC * dc, CRect & r)
   {
    int save = dc->SaveDC();
    int left = r.left;

    LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = r.Height();
    lf.lfWeight = FW_NORMAL;
    lstrcpy(lf.lfFaceName, _T("Courier New"));

    CFont fixed;
    fixed.CreateFontIndirect(&lf);
    dc->SelectObject(&fixed);

    int width;

    width = dc->GetTextExtent(CString(_T("000 "))).cx;

    CString s;
    s.Format(_T("%3d"), index);
    dc->TextOut(left, r.top, s);
    left += width;

    width = dc->GetTextExtent(CString(_T("        "))).cx;
    dc->FillSolidRect(CRect(CPoint(left, r.top), CSize(width, r.Height())),
	              RGB(color.rgbRed, color.rgbGreen, color.rgbBlue));
    
    left += width;
    BitmapData::CLIPTYPE clipped; // ignored
    
    left += DrawTupleItem(dc, left, r, color.rgbBlue, 4, clipped, RGB(0, 0, 255), RGB(255,255,255));
    left += DrawTupleItem(dc, left, r, color.rgbGreen, 4, clipped, RGB(0, 255, 0), RGB(0,0,0));
    left += DrawTupleItem(dc, left, r, color.rgbRed, 4, clipped, RGB(255, 0, 0), RGB(255,255,255));
    left += DrawTupleItem(dc, left, r, color.rgbReserved, 4, clipped, RGB(255, 255, 255), RGB(0, 0, 0));
    
    dc->RestoreDC(save);
    return left;
   } // BitmapPalette::Draw

/****************************************************************************
*                              BitmapRGB24::Draw
* Inputs:
*       CDC * dc: DC to draw in
*	CRect & r: Rectangle to draw in
* Result: int
*       Width of drawing
* Effect: 
*       Draws a 24-bit bitmap
****************************************************************************/

int BitmapRGB24::Draw(CDC * dc, CRect & r)
   {
    // Data is in multiples of RGB24 tuples
    int save = dc->SaveDC();
    int left = r.left;

    LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = r.Height();
    //lf.lfHeight = (int)((float)lf.lfHeight * 0.9f);
    lf.lfWeight = FW_NORMAL;
    lstrcpy(lf.lfFaceName, _T("Courier New"));

    CFont fixed;
    fixed.CreateFontIndirect(&lf);
    dc->SelectObject(&fixed);
    BOOL gapped = FALSE;

    BitmapData::CLIPTYPE clipped; // indicates clipping
    SetClip(CLIP_CLEAR);
    
    for(int i = 0; i < data.GetSize(); i++)
       { /* draw each */
	if(i < width * 3)
	   { /* real pixel */
	    switch(i % 3)
	       { /* decode */
		case 0: 
		   //****************************************************************
		   // BLUE PIXEL
		   //****************************************************************
#define SEP_THRESHOLD 224  // use gray separator line if value is > this

		   if(i > 0 && data[i - 1] > SEP_THRESHOLD && data[i] > SEP_THRESHOLD)
		      { /* gray separator */
		       CPen pen(PS_SOLID, 0, RGB(SEP_THRESHOLD, SEP_THRESHOLD, SEP_THRESHOLD));
		       dc->SelectObject(&pen);
		       dc->MoveTo(left - 1, r.top);
		       dc->LineTo(left - 1, r.bottom);
		      } /* gray separator */

		   left += DrawTupleItem(dc,
					 left,
					 r,
					 data[i],
					 GetFieldWidth(),
					 clipped,
					 RGB(0, 0, 255),
					 RGB(255,255,255),
					 RGB(data[i+2], data[i+1], data[i]),
					 TRUE);
		   break;
		   //****************************************************************
		case 1:
		   //****************************************************************
		   // GREEN PIXEL
		   //****************************************************************
		   left += DrawTupleItem(dc,
					 left,
					 r,
					 data[i],
					 GetFieldWidth(),
					 clipped,
					 RGB(0, 255, 0),
					 RGB(0,0,0),
					 RGB(data[i+1], data[i], data[i-1]),
					 TRUE);
		   break;
		   //****************************************************************
		case 2:
		   //****************************************************************
		   // RED PIXEL
		   //****************************************************************
		   left += DrawTupleItem(dc,
					 left,
					 r,
					 data[i],
					 GetFieldWidth(),
					 clipped,
					 RGB(255, 0, 0),
					 RGB(255,255,255),
					 RGB(data[i], data[i - 1], data[i - 2]),
					 TRUE);
		   left++; // interpixel gap
		   //****************************************************************
	       } /* decode */
	   } /* real pixel */
	else
	   { /* padding pixel */
	    if(!gapped)
	       { /* draw gap */
		left += ::GetSystemMetrics(SM_CXFRAME);
		gapped = TRUE;
	       } /* draw gap */
	    left += DrawTupleItem(dc, left, r, data[i], GetFieldWidth(), clipped, RGB(192, 192, 192), RGB(0,0,0));
	   } /* padding pixel */
	SetClip(clipped, i / 3);
       } /* draw each */
    dc->RestoreDC(save);
    return left;
   } // Bitmap24::Draw

/****************************************************************************
*                                Bitmap8::Draw
* Inputs:
*       CDC * dc: DC to use
*	CRect & r: Reference rectangle
* Result: int
*       Width drawn
* Effect: 
*       Draws an 8-bit (paletted) bitmap
****************************************************************************/

int Bitmap8::Draw(CDC * dc, CRect & r)
   {
    // Data is in multiples of RGB24 tuples
    int save = dc->SaveDC();
    int left = r.left;

    LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = r.Height();
    lf.lfWeight = FW_NORMAL;
    lstrcpy(lf.lfFaceName, _T("Courier New"));

    CFont fixed;
    fixed.CreateFontIndirect(&lf);
    dc->SelectObject(&fixed);

    BOOL gapped = FALSE;
    BitmapData::CLIPTYPE clipped;
    SetClip(CLIP_CLEAR);

    for(int i = 0; i < data.GetSize(); i++)
       { /* draw each */
	COLORREF bk;
	COLORREF tx;

	if(i < width)
	   { /* real pixel */
	    bk = RGB(GetR(data[i]), GetG(data[i]), GetB(data[i]));
	    tx = RGB(complement(GetR(data[i])), complement(GetG(data[i])), complement(GetB(data[i])));
	   } /* real pixel */
	else
	   { /* padding pixel */
	    if(!gapped)
	       { /* draw gap */
		left += ::GetSystemMetrics(SM_CXFRAME);
		gapped = TRUE;
	       } /* draw gap */

	    bk = RGB(192, 192, 192);
	    tx = RGB(0, 0, 0);
	   } /* padding pixel */

	left += DrawTupleItem(dc, left, r, data[i], GetFieldWidth(), clipped, bk, tx);
	SetClip(clipped, i);
       } /* draw each */
    dc->RestoreDC(save);
    return left;
   } // Bitmap8::Draw

/****************************************************************************
*                                Bitmap1::Draw
* Inputs:
*       CDC * dc: DC to use
*	CRect & r: Rectangle to draw in
* Result: int
*       Width of item drawn
* Effect: 
*       Draws a 1-pixel bitmap
****************************************************************************/

int Bitmap1::Draw(CDC * dc, CRect & r)
    {
     // Data is in 8-pixel bytes

     int save = dc->SaveDC();
     int left = r.left;

     LOGFONT lf;
     ::ZeroMemory(&lf, sizeof(lf));
     lf.lfHeight = r.Height();
     lf.lfWeight = FW_NORMAL;
     lstrcpy(lf.lfFaceName, _T("Courier New"));

     CFont fixed;
     fixed.CreateFontIndirect(&lf);
     dc->SelectObject(&fixed);

     BitmapData::CLIPTYPE clipped;
     SetClip(CLIP_CLEAR);

     for(int i = 0; i < data.GetSize() * 8; i++)
	{ /* draw each */
	 COLORREF bk;
	 COLORREF tx;
	 BYTE bunch = data[i / 8];  // An 8-bit bunch of pixels
	 BYTE pixel = (bunch >> (7 - (i % 8))) & 0x01; // an individual pixel
         //****************************************************************
         // Note the above takes into account the fact that h/o bit
	 // is the first pixel, e.g.,
	 //   0x93 = 10010011 draws as *__*__**
	 // So if the following values of i hold, the bits are as shown
	 //	i   (i % 8)   (7 - i % 8)      bit
	 //------------------------------------------
	 //	0	0	7		7
	 //	1	1	6		6
	 //	2	2	5		5
	 //...
	 //     7	7	0		0
	 //	8	0	7		7
	 //****************************************************************
	 
	 if(i < width)
	    { /* real pixel */
	     bk = RGB(GetR(pixel), GetG(pixel), GetB(pixel));
	     tx = RGB(complement(GetR(pixel)), complement(GetG(pixel)), complement(GetB(pixel)));
	    } /* real pixel */
	 else
	    { /* padding pixel */
	     bk = RGB(192, 192, 192);
	     tx = RGB(0, 0, 0);
	    } /* padding pixel */

	 if(i > 0 && i % 8 == 0)
	    { /* byte delimiter */
	     int w = ::GetSystemMetrics(SM_CXFRAME);
	     CRect gap(CPoint(left, r.top), CSize(w, r.Height()));
	     COLORREF c = i < width ? RGB(192, 192, 192) : RGB(255, 255, 255);
	     dc->FillSolidRect(&gap, c);
	     left += w;
	    } /* byte delimiter */
	 left += DrawTupleItem(dc, left, r, pixel, GetFieldWidth(), clipped, bk, tx);
	 SetClip(clipped, i);
	} /* draw each */
     dc->RestoreDC(save);
     return left;
    } // Bitmap1::Draw

/****************************************************************************
*                                Bitmap4::Draw
* Inputs:
*       CDC * dc: DC to use
*	CRect & r: Rectangle to draw in
* Result: int
*       Width of item drawn
* Effect: 
*       Draws a 1-pixel bitmap
****************************************************************************/

int Bitmap4::Draw(CDC * dc, CRect & r)
   {
    // Data is in 8-pixel bytes

    int save = dc->SaveDC();
    int left = r.left;

    LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight = r.Height();
    lf.lfWeight = FW_NORMAL;
    lstrcpy(lf.lfFaceName, _T("Courier New"));

    CFont fixed;
    fixed.CreateFontIndirect(&lf);
    dc->SelectObject(&fixed);

    BitmapData::CLIPTYPE clipped;
    SetClip(CLIP_CLEAR);

    for(int i = 0; i < data.GetSize() * 2; i++)
       { /* draw each */
	COLORREF bk;
	COLORREF tx;
	BYTE bunch = data[i / 2];  // An 8-bit bunch of pixels
	BYTE pixel = (bunch >> (4 * (1 - (i  & 0x1)))) & 0x0F; // an individual pixel
        //****************************************************************
	// Note the above takes into account the fact that h/o bit
	// is the first pixel, e.g.,
	//   0x93 = 10010011 draws as 9 3
	// So if the following values of i hold, the bits are as shown
	//	i   (i & 1)   (1 - (i & 0x1))   4*(...)   nybble
	//-------------------------------------------------------
	//	0	0	1		4	   h/o	
	//	1	1	0		0	   l/o
	//	2	0	1		4	   h/o
	//      3	1	0		0	   l/o
	//****************************************************************

       if(i < width)
	  { /* real pixel */
	   bk = RGB(GetR(pixel), GetG(pixel), GetB(pixel));
	   tx = RGB(complement(GetR(pixel)), complement(GetG(pixel)), complement(GetB(pixel)));
	  } /* real pixel */
       else
	  { /* padding pixel */
	   bk = RGB(192, 192, 192);
	   tx = RGB(0, 0, 0);
	  } /* padding pixel */

       if(i > 0 && (i & 0x1) == 0)
	  { /* byte delimiter */
	   int w = ::GetSystemMetrics(SM_CXFRAME);
	   CRect gap(CPoint(left, r.top), CSize(w, r.Height()));
	   COLORREF c = i < width ? RGB(192, 192, 192) : RGB(255, 255, 255);
	   dc->FillSolidRect(&gap, c);
	   left += w;
	  } /* byte delimiter */
       left += DrawTupleItem(dc, left, r, pixel, GetFieldWidth(), clipped, bk, tx);
       SetClip(clipped, i);
      } /* draw each */
   dc->RestoreDC(save);
   return left;
   } // Bitmap4::Draw

/****************************************************************************
*                             BitmapData::SetClip
* Inputs:
*       BitmapData::CLIPTYPE clip: Clip type
*	int x: x-dimension
* Result: void
*       
* Effect: 
*       Sets the clip information
* Notes:
*	Before starting, call with CLIP_CLEAR which resets the values
*	
****************************************************************************/

void BitmapData::SetClip(BitmapData::CLIPTYPE clip, int x /* = 0 */)
    {
     switch(clip)
	{ /* clip */
	 case BitmapData::CLIP_CLEAR:
	    left = right = 0;
	    break;
	 case BitmapData::CLIP_LEFT:
	    left = x;
	    break;
	 case BitmapData::CLIP_RIGHT:
	    if(right == 0)
	       right = x;
	    break;
	 case BitmapData::CLIP_NONE:
	    break;
	 default:
	       ASSERT(FALSE);
	} /* clip */
    } // BitmapData::SetClip
