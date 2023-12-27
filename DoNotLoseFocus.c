#define _WIN32_WINNT 0x0500
#include <windows.h>
#include<tchar.h>
#include <stdio.h>
//#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <conio.h>
#include <dwmapi.h>
#include <lmcons.h>
#define l(a,b) (int)(b)*xlen+(int)(a)   //convert from x,y to an array position
#include <ole2.h>
#include <olectl.h>
#include <gdiplus.h>

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#endif




BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)   //some insane code I found that saves an hbitmap to a file
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

int screenCapturePart(LPCSTR fname){        // takes a screenshot and saves it into a file
    RECT cr;
    HWND hwnd = NULL;
    if (hwnd != NULL) GetWindowRect(hwnd, &cr);
    else GetWindowRect(GetDesktopWindow(), &cr);
    HDC hdcSource = GetWindowDC(NULL);


    LONG w = cr.right-cr.left;
    LONG h = cr.bottom-cr.top;

    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, w, h, hdcSource, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);
    SaveHBITMAPToFile(hBitmap, fname);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
}
struct point{
    double x;
    double y;
};

struct particle{  // coords, speed vector, velocity vector, texture characters for states 1 and 2, particle group id, particle state, state switch period (ms)
    double x, y;
    double dx, dy;
    double ddx, ddy;
    char p1;
    char p2;
    short id;
    short state;
    short period;
};

char* int_str(int h){   //integer to char array converter
    int length = snprintf( NULL, 0, "%d", h );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", h );
    return str;
}

struct timeval startTime, nowTime;  // last frame timestamp, current moment

struct timeval laststage, nowstage; // last stage change timestamp, current moment

struct timeval diff; // time difference buffer


const int xlen = 128;   // screen x length
const int ylen = 48;    // screen y length


int swid = 33 + xlen*8; // physical screen x length
int shei = 51 + ylen*12;    // physical screen y length
float fps = 0;
int frame = 0;  // current frame
int sclength = 0;   // score string length
int gpass = 0;  // the player can go into the glitches stages

int leave = 0;  // player wants to exit flag

int cscreen = 0;    // current game screen
int sscreen = 0;    // game screen that needs to be loaded in

HANDLE hIn; // console input handle
HANDLE hOut;    // console output handle
DWORD prev_mode;    //
HWND hwnd;  // console window handle
HDC dc; // console HDC
HDC sdc;    // screen HDC
HMENU hmenu;    // console menu settings

short shake_req = -1;   // shake time length
short shake_req_max = -1;   // shake time length max
short shake_pow = 0;    // shake power

RECT cr;    // console rectangle
RECT scr;   // console rectangle buffer

double dt = 0; // deltatime

char* disp;     // the array that will be displayed on this frame
char* fg;       // current foreground layer
char* fgf;      // future foreground layer
char* bg1;      // current background 1 layer
char* bg1f;     // future background 1 layer
char* bg2;      // current background 2 layer
char* bg2f;     // future background 2 layer
char* pg;       // particle layer

char* ag;       // animation layer
char* agf;      // future animation layer


char* game_over[640];   // game over text texture

char* stagetext[392];   // stage text texture
char* hextext[1280];    // stage number font texture

char* numtext[300];     // score number font texture

char* swingtext[472];
char* simulatortext[792];   // logo textures

char* bye1text[792];
char* bye2text[680];       // goodbye screen textures

char* playbtn[72];       // button textures
char* exitbtn[72];

char* loadingtext[640];     // loading texture


char* entrance[6144];   // entrance gate texture

char* username;     // wow I wonder what that is

struct particle part[1024];     // particle storage array

int consx = 200;
int consy = 200;

float shift0 = 0;   // background 1 shift
float shift1 = 0;   // background 2 shift
float shift2 = 0;   // foreground shift

float shifta = 0;   // animation shift

float speed = 1;    // player speed

float px = 30;  // player x position
float py = 0;   // player y position
float dy = 0;   // player y speed
float ddy = 0.04;   // player y velocity

char pchar = 'v';   // player texture character

int iframes = 0;    // current iframes
int iframemax = 15; // maximum iframes

short pclick = 0;   // player button state
short scclick = 0;  // screenshot button state
short msclick = 0;  // mouse button state
POINT msloc;

int dead = 0;   // figure it out

int res = 0;    //
int skip = 0;   // allow stage skipping when the stage is reached (debug mode)
int start = 0;  // game status (0 - about to load, 1 - playing, -1 - dead or menu screen)
int fgc = 0;    // foreground color
int bgc = 15;   // background color

int stage = 0;  // stage
int stagetimer = 0; // time since last stage
int screen = 0; // current generated screen counter
int nextstage = 0;  // next stage

int score = 0;
char* scorestring;


void killitall(){   // execute on close, kill every payload
    system("taskkill -im screenshottest.exe -f 2>nul 1>nul");
    system("taskkill -im icongen.exe -f 2>nul 1>nul");
    system("taskkill -im distractor.exe -f 2>nul 1>nul");
    system("taskkill -im invoker.exe -f 2>nul 1>nul");
    system("taskkill -im cursorgen.exe -f 2>nul 1>nul");
    /*FILE* fptr;
    fptr = fopen("file.save", "w");
    fputs(scoreboard, fptr);
    fclose(fptr);*/
}

void freeze(){  // disable console mouse interactions
    GetConsoleMode(hIn, &prev_mode);
    SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS);
    return;
}
void unfreeze(){ // enable them back
    SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | (prev_mode));
    return;
}
void Console_RemoveExitButton() // make the exit button unclickable
{
    EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}
void coh(int bg, int fg){   // change the typing color
    SetConsoleTextAttribute(hOut, bg * 16 + fg);
}
void reset(){   // reset the console to the previous position after shaking it
    RECT r;
    GetWindowRect(hwnd, &r);
    if (r.right-r.left != swid || r.bottom-r.top != shei || r.left != cr.left || r.top != cr.top)
    {MoveWindow(hwnd, r.left, r.top, swid, shei, FALSE);
    showcur(0);}
    return;
}
void shake_request(short length, short power){  // accept the console shake request
    GetWindowRect(GetConsoleWindow(), &cr);     // save the current console position to then reset it after the shaking
    shake_req = length;
    shake_req_max = length;
    shake_pow = power;
}
void shake(){   // gets called every frame and randomly moves the window around
    if (shake_req > 0){
        int shake_val = (float)shake_pow*((float)shake_req/(float)shake_req_max);
        MoveWindow(GetConsoleWindow(), cr.left+(rand()%(shake_val*2+1)-shake_val), cr.top+(rand()%(shake_val*2+1)-shake_val), cr.right-cr.left, cr.bottom-cr.top, FALSE);
        shake_req--;
    }
    if (shake_req == 0){
        MoveWindow(GetConsoleWindow(), cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top, FALSE);
        shake_req = -1;
    }
}
void curjump(int x, int y){ // move the cursor
    COORD c = {x,y};
    SetConsoleCursorPosition(hOut,c);
}
void GetWindowPos( int *x, int *y ) {   // get the window position
    RECT rect = { NULL };
    if( GetWindowRect( hwnd, &rect ) ) {
        *x = rect.left;
        *y = rect.top;
    }
}
void showcur(WINBOOL showFlag)
{
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

int p_next_free(){  // get the next free particle array position
    for (int i=0;i<1024;i++){
        if (part[i].id == -1) return i;
    }
    return -1;
}
void p_create(float x, float y, float dx, float dy, float ddx, float ddy, char p1, char p2, short id, short state, short period){
    int h = p_next_free();  // particle creator
    if (h != -1){
        part[h].x = x;
        part[h].y = y;
        part[h].dx = dx;
        part[h].dy = dy;
        part[h].ddx = ddx;
        part[h].ddy = ddy;
        part[h].p1 = p1;
        part[h].p2 = p2;
        part[h].id = id;
        part[h].state = state;
        part[h].period = period;
    }
}
void p_clear(){ // clear out the entire particle array
    for (int i=0;i<1024;i++) part[i].id = -1;
}
void loadanimation(){ // loads up the future animation layer screen from a request
    for (int i=0;i<xlen*ylen;i++) agf[i] = ' ';
    if (sscreen == 0){
        for (int i=0; i<8; i++){
            for (int j=0; j<49; j++){
                agf[l(j+xlen/2-48,i+ylen/2-13)] = swingtext[i*49+j];
            }
        }
        for (int i=0; i<8; i++){
            for (int j=0; j<89; j++){
                agf[l(j+xlen/2-48,i+ylen/2+5)] = simulatortext[i*89+j];
            }
        }
        for (int i=0; i<8; i++){
            for (int j=0; j<9; j++){
                agf[l(j+xlen/2-48,i+ylen/2-4)] = playbtn[i*9+j];
                agf[l(j+xlen/2+2,i+ylen/2-4)] = exitbtn[i*9+j];
            }
        }
    }
    if (sscreen == 1){
        for (int i=0; i<8; i++){
            for (int j=0; j<80; j++){
                agf[l(j+xlen/2-54,i+ylen/2-4)] = loadingtext[i*80+j];
            }
        }
    }
    if (sscreen == 2){
        for (int i=0; i<8; i++){
            for (int j=0; j<99; j++){
                agf[l(j+xlen/2-54,i+ylen/2-9)] = bye1text[i*99+j];
            }
        }
        for (int i=0; i<8; i++){
            for (int j=0; j<85; j++){
                agf[l(j+xlen/2-54,i+ylen/2+1)] = bye2text[i*85+j];
            }
        }
    }
}
void animation(){   // prints out a whole bunch of things onto the animation layer
    for (int i=0;i<xlen*ylen;i++) ag[i] = ' ';
    if (cscreen == 0){                              // if we are on the main menu, print the logo and the buttons
        for (int i=0; i<8; i++){
            for (int j=0; j<59; j++){
                ag[l(j+xlen/2-48,i+ylen/2-13)] = swingtext[i*59+j];
            }
        }
        for (int i=0; i<8; i++){
            for (int j=0; j<99; j++){
                ag[l(j+xlen/2-48,i+ylen/2+5)] = simulatortext[i*99+j];
            }
        }
        for (int i=0; i<8; i++){
            for (int j=0; j<9; j++){
                ag[l(j+xlen/2-48,i+ylen/2-4)] = playbtn[i*9+j];
                ag[l(j+xlen/2+2,i+ylen/2-4)] = exitbtn[i*9+j];
            }
        }
        if (msclick == 1) {                 // I don't know why but I put the mouse click check there too
            msclick = -1;                   // if the mouse button is pressed on one of the buttons load up the thing

            if (msloc.x > xlen/2-48 && msloc.x <= xlen/2+39)
            if (msloc.y >= ylen/2-4 && msloc.y < ylen/2+4)
            {sscreen = 1; loadanimation(); shifta = 1;}

            if (msloc.x > xlen/2+2 && msloc.x <= xlen/2+11)
            if (msloc.y >= ylen/2-4 && msloc.y < ylen/2+4)
            {sscreen = 2; loadanimation(); shifta = 1;}

        }
    }
    if (cscreen == 1){  // if we are playing the game rn
        if (dead == 1){
            for (int i=0; i<8; i++){
                for (int j=0; j<80; j++){
                    ag[l(j+xlen/2-40,i+ylen/2-4)] = game_over[i*80+j]; // if dead print the game over text
                }
            }
        }
        if (stagetimer > 0){    // if the player reached a new stage, print the stage text
            for (int i=0; i<8; i++){
                for (int j=0; j<49; j++){
                    if (stagetext[i*49+j] == '@'){
                    if (frame % 2 == 0)
                        ag[l(j+xlen-61,i+ylen-9)] = 219;
                    else
                        ag[l(j+xlen-61,i+ylen-9)] = 177;}
                }
            }
            for (int i=0; i<8; i++){
                for (int j=0; j<10; j++){
                    if (hextext[i*160+j+stage*10] == '@'){
                        if (frame % 2 == 0)
                            ag[l(j+xlen-10,i+ylen-9)] = 219;
                        else
                            ag[l(j+xlen-10,i+ylen-9)] = 177;
                    }
                }
            }
            stagetimer--;
        }
        free(scorestring);
        sclength = snprintf( NULL, 0, "%d", score );
        scorestring = malloc( sclength + 1 );
        snprintf( scorestring, sclength + 1, "%d", score );

        for (int nu=0;nu<(int)log10(score)+1; nu++){    // print the score
            for (int i=0; i<5; i++){
                for (int j=0; j<5; j++){
                    if (numtext[i*60+j+(scorestring[nu]-48)*6] == '@'){
                        ag[l(j+1+nu*6,i+1)] = 177;
                    }
                }
            }
        }

        for (int i=0; i<5; i++){    // print the iframe counter
            for (int j=0; j<5; j++){
                if (numtext[i*60+j+(iframes%10)*6] == '@'){
                    ag[l(j+1+1*6,ylen-5+i-1)] = 177;
                }
            }
        }
        for (int i=0; i<5; i++){
            for (int j=0; j<5; j++){
                if (numtext[i*60+j+(iframes/10)*6] == '@'){
                    ag[l(j+1+0*6,ylen-5+i-1)] = 177;
                }
            }
        }

        for (int i=0; i<5; i++){
            ag[l(i+1+2*6,ylen-5+(4-i)-1)] = 177;
        }
        for (int i=0; i<5; i++){    //print the max iframes counter too
            for (int j=0; j<5; j++){
                if (numtext[i*60+j+(iframemax%10)*6] == '@'){
                    ag[l(j+1+4*6,ylen-5+i-1)] = 177;
                }
            }
        }
        for (int i=0; i<5; i++){
            for (int j=0; j<5; j++){
                if (numtext[i*60+j+(iframemax/10)*6] == '@'){
                    ag[l(j+1+3*6,ylen-5+i-1)] = 177;
                }
            }
        }
    }
}
void draw_frame(int* fps){ // display all of the layers on top of each other according to their shift values
    int i, j;
    for (i=0;i<xlen*ylen;i++)disp[i] = ' ';

    if (cscreen == 1){
        for (i=0;i<xlen-shift2;i++)
            for (j=0;j<ylen;j++)
                if (bg2[l(i+shift2,j)] != ' ') disp[l(i,j)] = bg2[l(i+shift2,j)];
        for (i=xlen-shift2;i<xlen;i++)
            for (j=0;j<ylen;j++)
                if (bg2f[l(i-xlen+shift2,j)] != ' ') disp[l(i,j)] = bg2f[l(i-xlen+shift2,j)];

        for (i=0;i<xlen-shift1;i++)
            for (j=0;j<ylen;j++)
                if (bg1[l(i+shift1,j)] != ' ') disp[l(i,j)] = bg1[l(i+shift1,j)];
        for (i=xlen-shift1;i<xlen;i++)
            for (j=0;j<ylen;j++)
                if (bg1f[l(i-xlen+shift1,j)] != ' ') disp[l(i,j)] = bg1f[l(i-xlen+shift1,j)];

        for (i=0;i<xlen-shift0;i++)
            for (j=0;j<ylen;j++)
                if (fg[l(i+shift0,j)] != ' ') disp[l(i,j)] = fg[l(i+shift0,j)];
        for (i=xlen-shift0;i<xlen;i++)
            for (j=0;j<ylen;j++)
                if (fgf[l(i-xlen+shift0,j)] != ' ') disp[l(i,j)] = fgf[l(i-xlen+shift0,j)];
    }

    for (i=0;i<xlen*ylen;i++) if (pg[i] != ' ') disp[i] = pg[i];

    for (i=0;i<xlen-shifta;i++)
            for (j=0;j<ylen;j++)
                if (ag[l(i+shifta,j)] != ' ') disp[l(i,j)] = ag[l(i+shifta,j)];
    for (i=xlen-shifta;i<xlen;i++)
        for (j=0;j<ylen;j++)
            if (agf[l(i-xlen+shifta,j)] != ' ') disp[l(i,j)] = agf[l(i-xlen+shifta,j)];

    /*disp[l(0,0)] = *fps/100 + 48;
    disp[l(1,0)] = *fps%100/10 + 48;
    disp[l(2,0)] = *fps%10 + 48;*/

    if (cscreen == 1)
    if (!dead) {        // print the player if they haven't exploded yet
        if (iframes)
            {disp[l(px-1, py)] = pchar; disp[l(px, py)] = '@';}
        else
            {disp[l(px-1, py)] = pchar; disp[l(px, py)] = '#';}
    }


    COORD c = {0,0};
    SetConsoleCursorPosition(hOut,c);       // set the position to 0, 0 and print the entire screen string
    printf("%s", disp);                     // this is the fastest way to print out the whole screen at once
}
void resetfg(){ // put the future layer onto the current layer, clear the future layer, the one is for the foreground
    free(fg);
    fg = fgf;
    fgf = malloc((ylen * xlen) * sizeof(int));
}
void resetbg1(){    // for background 1
    free(bg1);
    bg1 = bg1f;
    bg1f = malloc((ylen * xlen) * sizeof(int));
}
void resetbg2(){    // for background 2
    free(bg2);
    bg2 = bg2f;
    bg2f = malloc((ylen * xlen) * sizeof(int));
}
void resetag(){    // for the animation layer
    free(ag);
    ag = agf;
    agf = malloc((ylen * xlen) * sizeof(int));
}
void geometry(char* m, char p, short spam){ // generate the geometry onto a layer
    int thebigone = 0;
    for (int c=0;c<xlen*ylen;c++){m[c] = ' ';}

    for (int t=0;t<6-(max(stage-4,0)/3);t++){
        short rwx = rand()%7+2;
        short rwy = rand()%7+2;
        short rx = rand()%(xlen-rwx);
        short ry = rand()%(ylen-rwy);
        for (int i=0; i < rwx;i++)
        for (int j=0; j < rwy;j++){
            short offx = rand()%2; short offy = rand()%2;
            if ((i <= offx || i >= rwx-1-offx) || (j <= offy || j >= rwy-1-offy) || spam == 0)
                m[l(rx+i,ry+j)] = p;
            else
                m[l(rx+i,ry+j)] = (sin(j*(stage+1))*cos(i*i)*2+2)+220;
        }
    }
    for (int t=0;t<16-(stage/2);t++){
        short ry = 0;
        if (rand() % ((stage+1)*5) == 0 && thebigone < (4-stage/4))
            {ry = rand()%(ylen/2); thebigone++;}
        else
            ry = rand()%(ylen/2-(15-stage));
        short rx = rand()%(xlen/3);
        short ox = rand()%(xlen-rx);
        short wh = rand()%2;
        if (wh == 0){
            for (int i=ox;i<ox+rx;i++){
                for (int j=0;j<ry;j++){
                    short offx = rand()%3; short offy = rand()%3;
                    if ((i <= ox+offx || i >= ox+rx-1-offx) || (j <= offy || j >= ry-1-offy) || spam == 0)
                        m[l(i,j)] = p;
                    else
                        m[l(i,j)] = (sin(j*(stage+1))*cos(i*i)*2+2)+220;
                }
            }
            /*if (spam && rx>2 && ry>2){
                short ptx1 = rand()%(rx-2)+1;
                short ptx2 = rand()%(rx-1-ptx1)+ptx1;
                short pty1 = rand()%(ry-2)+1;
                short pty2 = rand()%(ry-1-pty1)+pty1;
                for (int i=ptx1+ox;i<ptx2+ox;i++){
                    for (int j=pty1;j<pty2;j++) m[l(i,j)] = rand()%4+220;
                }
            }*/
        }
        else{
            for (int i=ox;i<ox+rx;i++){
                for (int j=ylen-1;j>=ylen-ry;j--){
                    short offx = rand()%5; short offy = rand()%5;
                    if ((i <= ox+offx || i >= ox+rx-1-offx) || (j >= ylen-1-offy || j <= ylen-ry+offy) || spam == 0)
                        m[l(i,j)] = p;
                    else
                        m[l(i,j)] = (sin(j*(stage+1))*cos(i*iframes+1)*2+2)+220;
                }
            }
        }
    }
}

void rods(char* m, char* ml, char p){ // generate the random rods everywhere onto a layer
    int i, j, q;
    for (i=0;i<xlen;i++){
        for (j=0;j<ylen;j++){
            short pt = -1;
            if (i > 0) if(m[l(i,j)] == ' ' && m[l(i-1,j)] == p) pt = 2;


            if (j < ylen-1) if(m[l(i,j)] == ' ' && m[l(i,j+1)] == p) pt = 1;
            if (j > 0) if(m[l(i,j)] == ' ' && m[l(i,j-1)] == p) pt = 3;

            if (pt != -1){
                short gen = rand()%64;
                if (gen == 0){
                    //m[l(i,j)] = 48+pt;
                    if (pt == 2)
                        for (q=i;q<xlen && m[l(q,j)] != p;q++)
                            if (rand()%10) m[l(q,j)] = 196;
                            else m[l(q,j)] = 194;

                    if (pt == 1)
                        for (q=j;q>=0 && m[l(i,q)] != p;q--)
                            m[l(i,q)] = 179;
                    if (pt == 3)
                        for (q=j;q<ylen && m[l(i,q)] != p;q++)
                            m[l(i,q)] = 179;
                }
            }
        }
    }
    for (j=0;j<ylen;j++){
        if(ml[l(xlen-1,j)] == -60 && m[l(0,j)] == ' '){
            for (q=0;q<xlen && m[l(q,j)] != p;q++)
                m[l(q,j)] = 196;
        }
    }
    for (i=0;i<xlen;i++){
        for (j=0;j<ylen;j++){
            short ll=0, rr=0, uu=0, dd=0;
            if (i > 0 && m[l(i-1,j)] == -60) ll = 1;
            if (i < xlen-1 && m[l(i+1,j)] == -60) rr = 1;
            if (j > 0 && m[l(i,j-1)] == -77) uu = 1;
            if (j < ylen-1 && m[l(i,j+1)] == -77) dd = 1;

            if (uu && rr && dd && ll) {m[l(i,j)] = 197; continue;}
            if (uu && ll && rr) m[l(i,j)] = 193;
            if (dd && ll && rr) m[l(i,j)] = 194;
            if (uu && ll && dd) m[l(i,j)] = 180;
            if (uu && rr && dd) m[l(i,j)] = 195;
        }
    }
}
void control(POINT loc){    // button press controller
    if (GetKeyState(VK_UP) < 0 && pclick != -1) pclick = 1;
    if (GetKeyState(VK_UP) >= 0) pclick = 0;

    if (GetKeyState(VK_RSHIFT) < 0 && scclick != -1) scclick = 1;
    if (GetKeyState(VK_RSHIFT) >= 0) scclick = 0;

    if (GetKeyState(VK_LBUTTON) < 0 && msclick != -1) {msclick = 1;}
    if (GetKeyState(VK_LBUTTON) >= 0) msclick = 0;
    msloc = loc;
}
void particles(){   // particle movement processor/iterator
    for (int i=0;i<xlen*ylen;i++) pg[i] = ' ';
    for (int i=0;i<1024;i++){
        if (part[i].id != -1){
            part[i].dx += part[i].ddx;
            part[i].dy += part[i].ddy;
            part[i].x += part[i].dx + part[i].dx * (xlen-shifta)/20*(shifta>1);
            part[i].y += part[i].dy;
            if (part[i].x < 0 || part[i].x >= xlen) part[i].id = -1;
            else
            if (part[i].y < 0 || part[i].y >= ylen) part[i].id = -1;
            else{
                if (part[i].period != -1){
                    if (frame%part[i].period==0) part[i].state = -part[i].state;
                    if (part[i].state == 1) pg[l(part[i].x,part[i].y)] = part[i].p1;
                    else pg[l(part[i].x,part[i].y)] = part[i].p2;
                }
                else pg[l(part[i].x,part[i].y)] = part[i].p1;
            }
        }
    }
    if (!dead && cscreen == 1) p_create(px,py,-speed, 0, 0, 0, '+', '-', 2, 1, 10);
}
void physics(){ // checks of the player is dead, if not - move the player onto the position of the next frame
    if (disp[l(px+1, py)] >= -37 && disp[l(px+1, py)] <= -33){
        if (iframes == 0){
            int nsize = 0;
            char s[256] = "DEATHFRAME_";
            for (int i=0;i<256-11;i++) s[i+11] = '_';
            for (int i=0;i<UNLEN && username[i] != 0;i++) {if(username[i] != 32) s[i+11] = username[i]; else s[i+11] = '+'; nsize++;}
            s[11+nsize+0] = '.';
            s[11+nsize+1] = 'b';
            s[11+nsize+2] = 'm';
            s[11+nsize+3] = 'p';
            s[11+nsize+4] = 0;
            screenCapturePart(s);
            for (int i=0;i<20;i++){
                p_create(px, py, (float)(rand()%61-30)/20, (float)(rand()%61-30)/20, 0, 0.04, '|', '-', 1, 1-2*rand()%2, 3);
            }
            for (int i=0;i<1024;i++){
                if (part[i].id == 2){
                    part[i].dx = -(float)(rand()%31)/20;
                    part[i].dy = (float)(rand()%61-30)/20;
                }
            }
            dead = 1;
            system("taskkill -im screenshottest.exe -f 2>nul 1>nul");
            system("taskkill -im icongen.exe -f 2>nul 1>nul");
            system("taskkill -im distractor.exe -f 2>nul 1>nul");
            system("taskkill -im invoker.exe -f 2>nul 1>nul");
            system("taskkill -im cursorgen.exe -f 2>nul 1>nul");
            PlaySound("src/death.wav", NULL, SND_ASYNC);
            shake_req = 0;
            cr = scr;
            shake_request(60, 20);
            py = ylen/2;
        }
        else {
            iframes--;
            for (int i=0;i<3;i++){
                p_create(px-1, py, (float)(rand()%30-30)/20, (float)(rand()%61-30)/20, 0, 0.04, 177, 178, 1, 1-2*rand()%2, 3);
            }
        }
    }
    if (pclick == 1){
        pclick = -1; ddy = -ddy; dy = dy/2;
        if (ddy > 0) pchar = 'v'; else pchar = '^';
    }
    if (ddy < 0){
        if (py > 0 && disp[l(px, py-1)] != -37) {
                dy += ddy;
        }
        else{
            dy = 0;
            if (screen > 0 || shift0 > 48)
            {score += 10; if (score%250==0 && iframes < iframemax) iframes++;}
        }
    }
    if (ddy > 0){
        if (py < ylen-1 && disp[l(px, py+1)] != -37) {
                dy += ddy;
        }
        else{
            dy = 0;
            if (screen > 0 || shift0 > 48)
            {score += 10;
            if (stage >= 7) {if (score%250==0 && iframes < iframemax) iframes++;}
            else {if (score%125==0 && iframes < iframemax) iframes++;}}
        }
    }
    py += dy;
    if (py >= ylen/2) while (disp[l(px, py)] == -37 || py >= ylen) {py--; dy = 0;}
    if (py < ylen/2) while (disp[l(px, py)] == -37 || py < 0) {py++; dy = 0;}
}
void loadfg(){  // this clears out the whole foreground layer before everything else happens to it
    for (int i=0;i<xlen*ylen;i++) fg[i] = ' ';
}
void read_texture(FILE **foil, char **tex, int wt, int ht){ // function that reads textures
    char line[480];

    int i = 0, j = 0, l = 0;
    for (i=0;i<wt*ht;i++) *(tex+i) = '-';

    for (l = 0; l < ht; l++){
        fgets(line, 480, *foil);
        for (i=0;i<wt;i++)
            if (j < wt*ht && (line[i] == 32 ||
                              line[i] == '@' ||
                              line[i] == '.' ||
                              line[i] == '!' ||
                              line[i] == ':' ||
                              line[i] == '*'
                              )) {
                    *(tex+j) = line[i]; j++;
            }
    }
}
void btnretext(char **btn, int len){    // converts characters to other characters in the button textures
    for (int i=0;i<len;i++) {
        switch ((int)btn[i]){
            case 64: btn[i] = 219; break;
            case 33: btn[i] = 221; break;
            case 58: btn[i] = 222; break;
            case 46: btn[i] = 220; break;
            case 42: btn[i] = 223; break;
            default: btn[i] = ' '; break;
        }
    }
}
void load_textures(){   // loads the textures one by one in order from font.dat
    FILE *fptr;
    fptr = fopen("src/font.dat", "r");

    read_texture(&fptr, &game_over, 80, 8);
    for (int i=0;i<640;i++) {if(game_over[i] == '@') game_over[i] = 177;}
    read_texture(&fptr, entrance, 128, 48);
    for (int i=0;i<6144;i++) if(entrance[i] == '.') entrance[i] = 219; else entrance[i] = ' ';

    read_texture(&fptr, stagetext, 49, 8);
    read_texture(&fptr, hextext, 160, 8);
    read_texture(&fptr, numtext, 60, 5);
    read_texture(&fptr, swingtext, 59, 8);
    for (int i=0;i<472;i++) if(swingtext[i] == '@') swingtext[i] = 219; else swingtext[i] = ' ';
    read_texture(&fptr, simulatortext, 99, 8);
    for (int i=0;i<792;i++) if(simulatortext[i] == '@') simulatortext[i] = 219; else simulatortext[i] = ' ';

    read_texture(&fptr, playbtn, 9, 8);
    read_texture(&fptr, exitbtn, 9, 8);
    btnretext(playbtn,72);
    btnretext(exitbtn,72);

    read_texture(&fptr, loadingtext, 80, 8);
    btnretext(loadingtext,640);

    read_texture(&fptr, bye1text, 99, 8);
    btnretext(bye1text,792);
    read_texture(&fptr, bye2text, 85, 8);
    btnretext(bye2text,680);

    fclose(fptr);
}
void menu_loop(){   // menu loop, same as game loop, but no game physics, only the particles
    SetConsoleTitle("DO NOT LOSE FOCUS");
    POINT cd={0,0};
    POINT lscd={0,0};
    PlaySound("src/menumusic.wav", NULL, SND_LOOP | SND_ASYNC);
    dt = 0;
    gettimeofday(&startTime, NULL);
    //SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
    coh(bgc, fgc);
    while (1){
        fps = 1000.0/(dt);
        //cout << fps << endl;
        if (dt != 0){
            //PrintWindow(hwnd, dc, PW_CLIENTONLY);
            GetWindowPos(&consx, &consy);
            GetCursorPos(&cd);
            cd.x = (cd.x-consx-7)/8;
            cd.y = (cd.y-consy-31)/12;
            draw_frame(&fps);
            control(cd);
            gettimeofday(&nowTime, NULL);
            if (fps <= 60){
                startTime = nowTime;
                frame++;
                if (!(rand()%2)) p_create(xlen-1, rand()%ylen, (float)(-rand()%30)/10-1, 0, 0, 0, '*', '*', 0, 1, -1);
                animation();
                if (shifta > 0) {shifta+=(xlen-shifta)/10;}
                if (shifta+0.01 >= xlen) {
                    shifta = 0;
                    cscreen = sscreen;
                    resetag();
                    if (cscreen == 1) return;
                    if (cscreen == 2) {leave = 1; return;}
                }
                particles();
                shake();
                coh(bgc, fgc);
            }

            //if (GetKeyState(VK_RSHIFT) < 0) break;
            lscd = cd;
        }
        gettimeofday(&nowTime, NULL);
        diff.tv_sec = nowTime.tv_sec - startTime.tv_sec;
        diff.tv_usec = nowTime.tv_usec - startTime.tv_usec;
        if(diff.tv_usec<0) { diff.tv_sec--; diff.tv_usec+=1000000; }
        dt = diff.tv_sec*1000+diff.tv_usec/1000;
    }
}
void game_loop(){   // oh boy here we go boys and girls and non binary folks
    POINT cd={0,0};
    POINT lscd={0,0};
    for (int i=0;i<xlen*ylen;i++)ag[i] = ' ';
    dt = 0;
    gettimeofday(&startTime, NULL);
    gettimeofday(&laststage, NULL);
    shift0 = 0;
    shift1 = 0;
    shift2 = 0;
    cscreen = 1;
    //SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
    coh(bgc, fgc);
    p_clear();
    while (1){
        if (dead && start != -1) {start = -1;}
        if (start == 0){    // if the game is starting reset EVERYTHING, I'm sure you'll be able to tell what everything is
            res = 0;
            frame = 0;
            skip = 0;
            px = 30;
            py = ylen/2;
            dy = 0;
            ddy = 0.04;
            speed = 1;
            screen = 0;
            stage = 0;
            bgc = 15;
            fgc = 0;
            score = 0;
            iframes = 0;
            iframemax = 15;
            nextstage = 0;
            gettimeofday(&laststage, NULL);
            gettimeofday(&nowstage, NULL);
            PlaySound("src/losingfocusv1.wav", NULL, SND_LOOP | SND_ASYNC); // play my cool song


            shift0 = 0;
            shift1 = 0;
            shift2 = 0;
            loadfg();
            geometry(bg1f, 178, 0); // generate the geometry for the future background layer screen
            geometry(bg2f, 176, 0); // same thing but for the back background layer

            geometry(fgf, 219, 1); rods(fgf, fg, 219);  // geometry and rods for the foreground
            resetbg1(); geometry(bg1f, 178, 0); rods(bg1f, bg1, 178); // jokes on you that geometry generation was for the current layer
            resetbg2(); geometry(bg2f, 176, 0); rods(bg2f, bg2, 176); // now this one is for the future layer
            for (int i=0;i<xlen*ylen;i++) fg[i] = entrance[i]; // put the entrance down
            start = 1; // start game
        }
        if (start == -1 && GetKeyState(VK_SPACE) < 0) {start = 0; dead = 0; p_clear();} // if space pressed and player dead start loading game
        fps = 1000.0/(dt);
        //cout << fps << endl;
        if (dt != 0){
            //PrintWindow(hwnd, dc, PW_CLIENTONLY);
            GetWindowPos(&consx, &consy);
            GetCursorPos(&cd);
            cd.x = (cd.x-consx-7)/8;
            cd.y = (cd.y-consy-31)/12;
            draw_frame(&fps);
            control(cd);
            if (GetKeyState(VK_ESCAPE) < 0) { // if escape then go back to the menu and kill all payloads
                    cscreen = 0;
                    start = 0;
                    system("taskkill -im screenshottest.exe -f 2>nul 1>nul");
                    system("taskkill -im icongen.exe -f 2>nul 1>nul");
                    system("taskkill -im distractor.exe -f 2>nul 1>nul");
                    system("taskkill -im invoker.exe -f 2>nul 1>nul");
                    system("taskkill -im cursorgen.exe -f 2>nul 1>nul");
                    p_clear();
                    break;
            }
            gettimeofday(&nowTime, NULL);
            gettimeofday(&nowstage, NULL);
            diff.tv_sec = nowstage.tv_sec - laststage.tv_sec;
            diff.tv_usec = nowstage.tv_usec - laststage.tv_usec;
            if(diff.tv_usec<0) { diff.tv_sec--; diff.tv_usec+=1000000; }
            //printf("%d \n", diff.tv_sec*1000+diff.tv_usec/1000);
            if (diff.tv_sec*1000+diff.tv_usec/1000>= 20000*(stage+1)) {nextstage = 1;}  // this changes the stages synchronously with the song
            if (stage == 14 && !dead)shake_request(5, 1);
            if (stage == 15 && !dead)shake_request(5, 2);
            if (fps <= 60){
                startTime = nowTime;
                frame++;
                animation();
                particles();
                shake();
                coh(bgc, fgc);
                if (start != -1){
                    physics();
                    if (skip > 0) skip--; else{ // shift all of the layers to the left with different speeds to create a parallax effect
                        shift0+=speed;
                        shift1+=speed/2;
                        shift2+=speed/3;
                        speed+=0.0001;
                    }
                    if (!(rand()%10)) p_create(xlen-1, rand()%ylen, -speed*(rand()%3+2), 0, 0, 0, '-', '*', 0, 1, rand()%10+1);
                    if (shift0 >= xlen) {shift0 -= xlen; resetfg();
                    geometry(fgf, 219, 1); rods(fgf, fg, 219); screen++;}
                    if (nextstage) {
                        nextstage = 0;
                        //shake_req = 0;
                        stage++;
                        system("taskkill -im screenshottest.exe -f 2>nul 1>nul");

                        if (stage == 16 && !gpass){     // the player just reached stage 16 and they don't have the glitchpass?
                            ShowWindow( GetConsoleWindow(), SW_HIDE );     // then CONGRATULAIONS YOU WON etc etc
                            system("taskkill -im icongen.exe -f 2>nul 1>nul");
                            system("taskkill -im distractor.exe -f 2>nul 1>nul");
                            system("taskkill -im invoker.exe -f 2>nul 1>nul");
                            system("taskkill -im cursorgen.exe -f 2>nul 1>nul");
                            int nsize = 0;
                            char s[256] = "SUCCESSFRAME_";
                            for (int i=0;i<256-13;i++) s[i+13] = '_';
                            for (int i=0;i<UNLEN && username[i] != 0;i++) {if(username[i] != 32) s[i+13] = username[i]; else s[i+13] = '+'; nsize++;}
                            s[13+nsize] = '_';
                            for (int i=0;i<sclength;i++) s[i+nsize+14] = scorestring[i];

                            s[13+nsize+1+sclength+0] = '.';
                            s[13+nsize+1+sclength+1] = 'b';
                            s[13+nsize+1+sclength+2] = 'm';
                            s[13+nsize+1+sclength+3] = 'p';
                            s[13+nsize+1+sclength+4] = 0;
                            screenCapturePart(s);
                            FILE* fptr;
                            fptr = fopen(".glitchpass", "w");
                            fputs("Thanks for playing!\n\nGame made by X Future.\nSpecial thanks to jfero and tenler for helping me playtest.\n\n\n\n\nP.S. this file grants access to stages beyond F. Have fun!", fptr);
                            fclose(fptr);
                            Sleep(2000);
                            system("start notepad .glitchpass");
                            PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
                        }
                        switch ((fgc+1)%16){    // run the color break payload depending on what stage we are on
                            case 0: system("rundll32 user32.dll,UpdatePerUserSystemParameters"); break;
                            case 1: system("start screenshottest.exe 6 0"); break;
                            case 2: system("start screenshottest.exe 5 0"); break;
                            case 3: system("start screenshottest.exe 4 0"); break;
                            case 4: system("start screenshottest.exe 3 0"); break;
                            case 5: system("start screenshottest.exe 2 0"); break;
                            case 6: system("start screenshottest.exe 1 0"); break;
                            case 7: system("start screenshottest.exe 7 0"); break;
                            case 8: system("rundll32 user32.dll,UpdatePerUserSystemParameters"); break;
                            case 9: system("start screenshottest.exe 1 1"); break;
                            case 10: system("start screenshottest.exe 2 1"); break;
                            case 11: system("start screenshottest.exe 3 1"); break;
                            case 12: system("start screenshottest.exe 4 1"); break;
                            case 13: system("start screenshottest.exe 5 1"); break;
                            case 14: system("start screenshottest.exe 6 1"); break;
                            case 15: system("start screenshottest.exe 7 1"); break;
                            default: break;
                        }
                        switch (stage%16){  // run other payloads depending on what stage we are on
                            case 2: system("start cursorgen.exe"); break;
                            case 4: system("start icongen.exe"); iframemax += 15; break;
                            case 8: system("start distractor.exe"); iframemax += 15; break;
                            case 12: system("start invoker.exe"); iframemax += 15; break;
                            default: break;
                        }
                        iframes = min(iframemax, iframes+10);
                        stagetimer = 30; score+=2500;
                        if (stage < 8)
                            fgc = stage;
                        else
                            {fgc = stage; bgc = stage-8;}
                    }
                    if (shift1 >= xlen) {shift1 -= xlen; resetbg1();
                    geometry(bg1f, 178, 0); rods(bg1f, bg1, 178);}
                    if (shift2 >= xlen) {shift2 -= xlen; resetbg2();
                    geometry(bg2f, 176, 0); rods(bg2f, bg2, 176);}
                }
            }

            //if (GetKeyState(VK_RSHIFT) < 0) break;
            lscd = cd;
        }
        gettimeofday(&nowTime, NULL);
        diff.tv_sec = nowTime.tv_sec - startTime.tv_sec;
        diff.tv_usec = nowTime.tv_usec - startTime.tv_usec;
        if(diff.tv_usec<0) { diff.tv_sec--; diff.tv_usec+=1000000; }
        dt = diff.tv_sec*1000+diff.tv_usec/1000;
    }
}
int main()
{
    /*ShowWindow( GetConsoleWindow(), SW_HIDE );
    Sleep(2000);
    ShowWindow( GetConsoleWindow(), SW_RESTORE );*/
    if (access(".glitchpass", F_OK) == 0){  // check if the glitchpass file is present
        gpass = 1;
    }
    if (access(".howtoplay", F_OK) != 0){   // check if the howtoplay file is present (aka if the player has seen the tutorial)
        FILE* fptr;
        fptr = fopen(".howtoplay", "w");
        fputs("Hi!\nBefore you start playing, I think I should make you aware of the rules!\n\nControls: press the Up arrow to change gravity.\nSlide on the floor and ceiling to gain points and iFrames.\n\niFrames allow you to break through blocks, but they go away when you do.\n250 points convert to 1 iFrame on the first 8 stages, then the cost becomes 125 points.\nEntering stages gives you 10 iFrames and sometimes increases the maximum you can have.\nYour score is on the top left, your iFrame count is on the bottom left.\nStages show up on the bottom right.\n\nGood luck!\nAnd most importantly,\nDO NOT LOSE FOCUS!\n\n/ X Future", fptr);
        fclose(fptr);
        Sleep(2000);
        system("start notepad .howtoplay");
    }
    int wiper = atexit(killitall);
    CONSOLE_FONT_INFOEX cf = {0};
    cf.cbSize = sizeof cf;
    cf.dwFontSize.X = 8;
    cf.dwFontSize.Y = 12;
    OSVERSIONINFOEXW osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	if (RtlGetVersion(&osv) == 0)                           // windows 11 can honestly go fuck itself dude
	{
		if ((int)osv.dwBuildNumber < 22000){                // all this does is changes the font and console screen size if the system is windows 11
            wcscpy_s(cf.FaceName, 8, L"Terminal");          // because Microsoft replaced their functional terminal with a very, very slow and goofy one
		}
		else wcscpy_s(cf.FaceName, 8, L"Proggy Square");
	}
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 0, &cf);

    username = malloc((UNLEN+1) * sizeof(int));
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);

    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hwnd = GetConsoleWindow();
    dc = GetWindowDC(hwnd);
    sdc = GetWindowDC(NULL);

    RECT r;
    GetWindowRect(hwnd, &r);

    srand(time(0));

    /*CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    xlen = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ylen = csbi.srWindow.Bottom - csbi.srWindow.Top;*/

    disp = malloc((ylen * xlen) * sizeof(int));

    fg = malloc((ylen * xlen) * sizeof(int));
    fgf = malloc((ylen * xlen) * sizeof(int));

    bg1 = malloc((ylen * xlen) * sizeof(int));
    bg1f = malloc((ylen * xlen) * sizeof(int));

    bg2 = malloc((ylen * xlen) * sizeof(int));
    bg2f = malloc((ylen * xlen) * sizeof(int));

    pg = malloc((ylen * xlen) * sizeof(int));

    ag = malloc((ylen * xlen) * sizeof(int));
    agf = malloc((ylen * xlen) * sizeof(int));  // allocate space for all of the arrays

    p_clear();  // remove the particles
    animation();    // load up the animation screen

    load_textures();    // load up the textures
    //Sleep(10000);


    hmenu = GetSystemMenu(GetConsoleWindow(), FALSE);
    reset();
    freeze();
    while (1){
        menu_loop();
        if (!leave)
        game_loop();
        else break;
    }
    unfreeze();
    PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
    return 0;
}


