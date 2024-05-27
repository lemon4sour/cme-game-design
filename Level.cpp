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
Level::Level(int iTileSize, int iLevelNumber)
{
  // initialize the level
  m_layout = GenerateLevel();
  m_iLevelNumber = iLevelNumber;
  m_iTileSize = iTileSize;
  m_iWidth = m_layout[0].size();
  m_iHeight = m_layout.size();
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

BOOL Level::IsPointCollidable(POINT ptPosition) {
  int x = ptPosition.x / 32;
  int y = ptPosition.y / 32;

  if (x < 0 || y < 0 || (x >= m_iWidth) || (y >= m_iHeight)) return false;
  Tile* tile = m_tileMap[m_layout[y][x]];
  return tile->CanCollide();
}

POINT Level::GetNodeFromPosition(POINT ptPosition)
{
  return POINT{ ptPosition.x / 32, ptPosition.y / 32 };
}

std::vector<std::vector<int>> Level::GenerateLevel() {
    // Initialize the random seed
    std::srand(std::time(nullptr));

    // Create a 24x24 vector of vectors
    std::vector<std::vector<int>> layout(24, std::vector<int>(24));

    // Set the corners and sides to 1
    for (int i = 0; i < 24; ++i) {
        layout[0][i] = 1;         // Top row
        layout[23][i] = 1;        // Bottom row
        layout[i][0] = 1;         // Left column
        layout[i][23] = 1;        // Right column
    }

    for (int i = 1; i < 23; i++) {
        for (int j = 1; j < 23; j++) {
            if (i % 8 == 0 || j % 8 == 0)
                layout[i][j] = 1;
        }
    }

    int random;
    // Fill the layout with random 0s and 1s
    for (int i = 1; i < 23; ++i) {
        for (int j = 1; j < 23; ++j) {
            random = std::rand() % 10;

            if (random >= 9)
                layout[i][j] = 1;
        }
    }

    for (int i = 1; i < 23; ++i) {
        for (int j = 1; j < 23; ++j) {
            random = std::rand() % 10;

            if (layout[i][j] == 1 && random >= 6)
                layout[i][j] = 0;
        }
    }

    for (int i = 2; i < 23; ++i) {
        for (int j = 2; j < 23; ++j) {
            if (layout[i][j] == 0) {
                if (layout[i - 1][j] == 1 && // Top
                    layout[i + 1][j] == 1 && // Bottom
                    layout[i][j - 1] == 1 && // Left
                    layout[i][j + 1] == 1) {
                    // Randomly remove one of the surrounding walls
                    random = std::rand() % 4;
                    switch (random) {
                    case 0: layout[i - 1][j] = 0; break; // Remove top wall
                    case 1: layout[i + 1][j] = 0; break; // Remove bottom wall
                    case 2: layout[i][j - 1] = 0; break; // Remove left wall
                    case 3: layout[i][j + 1] = 0; break; // Remove right wall
                    }
                }
            }
        }
    }

    return layout;
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
