//-----------------------------------------------------------------
// Level Object
// C++ Header - Level.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include files
//-----------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <ctime>
#include "Bitmap.h"

//-----------------------------------------------------------------
// Tile Class
//-----------------------------------------------------------------
class Tile
{
private:
  Bitmap* m_bmpBitmap;
  bool m_bCollidable;
  int m_iSize;
  bool m_bMeltable;

public:
  // Constructor/Destructor
  Tile(Bitmap* bmpTile);
  void Tile::SetMeltable() { m_bMeltable = true; };
  void Tile::SetCollidable() { m_bCollidable = true; };
  virtual ~Tile();

  // General Methods
  void Draw(HDC hDC, int iPosX, int iPosY);
  BOOL CanCollide() { return m_bCollidable; };
  BOOL CanMelt() { return m_bMeltable; };
};

//-----------------------------------------------------------------
// Level Class
//-----------------------------------------------------------------
class Level
{
protected:
  // Member variables
  int   m_iTileSize;
  int   m_iLevelNumber;

  Tile* m_tileMap[256];

  int m_iWidth;
  int m_iHeight;

private:
	int m_iCurrentLevel;

public:
  std::vector<std::vector<int>> m_layout;

  // Constructor/Destructor
  Level(int iTileSize, int iLevelNumber, int icurrentLevel);
  virtual ~Level();

  // General Methods
  void MapTile(int iValue, Bitmap* bmpTile);
  Tile* Level::GetTile(int iValue);
  void Draw(HDC hDC);
  std::vector<std::vector<int>> GenerateLevel();
  BOOL Level::IsPointCollidable(POINT ptPosition);
  BOOL Level::IsPointMeltable(POINT ptPosition);
  POINT Level::GetNodeFromPosition(POINT ptPosition);
  void Level::SetTile(int x, int y, int tilenum) { m_layout[y][x] = tilenum; };
  void Level::SetTile(POINT ptPos, int tilenum) { m_layout[ptPos.y][ptPos.x] = tilenum; };
  int GetCurrentLevel() { return m_iCurrentLevel; }
  void SetLevelNumber(int currentLevel) { m_iCurrentLevel = currentLevel; };
};


