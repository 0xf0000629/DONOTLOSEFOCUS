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
const int w = 1920;
const int h = 1080;
HWND get_win_hw(char* naem, int len){
    for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
    {

        if (!IsWindowVisible(hwnd))
            continue;

        int length = GetWindowTextLength(hwnd);
        if (length == 0)
            continue;

        char* title = (char*)malloc((length+1) * sizeof(char));
        GetWindowText(hwnd, title, length+1);
        int c = 0;
        for (int i=0;i<len;i++){
            if (naem[i] != title[i]){
                c = 1;
                break;
            }
        }
        if (c == 0){
            return hwnd;
        }
        //cout << "HWND: " << hwnd << " Title: " << title << std::endl;

    }
    return NULL;
}
int main(){
    ShowWindow( GetConsoleWindow(), SW_HIDE );
    srand(time(0));
    Sleep(3000);
    HWND hwndc = FindWindow(NULL, "DO NOT LOSE FOCUS");
    while (1){
        switch (rand()%10){
            case 0: system("start calc"); break;
            case 1: system("start notepad"); break;
            case 2: system("start write"); break;
            case 3: system("start explorer"); break;
            case 4: system("rundll32 url.dll,FileProtocolHandler https://google.com/search?q=how+to+be+better+at+videogames"); break;
            case 5: system("rundll32 url.dll,FileProtocolHandler https://google.com/search?q=niko+oneshot"); break;
            case 6: system("rundll32 url.dll,FileProtocolHandler https://wikipedia.org/wiki/Yume_Nikki"); break;
            case 7: system("rundll32 url.dll,FileProtocolHandler https://rainworld.miraheze.org/wiki/Rain_World_Wiki"); break;
            case 8: system("rundll32 url.dll,FileProtocolHandler https://wikipedia.org/wiki/Special:Random"); break;
            case 9: system("rundll32 url.dll,FileProtocolHandler https://steamcharts.com/app/322170"); break;

        }
        SetWindowPos(hwndc, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        Sleep(5000);
    }
    return 0;
}
