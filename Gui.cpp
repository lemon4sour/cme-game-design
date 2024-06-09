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
TCHAR cpTimerText[4];
RECT rTimerTextRect = { 672, 0, 768, 32 };

//Level Variables
TCHAR cpLevel[1];
RECT rLevelRect = { 734, 734, 768,768 };
HFONT hFont = CreateFont(
  32,                 // Height of font
  24,                  // Width of font
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

//Obsolete
void PaintHealthBar(HDC hDC, int maxHealth, int currentHealth)
{
  iPercentage = currentHealth * 192 / maxHealth;

  SetRect(&rInnerHealthBar, 800, 24, 800 + iPercentage, 46);

  FillRect(hDC, &rOutterHealthBar, hGreyBrush);
  FillRect(hDC, &rInnerHealthBar, hRedBrush);
}

void PrintTime(HDC hDC)
{
  HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
  
  if (GetTickCount64() - iTickStart > 1000)
  {
    iTickStart = GetTickCount64();
    iTimer++;
  }

  SetBkColor(hDC, TRANSPARENT);
  SetTextColor(hDC, RGB(255, 255, 255));
  wsprintf(cpTimerText, TEXT("%d"), iTimer);
  DrawText(hDC, cpTimerText, -1, &rTimerTextRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
  
  SelectObject(hDC, hOldFont);
}

Inventory::Inventory(HDC hDC, Bitmap* earth,Bitmap* water) :
  m_hDC(hDC), m_pElementBitmaps{earth, water}, m_sElementNumber{ 0 }
{
    m_mapInventory[ElementType::Earth] = 8;
    m_mapInventory[ElementType::Water] = 8;
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
  //FillRect(m_hDC, &rOutterHealthBar, hGreyBrush);
  
  HFONT hOldFont = (HFONT)SelectObject(m_hDC, hFont);

  RECT NumberRect = {332,0,368,32};
  int position = 300;
  for (int i = 0; i < m_mapInventory.size(); i++)
  {
    m_pElementBitmaps[i]->Draw(m_hDC, position, 0, TRUE);
    position += 68;

    SetBkColor(m_hDC, TRANSPARENT);
    SetTextColor(m_hDC, RGB(255, 255, 255));
    wsprintf(m_sElementNumber, TEXT("x%d"), m_mapInventory[static_cast<ElementType>(i)]);
    DrawText(m_hDC, m_sElementNumber, -1, &NumberRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    NumberRect.left += 100;
    NumberRect.right += 100;
  }

  SelectObject(m_hDC, hOldFont);
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