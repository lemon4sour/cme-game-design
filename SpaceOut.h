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

Bitmap* _pOrbHealthBitmap;
Bitmap* _pOrbFireBitmap;
Bitmap* _pOrbAirBitmap;
Bitmap* _pOrbWaterBitmap;
Bitmap* _pOrbEarthBitmap;

Bitmap* _pFireResBitmap;
Bitmap* _pWaterResBitmap;
Bitmap* _pEarthResBitmap;
Bitmap* _pAirResBitmap;
Bitmap* _pPointBitmap;

Bitmap* _pPlayerDownBitmap;
Bitmap* _pPlayerUpBitmap;
Bitmap* _pPlayerLeftBitmap;
Bitmap* _pPlayerRightBitmap;

int _iCurrentLevel = 1;
const int _iMaxLevel = 5;

int _iBreatherTime = 0;
bool _bLevelClear = false;

int _iGameTime = 0;

Player* _pPlayer;

std::vector<Enemy*> _vEnemies;

Inventory* _pInventory;

char _lastDirection = 'R';

const unsigned int _iMaxOrbNumber = 10;
unsigned int _iCurrentOrbNumber;
//-----------------------------------------------------------------
// Function Declarations
//-----------------------------------------------------------------
void InitializeResources(HDC hDC);
void CreatePlayer(HDC hDC);
void CreateEnemies(HDC hDC);
void CreateInventory(HDC hDC);

void NextLevel(HDC hDC);

void GameOver();
void GameWin();

char PlayerDirectionUpdateRoutine();
Enemy* CreateEnemy(EnemyType enemyType);
Orb* CreateRandomOrb();
Orb* CreateOrb(OrbType type);
void SwingCombined(POINT targetPos, char direction);
void ElementUseCombined(POINT targetPos, char direction);

void SpawnOrb();

// INNER FUNCTIONS
void ClearBeforeNextLevel();
Enemy* CreateEnemy(EnemyType enemyType);
