//-----------------------------------------------------------------
// Level Object
// C++ Source - Level.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Level.h"

//-----------------------------------------------------------------
// Level Constructor(s)/Destructor
//-----------------------------------------------------------------
Level::Level(std::vector<std::vector<int>> layout, int iTileSize, int iLevelNumber)
{
  // initialize the level
  m_layout = layout;
  m_iLevelNumber = iLevelNumber;
  m_iTileSize = iTileSize;
  m_iWidth = layout[0].size();
  m_iHeight = layout.size();
}

Level::~Level()
{

}

//-----------------------------------------------------------------
// Level General Methods
//-----------------------------------------------------------------

void Level::MapTile(int iValue, Bitmap* bmpTile, BOOL bCollidable)
{
  m_tileMap[iValue] = new Tile(bmpTile, bCollidable);
}


void Level::Draw(HDC hDC)
{
  for (int y = 0; y < m_layout.size(); y++)
  {
    for (int x = 0; x < m_layout[y].size(); x++)
    {
      Tile* tile = m_tileMap[m_layout[y][x]];
      tile->Draw(hDC, (x * m_iTileSize), (y * m_iTileSize));
    }
  }
}

BOOL Level::IsPointCollidable(POINT ptPosition)
{
  return m_tileMap[m_layout[ptPosition.y / 128][ptPosition.x / 128]]->CanCollide();
}

POINT Level::GetNodeFromPosition(POINT ptPosition)
{
  return POINT{ ptPosition.x / 128, ptPosition.y / 128 };
}

//-----------------------------------------------------------------
// Tile Constructor(s)/Destructor
//-----------------------------------------------------------------
Tile::Tile(Bitmap* bmpTile, BOOL bCollidable)
{
  m_bmpBitmap = bmpTile;
  m_bCollidable = bCollidable;
}

Tile::~Tile()
{}

void Tile::Draw(HDC hDC, int iPosX, int iPosY)
{
  m_bmpBitmap->Draw(hDC, iPosX, iPosY);
}
