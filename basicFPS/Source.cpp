#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <chrono>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;
int nMapHeight = 16;
int nMapWidth = 16;

float fPlayerXpos = 8.0f;
float fPlayerYpos = 8.0f;
float fPlayerViewAng = 0.0f;

float fDepth = 16.0f;
float fFieldFOV = 3.14159f / 4.0f;

int main()
{
	// Creates screen buffer

	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	// Write to screen
	while (1)
	{

		// Controls

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerViewAng -= (0.1f);

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerViewAng += (0.1f);



		for (int x = 0; x < nScreenWidth; x++)
		{
			// Calculate the projected ray angle into world space for each column
			float fRayAngle = (fPlayerViewAng - fFieldFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFieldFOV;

			float fDistanceToWall = 0.0f;
			float fStep = 0.1f;

			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStep;

				int nTestX = (int)(fPlayerXpos + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerYpos + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					// test to see if the ray cell is a wall block
					if (map[nTestY * nMapWidth + nTestX] == '#') // accidently put hashtag in the array smh
					{
						bHitWall = true;
					}
				}
			}

			// Calc distance to floor and ceiling
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					screen[y*nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y*nScreenWidth + x] = '#';
				else
					screen[y*nScreenWidth + x] = ' ';
			}
		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}




	return 0;
}