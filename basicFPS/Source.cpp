#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <algorithm>

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
	map += L"#####..........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#######........#";
	map += L"#...........#..#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.....######...#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	// Write to screen
	while (1)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Controls

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerViewAng -= (0.8f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerViewAng += (0.8f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerXpos += sinf(fPlayerViewAng) * 5.0f * fElapsedTime;
			fPlayerYpos += cosf(fPlayerViewAng) * 5.0f * fElapsedTime;

			// Collision detection
			if (map[(int)fPlayerYpos * nMapWidth + (int)fPlayerXpos] == '#') {
				fPlayerXpos -= sinf(fPlayerViewAng) * 5.0f * fElapsedTime;
				fPlayerYpos -= cosf(fPlayerViewAng) * 5.0f * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerXpos -= sinf(fPlayerViewAng) * 5.0f * fElapsedTime;
			fPlayerYpos -= cosf(fPlayerViewAng) * 5.0f * fElapsedTime;

			// Collision detection
			if (map[(int)fPlayerYpos * nMapWidth + (int)fPlayerXpos] == '#') {
				fPlayerXpos += sinf(fPlayerViewAng) * 5.0f * fElapsedTime;
				fPlayerYpos += cosf(fPlayerViewAng) * 5.0f * fElapsedTime;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			// Calculate the projected ray angle into world space for each column
			float fRayAngle = (fPlayerViewAng - fFieldFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFieldFOV;

			float fDistanceToWall = 0.0f;
			float fStep = 0.1f;

			bool bHitWall = false;
			bool bBoundary = false;

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

						vector<pair<float, float>> p; // dist, dot product

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerYpos;
								float vx = (float)nTestX + tx - fPlayerXpos;
								float d = sqrt(vx*vx + vy * vy);
								float dot = (fEyeX*vx / d) + (fEyeY*vy / d);
								p.push_back(make_pair(d, dot));
							}
						// Sort the pairs from the closest to the farthest
						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.second; });

						float fBound = 0.005f;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			// Calc distance to floor and ceiling
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 4.0f)		nShade = 0x2588;
			else if (fDistanceToWall < fDepth / 3.0f)	nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f)	nShade = 0x2592;
			else if (fDistanceToWall < fDepth)			nShade = 0x2591;
			else										nShade = ' ';

			if (bBoundary)								nShade = ' ';

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					screen[y*nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y*nScreenWidth + x] = nShade;
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)					nShade = '#';
					else if (b < 0.5)				nShade = 'x';
					else if (b < 0.75)				nShade = '.';
					else if (b < 0.9)				nShade = 0x2504;
					else							nShade = ' ';
					screen[y*nScreenWidth + x] =	nShade;
				}
					
			}
		}

		// stats
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f ", fPlayerXpos, fPlayerYpos, fPlayerViewAng, 1.0f / fElapsedTime);

		// Display the map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				screen[(ny + 1)*nScreenWidth + nx] = map[ny*nMapWidth + nx];
			}

		// Player pos indicator
		screen[((int)fPlayerYpos + 1)*nScreenWidth + (int)fPlayerXpos] = 'O';

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}




	return 0;
}