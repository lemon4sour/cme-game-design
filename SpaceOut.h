//-----------------------------------------------------------------
// Space Out Application
// C++ Header - SpaceOut.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Level.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Actor.h"
#include "Gui.h"

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE         _hInstance;
GameEngine*       _pGame;
HDC               _hOffscreenDC;
HBITMAP           _hOffscreenBitmap;

Level*            _pLevel;
Bitmap*           _pEmptyBitmap;
Bitmap*	          _pWallBitmap;

Player*           _pPlayer;

ElementQueue*     _pElementQueue;
//-----------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------
