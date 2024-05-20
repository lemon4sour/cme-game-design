//-----------------------------------------------------------------
// Space Out Application
// C++ Header - SpaceOut.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"
#include "Bitmap.h"
#include "GameEngine.h"
#include "Level.h"
#include "Resource.h"
#include "Sprite.h"
#include <windows.h>

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE         _hInstance;
GameEngine* _pGame;
HDC               _hOffscreenDC;
HBITMAP           _hOffscreenBitmap;

Level* _pLevel;
Bitmap* _pEmptyBitmap;
Bitmap* _pWallBitmap;
Bitmap* _pSwingLeftBitmap;
Bitmap* _pSwingRightBitmap;
Bitmap* _pSwingDownBitmap;
Bitmap* _pSwingUpBitmap;

Player* _pPlayer;

std::vector<Enemy*> _enemies;

//-----------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------
