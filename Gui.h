//-----------------------------------------------------------------
// Gui Object
// C++ Header - Gui.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <queue>
#include <cstdlib>
#include "Sprite.h"
//-----------------------------------------------------------------
// GUI Class
//-----------------------------------------------------------------

//Health Bar Variables
extern int iPercentage;
extern RECT rOutterHealthBar;
extern RECT rInnerHealthBar;
extern HBRUSH hRedBrush;
extern HBRUSH hGreyBrush;

//Timer Print Variables
extern DWORD iTimer;
extern DWORD wTickStart;
extern TCHAR cpTimerText[10];
extern RECT  rTimerTextRect;

enum ElementType
{
	EMPTY = -1,
	Earth = 0,
	Fire = 1,
	Water = 2,
	Air = 3
};

void PaintHealthBar(HWND hWindow,HDC hDC, int maxHealth, int currentHealth);
void PrintTime(HWND hWindow, HDC hDC);
void printScore(HWND hWindow, HDC hDC);

class ElementQueue {
private:
	std::queue<Bitmap*> m_qElementBitmaps;
	std::queue<ElementType> m_qElementTypes;
	Bitmap* m_pElementBitmaps[4];
	const int iQueueSize = 4;

	HWND m_hWindow;
	HDC m_hDC;
	RECT m_rOutterQueue = {816, 200, 976, 481};

public:
	ElementQueue::ElementQueue(HWND hWindow, HDC hDC, Bitmap* earth, Bitmap* fire, Bitmap* water, Bitmap* Air);
	void FillRandom();
	ElementType UseElement();
	void AddRandomElement();
	void AddElement(ElementType type);
	void AddElement(int type);
	void DrawQueue();
};