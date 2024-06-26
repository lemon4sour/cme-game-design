//-----------------------------------------------------------------
// Bitmap Object
// C++ Header - Bitmap.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>

//-----------------------------------------------------------------
// Custom Data Types
//-----------------------------------------------------------------
struct BITMAPINFO_256
{
  BITMAPINFOHEADER  bmiHeader;
  RGBQUAD           bmiColors[256];
};

//-----------------------------------------------------------------
// Bitmap Class
//-----------------------------------------------------------------
class Bitmap
{
protected:
  // Member Variables
  HBITMAP m_hBitmap;
  int     m_iWidth, m_iHeight;
  HDC m_hDC;

  // Helper Methods
  void Free();

public:
  // Constructor(s)/Destructor
  Bitmap();
  Bitmap(HDC hDC, LPTSTR szFileName);
  Bitmap(HDC hDC, UINT uiResID, HINSTANCE hInstance);
  Bitmap(HDC hDC, int iWidth, int iHeight, COLORREF crColor = RGB(0, 0, 0));
  virtual ~Bitmap();

  // General Methods
  BOOL Create(LPTSTR szFileName);
  BOOL Create(UINT uiResID, HINSTANCE hInstance);
  BOOL Create(int iWidth, int iHeight, COLORREF crColor);
  void Draw(HDC hDC, int x, int y, BOOL bTrans = FALSE,
            COLORREF crTransColor = RGB(255, 0, 255));
  void DrawPart(HDC hDC, int x, int y, int xPart, int yPart,
                int wPart, int hPart, BOOL bTrans = FALSE,
                COLORREF crTransColor = RGB(255, 0, 255));
  int  GetWidth() { return m_iWidth; };
  int  GetHeight() { return m_iHeight; };
};
