#include <iostream>
#include <Windows.h>

using namespace std;

int nScreenHeight = 40;
int nScreenWidth = 120;

float playerXpos = 0.0f;
float playerYpos = 0.0f;
float playerViewAng = 0.0f;
float fDepth = 16.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fieldFOV = 3.14159f / 4.0f;

int main()
{
	// Creates screen buffer

	wchar_t *screen = new wchar_t[nScreenHeight*nScreenWidth];
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
		for (int i = 0; i < nScreenWidth; i++) 
		{
			// Calculate the projected ray angle into world space for each column
			float fRayAngle = (playerViewAng - fieldFOV / 2.0f) + ((float)i / (float)nScreenWidth) * fieldFOV;
			float fDistanceToWall = 0;
            bool bHitWall = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth) 
			{
               fDistanceToWall += 0.1f;

			   int nTestX = (int)(playerXpos + fEyeX * fDistanceToWall);
			   int nTestY = (int)(playerYpos + fEyeY * fDistanceToWall);

			   // Test if ray is out of bounds
			   if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
			   {
				   bHitWall = true;
				   fDistanceToWall = fDepth;
			   }
			   else 
			   {
				   // Ray is inbounds so test to see if the ray cell is a wall block
				   if (map[nTestY * nMapWidth + nTestX == '#'])
				   {
					   bHitWall = true;
				   }
			   }
			}

			// Calc distance to floor and ceiling
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;


		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenHeight * nScreenWidth, { 0, 0 }, &dwBytesWritten);
	}

	


	return 0;
}