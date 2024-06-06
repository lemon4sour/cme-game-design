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
#include "Gui.h"
#include "Level.h"
#include "Resource.h"
#include "Sprite.h"
#include <random>

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
Bitmap* _pRockBitmap;
Bitmap* _pFireballBitmap;
Bitmap* _pFlameBitmap;
Bitmap* _pWaterBitmap;
Bitmap* _pMudBitmap;
Bitmap* _pIceBitmap;
Bitmap* _pIceSpriteBitmap;
Bitmap* _pGustUpBitmap;
Bitmap* _pGustDownBitmap;
Bitmap* _pGustLeftBitmap;
Bitmap* _pGustRightBitmap;
Bitmap* _pSkullLeftBitmap;
Bitmap* _pSkullRightBitmap;
Bitmap* _pEyeLeftBitmap;
Bitmap* _pEyeRightBitmap;
Bitmap* _pEyeBackLeftBitmap;
Bitmap* _pEyeBackRightBitmap;
Bitmap* _pEyeBulletBitmap;
Bitmap* _pSlimeBitmap;
Bitmap* _pHumongousFrontBitmap;

Bitmap* _pHealthBar100Bitmap;
Bitmap* _pHealthBar75Bitmap;
Bitmap* _pHealthBar50Bitmap;
Bitmap* _pHealthBar25Bitmap;

Bitmap* _pPointBitmap;

Player* _pPlayer;

std::vector<Enemy*> _vEnemies;

ElementQueue* _pElementQueue;

char _lastDirection = 'R';
//-----------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------
void EnemySpawnRoutine(HDC hDC);

char PlayerDirectionUpdateRoutine();
Enemy* CreateEnemy(EnemyType enemyType);
void SwingCombined(POINT targetPos, char direction);
void ElementUseCombined(POINT targetPos, char direction);
