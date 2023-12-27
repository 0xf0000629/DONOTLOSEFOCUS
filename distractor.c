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
int w = 1920;
int h = 1080;
HDC hdc;
void drawpic(char* name, int wi, int hi, DWORD huh){
    HDC hdcmem = CreateCompatibleDC(NULL);
	HBITMAP cross = (HBITMAP)LoadImage(NULL, _T(name) ,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	SelectObject(hdcmem, cross);
	BitBlt(hdc,rand()%w,rand()%h,wi,hi,hdcmem,0,0,huh);
}
int main(){
    ShowWindow( GetConsoleWindow(), SW_HIDE );
    srand(time(0));
    hdc = GetWindowDC(NULL);
    w = GetDeviceCaps(hdc, HORZRES);
    h = GetDeviceCaps(hdc, VERTRES);
    Sleep(3000);
    while (1){
        switch (rand()%6){
            case 0: drawpic("src/image1.bmp", 316, 26, SRCAND); break;
            case 1: drawpic("src/image2.bmp", 316, 26, SRCPAINT); break;
            case 2: drawpic("src/image3.bmp", 274, 26, SRCINVERT); break;
            case 3: drawpic("src/image4.bmp", 342, 32, SRCAND); break;
            case 4: drawpic("src/image5.bmp", 342, 32, SRCPAINT); break;
            case 5: drawpic("src/image6.bmp", 39, 24, SRCPAINT); break;
        }
        Sleep(100);
    }
    return 0;
}
