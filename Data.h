class BitmapDisplayInfo {
    public:
       BitmapDisplayInfo() {}
       virtual ~BitmapDisplayInfo() {}
       virtual int Draw(CDC * dc, CRect & r) PURE;
       virtual int GetLine() { return -1; }
       virtual int GetLeft() { return -1; }
       virtual int GetRight() {return -1; }
};

/*****************************************************************************
*				   BitmapData
* An abstract class for the listbox display
*****************************************************************************/

class BitmapData : public BitmapDisplayInfo {
    public:
       BitmapData(int line) { scanline = line;}
       virtual ~BitmapData() {}
       virtual int Draw(CDC * dc, CRect & r) PURE;

       // The following are used to retrieve bounding box information
       // based on the scroll position
       virtual int GetLine() { return scanline; }
       virtual int GetLeft() { return left; }
       virtual int GetRight() { return right; }

       typedef enum { CLIP_CLEAR = -1, CLIP_LEFT, CLIP_NONE, CLIP_RIGHT } CLIPTYPE;
       void SetClip(CLIPTYPE clip, int x = 0);
    protected:
       virtual int GetFieldWidth() { return 4; }
       CArray<BYTE, BYTE> data;
       int padding;
       int width;
       int scanline; // current scan line value // set by constructor
       // These are set by the SetClip function
       int left;     // first x-pixel drawn
       int right;    // last x-pixel drawn
};

/*****************************************************************************
*				 BitmapPalette
* Palette data to display
*****************************************************************************/

class BitmapPalette : public BitmapDisplayInfo {
    public:
       BitmapPalette(int i, RGBQUAD c) { index = i; color = c; }
       virtual ~BitmapPalette() {}
       virtual int Draw(CDC * dc, CRect & r);
    protected:
       RGBQUAD color;
       int index;
};

/*****************************************************************************
*				   BitmapText
* Text to display
*****************************************************************************/

class BitmapText : public BitmapDisplayInfo {
    public:
       BitmapText(LPCTSTR t) { text = t; }
       virtual ~BitmapText() {}
       virtual int Draw(CDC * dc, CRect & r);
    protected:
       CString text;
};

/****************************************************************************
*                               BitmapFixedText
****************************************************************************/

class BitmapFixedText : public BitmapText {
    public:
       BitmapFixedText(LPCTSTR t) : BitmapText(t) { }
       virtual ~BitmapFixedText() { }
       virtual int Draw(CDC * dc, CRect & r);
};

/*****************************************************************************
*				  CRefPalette
*  A reference-counted palette.
*****************************************************************************/

class CRefPalette {
    public:
       CRefPalette(RGBQUAD * q) { count = 0; quads = q; }
       void AddRef() { count++; }
       void Release() { if(--count <= 0)
	                   { /* none left */
			    delete quads;
			    delete this;
			   } /* none left */
       };
       BYTE GetR(int i) { return quads != NULL ? quads[i].rgbRed : 255; }
       BYTE GetG(int i) { return quads != NULL ? quads[i].rgbGreen : 255; }
       BYTE GetB(int i) { return quads != NULL ? quads[i].rgbBlue : 255; }
    protected:
       virtual ~CRefPalette() { }
       int count;
       RGBQUAD * quads;
};

/*****************************************************************************
*				  BitmapRGB24
* Holds data for 24-bit bitmap
*
* Constructor:
*	LPBYTE p: Pointer to byte data
*	UINT len: Length of data
*	int w: Width of data
*	int ln: Scan line value
*	int pad: Number of bytes of data that are unused at the end of each line
*****************************************************************************/

class BitmapRGB24 : public BitmapData {
    public:
       BitmapRGB24(LPBYTE p, UINT len, int w, int ln, int pad) : BitmapData(ln) {
	  data.SetSize(len);
	  memcpy(data.GetData(), p, len);
	  padding = pad;
	  width = w;
       }
       virtual int Draw(CDC * dc, CRect & r);
    protected:
};


/****************************************************************************
*                                   BitmapP
* Abstract superclass of paletted bitmaps
****************************************************************************/

class BitmapP : public BitmapData {
    public:
       BitmapP(LPBYTE p, UINT len, int w, int ln, int pad, CRefPalette * pal) : BitmapData(ln) {
	  data.SetSize(len);
	  memcpy(data.GetData(), p, len);
	  palette = pal;
	  palette->AddRef();
	  width = w;
	  padding = pad;
       };
       virtual int Draw(CDC * dc, CRect & r) PURE;
       BYTE GetR(int i) { return palette->GetR(i); }
       BYTE GetG(int i) { return palette->GetG(i); }
       BYTE GetB(int i) { return palette->GetB(i); }
    protected:
       CRefPalette * palette;
};

/*****************************************************************************
*				    Bitmap8
* Holds data for paletted bitmap
*
* Constructor:
*	LPBYTE p: Pointer to bitmap data
*	UINT len: Length of the bitmap data
*	int w: bitmap width
*	int ln: Scan line #
*	int pad: Number of bytes of bitmap data that are padding to DWORD
*	CRefPalette * pal: Pointer to a reference-counted palette, or NULL
*****************************************************************************/

class Bitmap8 : public BitmapP {
    public:
       Bitmap8(LPBYTE p, UINT len, int w, int ln, int pad, CRefPalette * pal)
	  : BitmapP(p, len, w, ln, pad, pal) {
       };
       virtual ~Bitmap8() { palette->Release(); }
       virtual int Draw(CDC * dc, CRect & r);
    protected:
};

/*****************************************************************************
*				    Bitmap1
* Holds data for monochrome bitmap
*
* Constructor:
*	LPBYTE p: Pointer to bitmap data
*	UINT len: Length of bitmap data, in bytes
*	int w: Width in pixels
*	int ln: Scan line #
*	int pad: Number of bytes of padding to DWORD boundary
*	CRefPalette * pal: Pointer to palette, or NULL
*****************************************************************************/

class Bitmap1 : public BitmapP {
    public:
       Bitmap1(LPBYTE p, UINT len, int w, int ln, int pad, CRefPalette * pal)
	  : BitmapP(p, len, w, ln, pad, pal) { }
       virtual int Draw(CDC * dc, CRect & r);

    protected:
       virtual ~Bitmap1() { palette->Release(); }
       virtual int GetFieldWidth() { return 1; }
};

/*****************************************************************************
*				    Bitmap4
* Holds data for a 4-bit (16-color) bitmap
*
* Constructor:
*	LPBYTE p: Pointer to bitmap data
*	UINT len: Length of bitmap data, in bytes
*	int w: Width in pixels
*	int ln: Scan line #
*	int pad: Number of bytes of padding to DWORD boundary
*	CRefPalette * pal: Pointer to palette, or NULL
*****************************************************************************/

class Bitmap4 : public BitmapP {
    public:
       Bitmap4(LPBYTE p, UINT len, int w, int ln, int pad, CRefPalette * pal)
	  : BitmapP(p, len, w, ln, pad, pal) { }
       virtual int Draw(CDC * dc, CRect & r);

    protected:
       virtual ~Bitmap4() { palette->Release(); }
       virtual int GetFieldWidth() { return 2; }
};
