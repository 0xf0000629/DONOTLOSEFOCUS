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
void cursorspawn(){
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);
    DrawIcon(hdc, rand()%w, rand()%h, ci.hCursor);
}
int main(){
    ShowWindow( GetConsoleWindow(), SW_HIDE );
    srand(time(0));
    hdc = GetWindowDC(NULL);
    w = GetDeviceCaps(hdc, HORZRES);
    h = GetDeviceCaps(hdc, VERTRES);
    while (1){
        Sleep(100);
        cursorspawn();
    }
    return 0;
}
