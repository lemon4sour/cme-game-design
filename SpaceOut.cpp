//-----------------------------------------------------------------
// Space Out Application
// C++ Source - SpaceOut.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "SpaceOut.h"
#include <random>

//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE hInstance)
{
  // Create the game engine
  _pGame = new GameEngine(hInstance, TEXT("Space Out"),
                          TEXT("Space Out"), IDI_SPACEOUT, IDI_SPACEOUT_SM, 1024, 768);
  if (_pGame == NULL)
    return FALSE;

  // Set the frame rate
  _pGame->SetFrameRate(30);

  // Store the instance handle
  _hInstance = hInstance;

  return TRUE;
}

void GameStart(HWND hWindow)
{
  // Seed the random number generator
  srand(GetTickCount64());


  // Create the offscreen device context and bitmap
  _hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
  _hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
                                             _pGame->GetWidth(), _pGame->GetHeight());
  SelectObject(_hOffscreenDC, _hOffscreenBitmap);

  // Create and load the bitmaps
  HDC hDC = GetDC(hWindow);
  _pEmptyBitmap = new Bitmap(hDC, IDB_EMPTY, _hInstance);
  _pWallBitmap = new Bitmap(hDC, IDB_WALL, _hInstance);
  _pSwingLeftBitmap = new Bitmap(hDC, 24, 48, RGB(255, 0, 0));
  _pSwingRightBitmap = new Bitmap(hDC, 24, 48, RGB(255, 0, 0));
  _pSwingUpBitmap = new Bitmap(hDC, 48, 24, RGB(255, 0, 0));
  _pSwingDownBitmap = new Bitmap(hDC, 48, 24, RGB(255, 0, 0));
  Bitmap* _pFireBitmap = new Bitmap(hDC, IDB_FIRERES, _hInstance);
  Bitmap* _pWaterBitmap = new Bitmap(hDC, IDB_WATERRES, _hInstance);
  Bitmap* _pEarthBitmap = new Bitmap(hDC, IDB_EARTHRES, _hInstance);
  Bitmap* _pAirBitmap = new Bitmap(hDC, IDB_AIRRES, _hInstance);
  
  _pRockBitmap = new Bitmap(hDC, IDB_ROCK, _hInstance);

  std::vector<std::vector<int>> layout = {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  };

  _pLevel = new Level(layout, 32, 1);
  _pLevel->MapTile(0, _pEmptyBitmap);
  _pLevel->MapTile(1, _pWallBitmap, FALSE);

  Bitmap* bmpPlayerDown = new Bitmap(hDC, IDB_PLAYERMOVEDOWN, _hInstance);
  Bitmap* bmpPlayerUp = new Bitmap(hDC, IDB_PLAYERMOVEUP, _hInstance);
  Bitmap* bmpPlayerLeft = new Bitmap(hDC, IDB_PLAYERMOVELEFT, _hInstance);
  Bitmap* bmpPlayerRight = new Bitmap(hDC, IDB_PLAYERMOVERIGHT, _hInstance);
  _pPlayer = new Player(bmpPlayerDown, _pLevel);
  _pPlayer->LinkBitmapToState(PLR_DOWN, bmpPlayerDown);
  _pPlayer->LinkBitmapToState(PLR_UP, bmpPlayerUp);
  _pPlayer->LinkBitmapToState(PLR_LEFT, bmpPlayerLeft);
  _pPlayer->LinkBitmapToState(PLR_RIGHT, bmpPlayerRight);
  _pPlayer->SetState(PLR_DOWN);
  _pPlayer->SetNumFrames(4);
  _pPlayer->SetFrameDelay(3);
  _pPlayer->SetPosition(POINT{ 256,256 });
  _pGame->AddSprite(_pPlayer);

  for (int i = 0; i < 5; i++)
  {
    // Modern C++ random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3072);

    // Random color for the enemy
    Bitmap* bitmap2 = new Bitmap(hDC, 24, 24, RGB(dis(gen) % 256, dis(gen) % 256, dis(gen) % 256));
    Enemy* enemy = new Enemy(bitmap2, _pLevel);

    // Random position between 0 and 1024
    enemy->SetPosition(POINT{ dis(gen), dis(gen) });

    if (enemy->AmIStuck())
    {
      delete enemy;
      delete bitmap2;
      i--;
    }
    else
    {
      _pGame->AddSprite(enemy);
      enemy->SetTarget(_pPlayer);
      _vEnemies.push_back(enemy);
    }
  }

  _pElementQueue = new ElementQueue(hWindow, hDC, _pEarthBitmap, _pFireBitmap, _pWaterBitmap, _pAirBitmap);
  _pElementQueue->FillRandom();

  //Play the background music
  _pGame->PlayMIDISong(TEXT("Music.mid"));

  Actor::initializeSprites(_pGame->GetSpritesListPointer());
}

void GameEnd()
{
  // Close the MIDI player for the background music
  _pGame->CloseMIDIPlayer();

  // Cleanup the offscreen device context and bitmap
  DeleteObject(_hOffscreenBitmap);
  DeleteDC(_hOffscreenDC);

  // Cleanup the bitmaps
  delete _pWallBitmap;

  // Cleanup the sprites
  _pGame->CleanupSprites();

  // Cleanup the game engine
  delete _pGame;
}

void GameActivate(HWND hWindow)
{
  // Resume the background music
  _pGame->PlayMIDISong(TEXT(""), FALSE);
}

void GameDeactivate(HWND hWindow)
{
  // Pause the background music
  _pGame->PauseMIDISong();
}

void GamePaint(HDC hDC)
{
  _pLevel->Draw(hDC);

  // Draw the sprites
  _pGame->DrawSprites(hDC);
  
}

void GameCycle()
{
  // Update the sprites
  _pGame->UpdateSprites();

  // Obtain a device context for repainting the game
  HWND  hWindow = _pGame->GetWindow();
  HDC   hDC = GetDC(hWindow);

  // Paint the game to the offscreen device context
  GamePaint(_hOffscreenDC);


  // Blit the offscreen bitmap to the game screen
  BitBlt(hDC, 0, 0, _pGame->GetWidth(), _pGame->GetHeight(),
         _hOffscreenDC, 0, 0, SRCCOPY);

  // Paint the GUI
  PaintHealthBar(hWindow, hDC, _pPlayer->GetMaxHealth(), _pPlayer->GetCurrentHealth());
  PrintTime(hWindow, hDC);
  _pElementQueue->DrawQueue();

  // Cleanup
  ReleaseDC(hWindow, hDC);
}

void HandleKeys()
{
  POINT ptVelocity = POINT{ 0, 0 };
  if (GetAsyncKeyState('W') < 0)
  {
    ptVelocity.y -= 32;
  }
  if (GetAsyncKeyState('S') < 0)
  {
    ptVelocity.y += 32;
  }
  if (GetAsyncKeyState('A') < 0)
  {
    ptVelocity.x -= 32;
  }
  if (GetAsyncKeyState('D') < 0)
  {
    ptVelocity.x += 32;
  }

  if (ptVelocity.x == 0 && ptVelocity.y == 0) {
      _pPlayer->SetFrameDelay(100);
  }
  else {
      _pPlayer->SetFrameDelay(3);
  }

  if (GetAsyncKeyState('K') < 0) {
      _pPlayer->SubtractHealth(10);
  }
  if (GetAsyncKeyState('U') < 0) {
      _pElementQueue->UseElement();
  }
  if (GetAsyncKeyState('Y') < 0) {
      _pElementQueue->AddRandomElement();
  }


  _pPlayer->SetTargetVelocity(ptVelocity);
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
    if (bLeft) {
        RECT rtPlayerPos = _pPlayer->GetPosition();
        POINT ptPlayerCenterPos = POINT{ (rtPlayerPos.left + rtPlayerPos.right) / 2, (rtPlayerPos.top + rtPlayerPos.bottom) / 2 };
        POINT ptMouseOffset = POINT{ x - ptPlayerCenterPos.x, y - ptPlayerCenterPos.y };

        Swing* pSwingSprite;
        if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingDownBitmap, _pLevel, POINT{0,1});
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x, ptPlayerCenterPos.y + 96 });
            _pGame->AddSprite(pSwingSprite);
            //DOWN
        }
        if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingLeftBitmap, _pLevel, POINT{ -1,0 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x - 96, ptPlayerCenterPos.y });
            _pGame->AddSprite(pSwingSprite);
            //LEFT
        }
        if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingRightBitmap, _pLevel, POINT{ 1,0 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x + 96, ptPlayerCenterPos.y });
            _pGame->AddSprite(pSwingSprite);
            //RIGHT
        }
        if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingUpBitmap, _pLevel, POINT{ 0,-1 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x, ptPlayerCenterPos.y - 96 });
            _pGame->AddSprite(pSwingSprite);
            //UP
        }
    }
    else {
        Rock* pRock = new Rock(_pRockBitmap, _pLevel);
        pRock->SetPositionFromCenter(_pPlayer->GetPositionFromCenter());
        _pGame->AddSprite(pRock);
    }
}

void MouseButtonUp(int x, int y, BOOL bLeft)
{}

void MouseMove(int x, int y)
{
    _pPlayer->SetMousePos(x, y);
}

void HandleJoystick(JOYSTATE jsJoystickState)
{}

BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
  return FALSE;
}

//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------
