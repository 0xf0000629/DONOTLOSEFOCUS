#define _WIN32_WINNT 0x0500
#include <windows.h>
#include<tchar.h>
#include <stdio.h>
//#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <conio.h>
#include <ole2.h>
#include <olectl.h>
#include <gdiplus.h>
#include <stdint.h>

BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
    HDC hDC;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal2 = NULL;
    hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else
        wBitCount = 24;
    GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = -Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 256;
    dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
        * Bitmap0.bmHeight;
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }


    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

    if (hOldPal2)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}

int screenCapturePart(LPCSTR fname){
    RECT cr;
    HWND hwnd = GetConsoleWindow();
    GetWindowRect(hwnd, &cr);
    HDC hdcSource = GetWindowDC(hwnd);

    LONG w = cr.right-cr.left;
    LONG h = cr.bottom-cr.top;

    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w-8-25, h-31-8, hdcSource, 8, 31, SRCCOPY);

    SaveHBITMAPToFile(hBitmap, fname);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
}
void grayscale(HBITMAP hbitmap, int huh, int ok, int inv)
{
    BITMAP bm;
    GetObject(hbitmap, sizeof(bm), &bm);
    if(bm.bmBitsPixel < 24)
    {
        DebugBreak();
        return;
    }

    HDC hdc = GetDC(HWND_DESKTOP);
    DWORD size = ((bm.bmWidth * bm.bmBitsPixel + 31) / 32) * 4 * bm.bmHeight;

    BITMAPINFO bmi =
    {sizeof(BITMAPINFOHEADER),bm.bmWidth,bm.bmHeight,1,bm.bmBitsPixel,BI_RGB,size};

    int stride = bm.bmWidth + (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
    unsigned char *bits = (unsigned char *)malloc(size * sizeof(unsigned char));
    GetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
    for(int y = 0; y < bm.bmHeight; y++) {
        for(int x = 0; x < stride; x++) {
            int i = (x + y * stride) * bm.bmBitsPixel / 8;
            if (huh & 1) bits[i+0] = ok;
            if (huh & 2) bits[i+1] = ok;
            if (huh & 4) bits[i+2] = ok;
            if (inv){
                if (!(huh & 1)) bits[i+0] = 255-bits[i+0];
                if (!(huh & 2)) bits[i+1] = 255-bits[i+1];
                if (!(huh & 4)) bits[i+2] = 255-bits[i+2];
            }
        }
    }

    SetDIBits(hdc, hbitmap, 0, bm.bmHeight, bits, &bmi, DIB_RGB_COLORS);
    ReleaseDC(HWND_DESKTOP, hdc);
    free(bits);
}
int w = 1920;
int h = 1080;
void glitch(HDC hdc, int setcolor, int val){

    HDC hdcMemory = CreateCompatibleDC(hdc);
    HDC hdcMemory2 = CreateCompatibleDC(hdc);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP hBitmap2 = CreateCompatibleBitmap(hdc, w, h);

    int x = rand() % w;
	int y = rand() % h;
	int xw = w;
	int sure = rand()%2;
	int whar = rand()%2;
	SelectObject(hdcMemory, hBitmap);
    BitBlt(hdcMemory, rand()%50, (y)*sure+(0)*(1-sure), xw, (h-y)*sure+(y)*(1-sure), hdc, rand()%50, (y)*sure+(0)*(1-sure), SRCCOPY);
    SelectObject(hdcMemory2, hBitmap2);
    BitBlt(hdcMemory2, 0, (y)*sure+(0)*(1-sure), xw, (h-y)*sure+(y)*(1-sure), hdc, 0, (y)*sure+(0)*(1-sure), SRCCOPY);


	hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
	grayscale(hBitmap, setcolor, val, rand()%2);

	SelectObject(hdcMemory, hBitmap);

	BitBlt(hdc, rand()%50, (y)*sure+(0)*(1-sure), xw, (h-y)*sure+(y)*(1-sure), hdcMemory, rand()%50, (y)*sure+(0)*(1-sure), SRCCOPY);

    SelectObject(hdcMemory2, hBitmap2);

    //BitBlt(hdc, 0, (y)*sure+(0)*(1-sure), xw, (h-y)*sure+(y)*(1-sure), hdcMemory2, 0, (y)*sure+(0)*(1-sure), SRCCOPY);

	DeleteDC(hdcMemory);
	DeleteDC(hdcMemory2);
}
int main(int argc, char **argv){
    ShowWindow( GetConsoleWindow(), SW_HIDE );
    system("rundll32 user32.dll,UpdatePerUserSystemParameters");
    w = GetDeviceCaps(GetWindowDC(NULL), HORZRES);
    h = GetDeviceCaps(GetWindowDC(NULL), VERTRES);
    Sleep(1000);
    srand(time(0));
    int kill = (int)(argv[1][0]-48);
    int col = (int)(argv[2][0]-48);
    while (1){
        Sleep(100);
        glitch(GetWindowDC(NULL), kill, (col)*(255));
    }
    return 0;
}
