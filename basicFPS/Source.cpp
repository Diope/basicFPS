#include <iostream>
#include <Windows.h>

using namespace std;

int nScreenHeight = 40;
int nScreenWidth = 120;

float playerXpos = 0.0f;
float playerYpos = 0.0f;
float playerViewAng = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

int main()
{
	// Creates screen buffer

	wchar_t *screen = new wchar_t[nScreenHeight*nScreenWidth];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Write to screen
	while (1)
	{
		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenHeight * nScreenWidth, { 0, 0 }, &dwBytesWritten);
	}

	


	return 0;
}