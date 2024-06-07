#include "Gui.h"

//Health Bar Variables
int iPercentage;
RECT rOutterHealthBar = { 796, 20, 996, 50 };
RECT rInnerHealthBar;
HBRUSH hRedBrush = CreateSolidBrush(RGB(153, 3, 3));
HBRUSH hGreyBrush = CreateSolidBrush(RGB(113, 112, 110));

//Timer Print Variables
DWORD iTimer = 0;
DWORD iTickStart = GetTickCount64();
TCHAR cpTimerText[10];
RECT rTimerTextRect = { 800, 50, 900, 70 };

//Level Variables
TCHAR cpLevel[2];
RECT rLevelRect = { 816, 500, 976, 660 };
HFONT hFont = CreateFont(
  128,                 // Height of font
  64,                  // Width of font
  0,                  // Angle of escapement
  0,                  // Orientation angle
  FW_NORMAL,          // Font weight
  FALSE,              // Italic attribute option
  FALSE,              // Underline attribute option
  FALSE,              // Strikeout attribute option
  DEFAULT_CHARSET,    // Character set identifier
  OUT_DEFAULT_PRECIS, // Output precision
  CLIP_DEFAULT_PRECIS,// Clipping precision
  DEFAULT_QUALITY,    // Output quality
  DEFAULT_PITCH | FF_SWISS, // Pitch and family
  TEXT("Arial"));       // Font name

void PrintLevel(HDC hDC, int currentLevel)
{
  HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

  SetTextColor(hDC, RGB(255, 255, 255));
  SetBkMode(hDC, TRANSPARENT);

  wsprintf(cpLevel, TEXT("%d"), currentLevel);
  DrawText(hDC, cpLevel, -1, &rLevelRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  SelectObject(hDC, hOldFont);

}

void PaintHealthBar(HDC hDC, int maxHealth, int currentHealth)
{
  iPercentage = currentHealth * 192 / maxHealth;

  SetRect(&rInnerHealthBar, 800, 24, 800 + iPercentage, 46);

  FillRect(hDC, &rOutterHealthBar, hGreyBrush);
  FillRect(hDC, &rInnerHealthBar, hRedBrush);
}

void PrintTime(HDC hDC)
{
  if (GetTickCount64() - iTickStart > 1000)
  {
    iTickStart = GetTickCount64();
    iTimer++;
  }

  SetBkColor(hDC, TRANSPARENT);
  SetTextColor(hDC, RGB(255, 255, 255));
  wsprintf(cpTimerText, TEXT("Timer: %d"), iTimer);
  DrawText(hDC, cpTimerText, -1, &rTimerTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

Inventory::Inventory(HDC hDC, Bitmap* earth, Bitmap* fire, Bitmap* water, Bitmap* air, Bitmap* pointer) :
  m_hDC(hDC), m_pElementBitmaps{ earth, fire,water, air }, m_pPointBitmap(pointer), m_sElementNumber{ 0 }
{

    m_mapInventory[ElementType::Earth] = 0;
    m_mapInventory[ElementType::Fire] = 0;
    m_mapInventory[ElementType::Water] = 0;
    m_mapInventory[ElementType::Air] = 0;
}

//void Inventory::FillRandom() {
//    for (int i = 0; i < iQueueSize; i++) {
//        AddRandomElement();
//    }
//}

//void Inventory::AddRandomElement() {
//    if (m_qElementBitmaps.size() >= iQueueSize)
//        return;
//
//    int randomElement = rand() % 4;
//    m_qElementBitmaps.push(m_pElementBitmaps[randomElement]);
//    m_qElementTypes.push(static_cast<ElementType>(randomElement));
//}

void Inventory::AddElement(ElementType type)
{
  if (m_mapInventory[type] <= 10)
    m_mapInventory[type]++;
}

void Inventory::AddElement(int type)
{
  AddElement(static_cast<ElementType>(type));
}

void Inventory::Draw()
{
  FillRect(m_hDC, &m_rOutterQueue, hGreyBrush);
  m_pPointBitmap->Draw(m_hDC, 900, 205 + (m_iSelect * 69), TRUE);

  RECT NumberRect = { 880, 250, 900, 270 };
  int position = 205;


  for (int i = 0; i < m_mapInventory.size(); i++)
  {
    m_pElementBitmaps[i]->Draw(m_hDC, 821, position, TRUE);
    position += 69;

    SetBkColor(m_hDC, RGB(113, 112, 110));
    SetTextColor(m_hDC, RGB(255, 255, 255));
    wsprintf(m_sElementNumber, TEXT("%d"), m_mapInventory[static_cast<ElementType>(i)]);
    DrawText(m_hDC, m_sElementNumber, -1, &NumberRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    NumberRect.top += 69;
    NumberRect.bottom += 69;
  }

}

bool Inventory::UseElement(ElementType type)
{

  if (m_mapInventory[type] <= 0)
    return false;

  m_mapInventory[type]--;

  return true;
}

bool Inventory::UseElement(int type)
{
  return UseElement(static_cast<ElementType>(type));
}