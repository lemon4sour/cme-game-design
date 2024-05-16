//-----------------------------------------------------------------
// Level Object
// C++ Header - Level.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include files
//-----------------------------------------------------------------
#include "Bitmap.h"
#include <vector>
#include <windows.h>

//-----------------------------------------------------------------
// Tile Class
//-----------------------------------------------------------------
class Tile
{
private:
  Bitmap* m_bmpBitmap;
  bool m_bCollidable;
  int m_iSize;

public:
  // Constructor/Destructor
  Tile(Bitmap* bmpTile, BOOL bCollidable = true);
  virtual ~Tile();

  // General Methods
  void Draw(HDC hDC, int iPosX, int iPosY);
  BOOL CanCollide();

};

//-----------------------------------------------------------------
// Level Class
//-----------------------------------------------------------------
class Level
{
protected:
  // Member variables
  std::vector<std::vector<int>> m_layout;
  int   m_iTileSize;
  int   m_iLevelNumber;

  Tile* m_tileMap[256];

  int m_iWidth;
  int m_iHeight;

public:
  // Constructor/Destructor
  Level(std::vector<std::vector<int>> layout, int iTileSize, int iLevelNumber);
  virtual ~Level();

  // General Methods
  void MapTile(int iValue, Bitmap* bmpTile, BOOL bCollision = true);
  void Draw(HDC hDC);
  BOOL CheckCollision(float f_xPos, float f_yPos);
};


