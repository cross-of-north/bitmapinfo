#include "stdafx.h"
#include "dib.h"

/*****************************************************************************
	      This code comes from the DIBSHOW example of the MSDN

	      This is why it drops down to raw API functions rather
	      than using MFC.
*****************************************************************************/


/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)
static WORD DibNumColors (VOID FAR * pv);
static WORD PaletteSize (VOID FAR * pv);

/****************************************************************************
*                                StrecthDibBlt
* Inputs:
*       CDC * dc: DC
* Result: BOOL
*       The return value of StretchBlt
* Effect: 
*       Draws the pixels on the DC, scaled
****************************************************************************/

static BOOL StretchDibBlt (
			   CDC * dc,
			   INT x,
			   INT y,
			   INT dx,
			   INT dy,
			   HANDLE hdib,
			   INT x0,
			   INT y0,
			   INT dx0,
			   INT dy0,
			   LONG rop)

   {
    LPBITMAPINFOHEADER lpbi;
    LPBYTE       pBuf;
    BOOL         f;

    // If there isn't a DIB provided, erase the area using
    // PatBlt to paint with the selected brush
    if (hdib == NULL)
       return dc->PatBlt(x,y,dx,dy,rop);

    // This function uses the old Win16 GlobalLock mechanism
    // for compatibility. 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
       return FALSE;

    pBuf = (LPBYTE)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

    f = ::StretchDIBits ( dc->m_hDC,
			  x, y,
			  dx, dy,
			  x0, y0,
			  dx0, dy0,
			  pBuf,
			  (LPBITMAPINFO)lpbi,
			  DIB_RGB_COLORS,
			  rop);

    GlobalUnlock(hdib);
    return f;
   }


/****************************************************************************
*                                   DibInfo
* Inputs:
*       HANDLE hbi: DIB handle
*	LPBITMAPINFOHEADER lpbi
* Result: BOOL
*       ¶
* Effect: 
*       Returns the bitmap info
****************************************************************************/

BOOL DibInfo (HANDLE hbi, LPBITMAPINFOHEADER lpbi)
   {
    if (hbi)
       {
	*lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

	/* fill in the default fields */
	if (lpbi->biSize != sizeof (BITMAPCOREHEADER))
	   {
	    if (lpbi->biSizeImage == 0L)
	       lpbi->biSizeImage = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;
	    
	    if (lpbi->biClrUsed == 0L)
	       lpbi->biClrUsed = DibNumColors (lpbi);
	   }
	GlobalUnlock (hbi);
	return TRUE;
       }
    return FALSE;
   }	

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/
static WORD DibNumColors (VOID FAR * pv)
   {
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
       if (lpbi->biClrUsed != 0)
	  return (WORD)lpbi->biClrUsed;
       bits = lpbi->biBitCount;
    }
    else
       bits = lpbc->bcBitCount;

    switch (bits){
	case 1:
	   return 2;
	case 4:
	   return 16;
	case 8:
	   return 256;
	default:
		/* A 24 bitcount DIB has no color table */
	   return 0;
    }
   }

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *                is of the BITMAPCOREHEADER type, the number of colors is  *
 *                multiplied by 3 to give the palette size, otherwise the   *
 *                number of colors is multiplied by 4.                      *
 *                                                                          *
 *  RETURNS    :  Palette size in number of bytes.                          *
 *                                                                          *
 ****************************************************************************/
WORD PaletteSize (VOID FAR * pv)
   {
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
       return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
       return (WORD)(NumColors * sizeof(RGBQUAD));
   }

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibFromBitmap()                                            *
 *                                                                          *
 *  PURPOSE    : Will create a global memory block in DIB format that       *
 *               represents the Device-dependent bitmap (DDB) passed in.    *
 *                                                                          *
 *  RETURNS    : A handle to the DIB                                        *
 *                                                                          *
 ****************************************************************************/
static HANDLE DibFromBitmap (
		      HBITMAP      hbm,
		      DWORD            biStyle,
		      WORD             biBits,
		      HPALETTE     hpal)
   {
    BITMAP               bm;
    BITMAPINFOHEADER     bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD                dwLen;
    HANDLE               hdib;
    HANDLE               h;
    HDC                  hdc;

    if (!hbm)
       return NULL;

    if (hpal == NULL)
       hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);

    if (biBits == 0)
       biBits =  bm.bmPlanes * bm.bmBitsPixel;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight; // create bottom-to-top!
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = biStyle;
    bi.biSizeImage          = 0;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = 0;
    bi.biClrImportant       = 0;

    dwLen  = bi.biSize + PaletteSize(&bi);

    hdc = GetDC(NULL);
    hpal = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);

    hdib = GlobalAlloc(GHND,dwLen);

    if (!hdib){
       SelectPalette(hdc,hpal,FALSE);
       ReleaseDC(NULL,hdc);
       return NULL;
    }

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    *lpbi = bi;

    /*  call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
    GetDIBits(hdc, hbm, 0L, (DWORD)bi.biHeight,
	      (LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

    bi = *lpbi;
    GlobalUnlock(hdib);

    /* If the driver did not fill in the biSizeImage field, make one up */
    if (bi.biSizeImage == 0){
       bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

       if (biStyle != BI_RGB)
	  bi.biSizeImage = (bi.biSizeImage * 3) / 2;
    }

    /*  realloc the buffer big enough to hold all the bits */
    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;
    if (h = GlobalReAlloc(hdib,dwLen,0))
       hdib = h;
    else{
       GlobalFree(hdib);
       hdib = NULL;

       SelectPalette(hdc,hpal,FALSE);
       ReleaseDC(NULL,hdc);
       return hdib;
    }

    /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (GetDIBits( hdc,
		   hbm,
		   0L,
		   (DWORD)bi.biHeight,
		   (LPBYTE)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi),
		   (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS) == 0){
       GlobalUnlock(hdib);
       hdib = NULL;
       SelectPalette(hdc,hpal,FALSE);
       ReleaseDC(NULL,hdc);
       return NULL;
    }

    bi = *lpbi;
    GlobalUnlock(hdib);

    SelectPalette(hdc,hpal,FALSE);
    ReleaseDC(NULL,hdc);
    return hdib;
   }

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : BitmapFromDib(HANDLE hdib, HPALETTE hpal)                  *
 *                                                                          *
 *  PURPOSE    : Will create a DDB (Device Dependent Bitmap) given a global *
 *               handle to a memory block in CF_DIB format                  *
 *                                                                          *
 *  RETURNS    : A handle to the DDB.                                       *
 *                                                                          *
 ****************************************************************************/

static HBITMAP BitmapFromDib (
		       HANDLE         hdib,
		       HPALETTE   hpal)
   {
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    HDC                 hdc;
    HBITMAP             hbm;
    
    if (!hdib)
       return NULL;
    
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
       return NULL;

    hdc = GetDC(NULL);

    if (hpal)
       {
	hpalT = SelectPalette(hdc,hpal,FALSE);
	RealizePalette(hdc);     // GDI Bug...????
       }	

    hbm = CreateDIBitmap(hdc,
			 (LPBITMAPINFOHEADER)lpbi,
			 (LONG)CBM_INIT,
			 (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
			 (LPBITMAPINFO)lpbi,
			 DIB_RGB_COLORS );
    
    if (hpal)
       SelectPalette(hdc,hpalT,FALSE);
    
    ReleaseDC(NULL,hdc);
    GlobalUnlock(hdib);

    return hbm;
   }

/****************************************************************************
*                                   MakeDIB
* Inputs:
*       CBitmap * bmp: Input bitmap 
* Result: CBitmap *
*       Output bitmap
* Effect: 
*       Converts the input bitmap to an output bitmap
****************************************************************************/

HANDLE MakeDIB(CBitmap * bmp)
    {
     return DibFromBitmap((HBITMAP)*bmp, BI_RGB, 0, NULL);
    } // MakeDIB

/****************************************************************************
*                                  PrintDIB
* Inputs:
*       CDC * dc: DC to use
*	HANDLE dib: Handleto DIB
*	int x:
*	int y:
*	int width:
*	int height:
* Result: void
*       
* Effect: 
*       Draws the DIB in the location
****************************************************************************/

VOID PrintDIB (CDC * dc, HANDLE dib, INT x, INT y, INT dx, INT dy)
   {
    BITMAPINFOHEADER bi;

    DibInfo (dib, &bi);

    if (dib != NULL)
       {
	/* Stretch the DIB to printer DC */
	StretchDibBlt ( dc,
			x,
			y,
			dx,
			dy,
			dib,
			0,
			0,
			bi.biWidth,
			bi.biHeight,
			SRCCOPY);
       }
   }

/****************************************************************************
*                                   FreeDIB
* Inputs:
*       HANDLE dib: DIB to free
* Result: void
*       
* Effect: 
*       Frees the DIB
****************************************************************************/

void FreeDIB(HANDLE dib)
    {
     GlobalFree(dib);
    } // FreeDIB
