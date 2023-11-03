#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <ctime>

static int g_nScreenIndex;
static HANDLE g_hScreen[2];
int g_numofFPS;
clock_t CurTime, OldTime;
char* FPSTextInfo;

void ScreenInit()
{
    CONSOLE_CURSOR_INFO cci;

    // 화면 버퍼 2개를 만든다.
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 커서를 숨긴다.
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(g_hScreen[0], &cci);
    SetConsoleCursorInfo(g_hScreen[1], &cci);
}

void ScreenFlipping()
{
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = !g_nScreenIndex;
}

void ScreenClear()
{
    COORD Coor = { 0, 0 };
    DWORD dw;
    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 80 * 25, Coor, &dw);
}

void ScreenRelease()
{
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}

void ScreenPrint(int x, int y, char* string)
{
    DWORD dw;
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteFile(g_hScreen[g_nScreenIndex], string, strlen(string), &dw, NULL);
}

void Render()
{
    ScreenClear();

    if (CurTime - OldTime >= 1000) // 출력 코드
    {
        sprintf(FPSTextInfo, "FPS : %d", g_numofFPS);
        OldTime = CurTime;
        g_numofFPS = 0;
    }

    g_numofFPS++;
    ScreenPrint(0, 0, FPSTextInfo);
    ScreenFlipping();
}

void Release()
{
    delete[] FPSTextInfo;
}

int main()
{
    g_numofFPS = 0;
    FPSTextInfo = new char[128];
    memset(FPSTextInfo, '\0', 128);

    ScreenInit();

    OldTime = clock(); // 시간을 측정한다. 1초마다 갱신한다.

    while (1)
    {
        CurTime = clock();
        Render();
    }

    Release();
    ScreenRelease();

    return 0;
}