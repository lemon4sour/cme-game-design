//-----------------------------------------------------------------
// Gui Object
// C++ Header - Gui.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <map>
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
extern TCHAR cpTimerText[4];
extern RECT  rTimerTextRect;

//Level Variables
extern TCHAR cpLevel[1];
extern RECT rLevelRect;
extern HFONT hFont;

enum ElementType
{
  EMPTY = -1,
  Earth = 0,
  Water = 1,
  Fire = 2,
  Air = 3
};


void PaintHealthBar(HDC hDC, int maxHealth, int currentHealth, POINT position);
void PrintTime(HDC hDC);
void printScore(HDC hDC);
void PrintLevel(HDC hDC, int currentLevel);
void PrintLifes(HDC hDC, int currentLifeNumber, Bitmap* playerBitmap);

class Inventory
{
private:
  std::map<ElementType, UINT8> m_mapInventory;
  Bitmap* m_pElementBitmaps[2];
  Bitmap* m_pPointBitmap;
  const UINT8 m_i8MaxElementNumber = 9;
  TCHAR m_sElementNumber[2];
  int m_iSelect = 0;
  RECT m_rOutterQueue = { 300, 0, 436, 32 };

public:
  Inventory(HDC hDC, Bitmap* earth, Bitmap* water);

  void FillRandom();
  bool UseElement(ElementType type);
  bool UseElement(int type);
  void AddRandomElement();
  void AddElement(ElementType type);
  void AddElement(int type);
  void Draw(HDC hDC);
  void SetISelect(int direction) { m_iSelect = direction; }
  int GetISelect() { return m_iSelect; }
};