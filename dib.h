HANDLE MakeDIB(CBitmap * bmp);
void PrintDIB(CDC * dc, HANDLE dib, int x, int y, int width, int height);
BOOL DibInfo (HANDLE hbi, LPBITMAPINFOHEADER lpbi);
void FreeDIB(HANDLE dib);
