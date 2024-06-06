//-----------------------------------------------------------------
// Bitmap Object
// C++ Source - Bitmap.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Bitmap.h"

//-----------------------------------------------------------------
// Bitmap Constructor(s)/Destructor
//-----------------------------------------------------------------
Bitmap::Bitmap()
  : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0)
{}

// Create a bitmap from a file
Bitmap::Bitmap(HDC hDC, LPTSTR szFileName)
  : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0), m_hDC(hDC)
{
  Create(szFileName);
}

// Create a bitmap from a resource
Bitmap::Bitmap(HDC hDC, UINT uiResID, HINSTANCE hInstance)
  : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0), m_hDC(hDC)
{
  Create(uiResID, hInstance);
}

// Create a blank bitmap from scratch
Bitmap::Bitmap(HDC hDC, int iWidth, int iHeight, COLORREF crColor)
  : m_hBitmap(NULL), m_iWidth(0), m_iHeight(0), m_hDC(hDC)
{
  Create(iWidth, iHeight, crColor);
}

Bitmap::~Bitmap()
{
  Free();
}

//-----------------------------------------------------------------
// Bitmap Helper Methods
//-----------------------------------------------------------------
void Bitmap::Free()
{
  // Delete the bitmap graphics object
  if (m_hBitmap != NULL)
  {
    DeleteObject(m_hBitmap);
    m_hBitmap = NULL;
  }
}

//-----------------------------------------------------------------
// Bitmap General Methods
//-----------------------------------------------------------------
BOOL Bitmap::Create(LPTSTR szFileName)
{
  // Free any previous bitmap info
  Free();

  // Open the bitmap file
  HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  // Read the bitmap file header
  BITMAPFILEHEADER  bmfHeader;
  DWORD             dwBytesRead;
  BOOL bOK = ReadFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER),
                      &dwBytesRead, NULL);
  if ((!bOK) || (dwBytesRead != sizeof(BITMAPFILEHEADER)) ||
      (bmfHeader.bfType != 0x4D42))
  {
    CloseHandle(hFile);
    return FALSE;
  }

  BITMAPINFO* pBitmapInfo = (BITMAPINFO*)(new BITMAPINFO_256);
  if (pBitmapInfo != NULL)
  {
    // Read the bitmap info header
    bOK = ReadFile(hFile, pBitmapInfo, sizeof(BITMAPINFOHEADER),
                   &dwBytesRead, NULL);
    if ((!bOK) || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
    {
      CloseHandle(hFile);
      Free();
      return FALSE;
    }

    // Store the width and height of the bitmap
    m_iWidth = (int)pBitmapInfo->bmiHeader.biWidth;
    m_iHeight = (int)pBitmapInfo->bmiHeader.biHeight;

    // Skip (forward or backward) to the color info, if necessary
    if (pBitmapInfo->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
      SetFilePointer(hFile, pBitmapInfo->bmiHeader.biSize - sizeof
      (BITMAPINFOHEADER), NULL, FILE_CURRENT);

    // Read the color info
    bOK = ReadFile(hFile, pBitmapInfo->bmiColors,
                   pBitmapInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD), &dwBytesRead,
                   NULL);

    // Get a handle to the bitmap and copy the image bits
    PBYTE pBitmapBits;
    m_hBitmap = CreateDIBSection(m_hDC, pBitmapInfo, DIB_RGB_COLORS,
                                 (PVOID*)&pBitmapBits, NULL, 0);
    if ((m_hBitmap != NULL) && (pBitmapBits != NULL))
    {
      SetFilePointer(hFile, bmfHeader.bfOffBits, NULL, FILE_BEGIN);
      bOK = ReadFile(hFile, pBitmapBits, pBitmapInfo->bmiHeader.biSizeImage,
                     &dwBytesRead, NULL);
      if (bOK)
        return TRUE;
    }
  }

  // Something went wrong, so cleanup everything
  Free();
  return FALSE;
}

BOOL Bitmap::Create(UINT uiResID, HINSTANCE hInstance)
{
  // Free any previous DIB info
  Free();

  // Load the bitmap using LoadImage
  m_hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(uiResID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
  if (m_hBitmap == NULL)
  {
    return FALSE;
  }

  // Get the bitmap dimensions
  BITMAP bitmap;
  if (GetObject(m_hBitmap, sizeof(BITMAP), &bitmap) == 0)
  {
    Free();
    return FALSE;
  }

  m_iWidth = bitmap.bmWidth;
  m_iHeight = bitmap.bmHeight;

  return TRUE;
}

BOOL Bitmap::Create(int iWidth, int iHeight, COLORREF crColor)
{
  // Create a blank bitmap
  m_hBitmap = CreateCompatibleBitmap(m_hDC, iWidth, iHeight);
  if (m_hBitmap == NULL)
    return FALSE;

  // Set the width and height
  m_iWidth = iWidth;
  m_iHeight = iHeight;

  // Create a memory device context to draw on the bitmap
  HDC hMemDC = CreateCompatibleDC(m_hDC);

  // Create a solid brush to fill the bitmap
  HBRUSH hBrush = CreateSolidBrush(crColor);

  // Select the bitmap into the device context
  HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

  // Fill the bitmap with a solid color
  RECT rcBitmap = { 0, 0, m_iWidth, m_iHeight };
  FillRect(hMemDC, &rcBitmap, hBrush);

  // Cleanup
  SelectObject(hMemDC, hOldBitmap);
  DeleteDC(hMemDC);
  DeleteObject(hBrush);

  return TRUE;
}

void Bitmap::Draw(HDC hDC, int x, int y, BOOL bTrans, COLORREF crTransColor)
{
  DrawPart(hDC, x, y, 0, 0, GetWidth(), GetHeight(), bTrans, crTransColor);
}

void Bitmap::DrawPart(HDC hDC, int x, int y, int xPart, int yPart,
                      int wPart, int hPart, BOOL bTrans, COLORREF crTransColor)
{
  if (m_hBitmap != NULL)
  {
    // Create a memory device context for the bitmap
    HDC hMemDC = CreateCompatibleDC(hDC);

    // Select the bitmap into the device context
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

    // Draw the bitmap to the destination device context
    if (bTrans)
      TransparentBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart,
                     wPart, hPart, crTransColor);
    else
      BitBlt(hDC, x, y, wPart, hPart, hMemDC, xPart, yPart, SRCCOPY);

    // Restore and delete the memory device context
    SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
  }
}
