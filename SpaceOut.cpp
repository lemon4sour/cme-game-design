//-----------------------------------------------------------------
// Space Out Application
// C++ Source - SpaceOut.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "SpaceOut.h"

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

  Actor::initializeGame(_pGame);


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
  Bitmap* _pFireResBitmap = new Bitmap(hDC, IDB_FIRERES, _hInstance);
  Bitmap* _pWaterResBitmap = new Bitmap(hDC, IDB_WATERRES, _hInstance);
  Bitmap* _pEarthResBitmap = new Bitmap(hDC, IDB_EARTHRES, _hInstance);
  Bitmap* _pAirResBitmap = new Bitmap(hDC, IDB_AIRRES, _hInstance);
  
  _pRockBitmap = new Bitmap(hDC, IDB_ROCK, _hInstance);
  _pFireballBitmap = new Bitmap(hDC, IDB_FIREBALL, _hInstance);
  _pFlameBitmap = new Bitmap(hDC, IDB_FLAME, _hInstance);
  Flame::setFlameBitmap(_pFlameBitmap);
  _pMudBitmap = new Bitmap(hDC, IDB_MUD, _hInstance);
  _pWaterBitmap = new Bitmap(hDC, IDB_WATER, _hInstance);
  _pGustUpBitmap = new Bitmap(hDC, IDB_GUSTUP, _hInstance);
  _pGustDownBitmap = new Bitmap(hDC, IDB_GUSTDOWN, _hInstance);
  _pGustLeftBitmap = new Bitmap(hDC, IDB_GUSTLEFT, _hInstance);
  _pGustRightBitmap = new Bitmap(hDC, IDB_GUSTRIGHT, _hInstance);

  _pPointBitmap = new Bitmap(hDC, IDB_POINT, _hInstance);


  _pLevel = new Level(32, 1);
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
  _pPlayer->SetZOrder(10);
  _pPlayer->SetPosition(POINT{ 64,64 });
  _pGame->AddSprite(_pPlayer);

  std::random_device rd;
  std::mt19937 gen(rd());
  for (int i = 0; i < 8; i++)
  {
    std::uniform_int_distribution<> dis(0, 3072);

    // Select enemy type randomly
    const EnemyType type{ static_cast<EnemyType>(dis(gen) % 4) };

    // Create enemy
    Enemy* enemy = new Enemy(
      new Bitmap(hDC, 24, 24, RGB(255, 0, 255)), _pLevel, type, _pPlayer
    );

    // Random position between 0 and 1024
    enemy->SetPosition(POINT{ dis(gen), dis(gen) });

    if (enemy->AmIStuck())
    {
      reinterpret_cast<Sprite*>(enemy)->~Sprite();
      delete enemy;
      i--;
    }
    else
    {
      _pGame->AddSprite(enemy);
      _vEnemies.push_back(enemy);
    }
  }
  

  _pElementQueue = new ElementQueue(hWindow, hDC, _pEarthResBitmap, _pFireResBitmap, _pWaterResBitmap, _pAirResBitmap);
  _pElementQueue->SetPointBitmap(_pPointBitmap);
  _pElementQueue->FillRandom();

  // Play the background music
  _pGame->PlayMIDISong(TEXT("Music.mid"));

  
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
    ptVelocity.y -= 8;
  }
  if (GetAsyncKeyState('S') < 0)
  {
    ptVelocity.y += 8;
  }
  if (GetAsyncKeyState('A') < 0)
  {
    ptVelocity.x -= 8;
  }
  if (GetAsyncKeyState('D') < 0)
  {
    ptVelocity.x += 8;
  }

  _pPlayer->SetTargetVelocity(ptVelocity);

  if (ptVelocity.x == 0 && ptVelocity.y == 0) {
      _pPlayer->SetFrameDelay(1000);
  }
  else
  {
    _pPlayer->SetFrameDelay(3);
  }

  if (GetAsyncKeyState('1') < 0)
  {
      iSelect = 0;
  }
  if (GetAsyncKeyState('2') < 0)
  {
      iSelect = 1;
  }
  if (GetAsyncKeyState('3') < 0)
  {
      iSelect = 2;
  }
  if (GetAsyncKeyState('4') < 0)
  {
      iSelect = 3;
  }

  if (GetAsyncKeyState('K') < 0) {
      _pPlayer->SubtractHealth(10);
  }
  if (GetAsyncKeyState('U') < 0)
  {
    _pElementQueue->UseElement();
  }
  if (GetAsyncKeyState('Y') < 0)
  {
    _pElementQueue->AddRandomElement();
  }


  
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
  if (bLeft)
  {
    RECT rtPlayerPos = _pPlayer->GetPosition();
    POINT ptPlayerCenterPos = POINT{ (rtPlayerPos.left + rtPlayerPos.right) / 2, (rtPlayerPos.top + rtPlayerPos.bottom) / 2 };
    POINT ptMouseOffset = POINT{ x - ptPlayerCenterPos.x, y - ptPlayerCenterPos.y };

        Swing* pSwingSprite;
        if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingDownBitmap, _pLevel, POINT{0,1});
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x, ptPlayerCenterPos.y + 24 });
            _pGame->AddSprite(pSwingSprite);
            //DOWN
        }
        if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingLeftBitmap, _pLevel, POINT{ -1,0 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x - 24, ptPlayerCenterPos.y });
            _pGame->AddSprite(pSwingSprite);
            //LEFT
        }
        if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingRightBitmap, _pLevel, POINT{ 1,0 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x + 24, ptPlayerCenterPos.y });
            _pGame->AddSprite(pSwingSprite);
            //RIGHT
        }
        if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
        {
            pSwingSprite = new Swing(_pSwingUpBitmap, _pLevel, POINT{ 0,-1 });
            pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerCenterPos.x, ptPlayerCenterPos.y - 24 });
            _pGame->AddSprite(pSwingSprite);
            //UP
        }
    }
    else {
        if (iSelect == 0) {
            Rock* pRock = new Rock(_pRockBitmap, _pLevel);
            pRock->SetPositionFromCenter(_pPlayer->GetPositionFromCenter());
            _pGame->AddSprite(pRock);
        }
        if (iSelect == 1) {
            POINT ptPlayerPos = _pPlayer->GetPositionFromCenter();
            POINT ptMouseOffset = POINT{ x - ptPlayerPos.x, y - ptPlayerPos.y };

            Fireball* pFireball = new Fireball(_pFireballBitmap, _pLevel);
            pFireball->SetNumFrames(3);
            pFireball->SetFrameDelay(3);
            pFireball->SetPositionFromCenter(ptPlayerPos);

            if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
            {
                pFireball->SetVelocity(POINT{0, 15});                
                //DOWN
            }
            if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
            {
                pFireball->SetVelocity(POINT{ -15, 0 });
                //LEFT
            }
            if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
            {
                pFireball->SetVelocity(POINT{ 15, 0 });
                //RIGHT
            }
            if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
            {
                pFireball->SetVelocity(POINT{ 0, -15 });
                //UP
            }
            _pGame->AddSprite(pFireball);
        }
        if (iSelect == 2) {
            Puddle* pPuddle = new Puddle(_pWaterBitmap, _pLevel);
            pPuddle->SetPositionFromCenter(_pPlayer->GetPositionFromCenter());
            _pGame->AddSprite(pPuddle);
        }
        if (iSelect == 3) {
            POINT ptPlayerPos = _pPlayer->GetPositionFromCenter();
            POINT ptMouseOffset = POINT{ x - ptPlayerPos.x, y - ptPlayerPos.y };

            Gust* pGust;
            POINT ptGustVelocity;
            
            if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
            {
                pGust = new Gust(_pGustDownBitmap, _pLevel);               
                ptGustVelocity = POINT{ 0, 8 };
                //DOWN
            }
            if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
            {
                pGust = new Gust(_pGustLeftBitmap, _pLevel);
                ptGustVelocity = POINT{ -8, 0 };
                //LEFT
            }
            if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
            {
                pGust = new Gust(_pGustRightBitmap, _pLevel);
                ptGustVelocity = POINT{ 8, 0 };
                //RIGHT
            }
            if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
            {
                pGust = new Gust(_pGustUpBitmap, _pLevel);
                ptGustVelocity = POINT{ 0, -8 };
                //UP
            }

            
            pGust->SetVelocity(ptGustVelocity);
            pGust->SetBoundsAction(BA_DIE);
            POINT ptPlayerVelocity = _pPlayer->GetVelocity();
            _pPlayer->SetVelocity(POINT{ ptPlayerVelocity.x - (ptGustVelocity.x*2),ptPlayerVelocity.y - (ptGustVelocity.y*2) });

            pGust->SetPositionFromCenter(ptPlayerPos);
            _pGame->AddSprite(pGust);

        }
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
    Swing* swing = dynamic_cast<Swing*>(pSpriteHitter);
    // SWING INTERACTION
    if (swing) {
        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        // SWING TO ENEMY
        if (enemy) {
            enemy->Kill();
            return false;
        }
        Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
        // SWING TO ROCK
        if (rock) {
            if (rock->GetCooldown() > 0) return false;
            rock->SetCooldown(3);
            int hits = rock->GetNumHits();
            if (hits >= rock->GetMaxHits()) {
                rock->Kill();
                return false;
            }
            rock->GetNumHits(++hits);

            if (swing->GetDirection().x == 0) {
                rock->SetVelocity((rock->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) + (rand() % 5) - 2, swing->GetDirection().y * 30);
            }
            else {
                rock->SetVelocity(swing->GetDirection().x * 30, (rock->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) + (rand() % 5) - 2);
            }

            return false;
        }
    }

    Rock* rock = dynamic_cast<Rock*>(pSpriteHitter);
    // ROCK INTERACTION
    if (rock) {
        Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
        // ROCK TO PUDDLE
        if (puddle) {
            Mud* _pMud = new Mud(_pMudBitmap, _pLevel);
            _pMud->SetPositionFromCenter(puddle->GetPositionFromCenter());
            _pGame->AddSprite(_pMud);
            puddle->Kill();
            rock->Kill();
            return false;
        }

        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        // ROCK TO ENEMY
        if (enemy) {
            if (rock->GetVelocity().x != 0 || rock->GetVelocity().y != 0) {
                enemy->Kill();
            }
            return false;
        }
    }

    Fireball* fireball = dynamic_cast<Fireball*>(pSpriteHitter);
    // FIREBALL INTERACTION
    if (fireball) {
        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        // FIREBALL TO ENEMY
        if (enemy) {
            enemy->Kill();
            return false;
        }
        Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
        // FIREBALL TO ROCK
        if (rock) {
            rock->Kill();

            vector<POINT> directions = {
            POINT {8  + (rand() % 5), 8  + (rand() % 5)},
            POINT {-2 + (rand() % 5), 8  + (rand() % 5)},
            POINT {-8 - (rand() % 5), 8  + (rand() % 5)},
            POINT {-8 - (rand() % 5), -2 + (rand() % 5)},
            POINT {-8 - (rand() % 5), -8 - (rand() % 5)},
            POINT {-2 + (rand() % 5), -8 - (rand() % 5)},
            POINT {8  + (rand() % 5), -8 - (rand() % 5)},
            POINT {8  + (rand() % 5), -2 + (rand() % 5)},
            };

            for (POINT point : directions) {
                Flame* pFlame = new Flame(Flame::m_pFlameBitmap, _pLevel);
                pFlame->SetNumFrames(3);
                pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
                pFlame->SetVelocity(point);
                _pGame->AddSprite(pFlame);
            }

            fireball->Kill();
            return false;
        }
        Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
        // FIREBALL TO PUDDLE
        if (puddle) {
            fireball->Kill();
            return false;
        }
    }

    Flame* flame = dynamic_cast<Flame*>(pSpriteHitter);
    // FLAME INTERACTION
    if (flame) {
        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        // FLAME TO ENEMY
        if (enemy) {
            enemy->Kill();
            return false;
        }
        Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
        // FLAME TO PUDDLE
        if (puddle) {
            flame->Kill();
            return false;
        }
    }

    Mud* mud = dynamic_cast<Mud*>(pSpriteHitter);
    // MUD INTERACTION
    if (mud) {
        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        // MUD TO ENEMY
        if (enemy) {
            enemy->Kill();
            return false;
        }
        Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
        // MUD TO PUDDLE
        if (puddle) {
            if (mud->getSpreadCooldown() <= 0) {
                Mud* _pMud = new Mud(_pMudBitmap, _pLevel);
                _pMud->SetPositionFromCenter(puddle->GetPositionFromCenter());
                _pGame->AddSprite(_pMud);
                puddle->Kill();
            }
            return false;
        }
    }

    Gust* gust = dynamic_cast<Gust*>(pSpriteHitter);
    // GUST INTERACTION
    if (gust) {
        Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
        if (enemy) {
                enemy->Kill();
            return false;
        }
        Flame* flame = dynamic_cast<Flame*>(pSpriteHittee);
        if (flame) {
                if (!flame->IsCloned() && flame->GetCloneDepth() < 4) {

                    Flame* pFlame = new Flame(flame->GetBitmap(), _pLevel);
                    flame->SetCloneDepth(flame->GetCloneDepth() + 1);
                    pFlame->SetCloneDepth(flame->GetCloneDepth());
                    flame->SetCloned(true);
                    pFlame->SetCloned(true);
                    pFlame->SetNumFrames(3);
                    if (gust->GetVelocity().x == 0) {
                        pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x - 10, flame->GetPositionFromCenter().y);
                        flame->SetPositionFromCenter(flame->GetPositionFromCenter().x + 10, flame->GetPositionFromCenter().y);
                    }
                    else {
                        pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y - 10);
                        flame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y + 10);
                    }
                    _pGame->AddSprite(pFlame);
                }

                if (gust->GetVelocity().x == 0) {
                    flame->SetVelocity(gust->GetVelocity().x - ((gust->GetPositionFromCenter().x - flame->GetPositionFromCenter().x) / 6), (gust->GetVelocity().y / 2) + flame->GetVelocity().y);
                }
                else {
                    flame->SetVelocity((gust->GetVelocity().x / 2) + flame->GetVelocity().x, gust->GetVelocity().y - ((gust->GetPositionFromCenter().y - flame->GetPositionFromCenter().y) / 6));
                }
                flame->SetTime((rand() % 40) + 80);
            return false;
        }
        Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
        if (rock) {
                if (gust->GetVelocity().x == 0) {
                    rock->SetVelocity(gust->GetVelocity().x - ((gust->GetPositionFromCenter().x - rock->GetPositionFromCenter().x) / 6), ((3 * gust->GetVelocity().y) / 2) + rock->GetVelocity().y);
                }
                else {
                    rock->SetVelocity(((3 * gust->GetVelocity().x) / 2) + rock->GetVelocity().x, gust->GetVelocity().y - ((gust->GetPositionFromCenter().y - rock->GetPositionFromCenter().y) / 6));
                }
            return false;
        }
    }
}

//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------
