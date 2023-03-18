#include "stdafx.h"
#include "pal.h"

/****************************************************************************
*                       CBitMapInfoDlg::GetPaletteSize
* Inputs:
*       int n: Number of bits
* Result: int
*       Number of bits required for palette
*	n	return
*	1	2
*	4	16
*	8	256
*	16	0
*	24	0
*	32	0
****************************************************************************/

int GetPaletteSize(DWORD n)
   {
    switch(n)
       { /* n */
	case 1:
	   return 2;
	case 4:
	   return 16;
	case 8:
	   return 256;
	default:
	   return 0;
       } /* n */
   } // GetPaletteSize

/****************************************************************************
*                               GetPaletteSize
* Inputs:
*       BITMAPINFOHEADER & hdr
* Result: int
*       Size of the palette
****************************************************************************/

int GetPaletteSize(BITMAPINFOHEADER & hdr)
   {
    int colors = hdr.biClrUsed;
    if(colors == 0)
       colors = GetPaletteSize(hdr.biBitCount);
    return colors;
   } // GetPaletteSize
