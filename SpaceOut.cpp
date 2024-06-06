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
  _pIceBitmap = new Bitmap(hDC, IDB_ICE, _hInstance);
  _pSwingLeftBitmap = new Bitmap(hDC, IDB_SWINGLEFT, _hInstance);
  _pSwingRightBitmap = new Bitmap(hDC, IDB_SWINGRIGHT, _hInstance);
  _pSwingUpBitmap = new Bitmap(hDC, IDB_SWINGUP, _hInstance);
  _pSwingDownBitmap = new Bitmap(hDC, IDB_SWINGDOWN, _hInstance);
  Bitmap* _pFireResBitmap = new Bitmap(hDC, IDB_FIRERES, _hInstance);
  Bitmap* _pWaterResBitmap = new Bitmap(hDC, IDB_WATERRES, _hInstance);
  Bitmap* _pEarthResBitmap = new Bitmap(hDC, IDB_EARTHRES, _hInstance);
  Bitmap* _pAirResBitmap = new Bitmap(hDC, IDB_AIRRES, _hInstance);

  _pRockBitmap = new Bitmap(hDC, IDB_ROCK, _hInstance);
  _pFireballBitmap = new Bitmap(hDC, IDB_FIREBALL, _hInstance);
  _pFlameBitmap = new Bitmap(hDC, IDB_FLAME, _hInstance);
  Flame::setFlameBitmap(_pFlameBitmap);
  _pMudBitmap = new Bitmap(hDC, IDB_MUD, _hInstance);
  _pIceSpriteBitmap = new Bitmap(hDC, 32, 32, RGB(255, 0, 255));
  _pWaterBitmap = new Bitmap(hDC, IDB_WATER, _hInstance);
  _pGustUpBitmap = new Bitmap(hDC, IDB_GUSTUP, _hInstance);
  _pGustDownBitmap = new Bitmap(hDC, IDB_GUSTDOWN, _hInstance);
  _pGustLeftBitmap = new Bitmap(hDC, IDB_GUSTLEFT, _hInstance);
  _pGustRightBitmap = new Bitmap(hDC, IDB_GUSTRIGHT, _hInstance);

  _pPointBitmap = new Bitmap(hDC, IDB_POINT, _hInstance);

  _pLevel = new Level(32, 1);
  _pLevel->MapTile(0, _pEmptyBitmap);
  _pLevel->GetTile(0)->SetCollidable();
  _pLevel->MapTile(1, _pWallBitmap);
  _pLevel->MapTile(2, _pIceBitmap);
  _pLevel->GetTile(2)->SetMeltable();

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

  EnemySpawnRoutine(hDC);

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

  _pGame->InsertSpritesFromBuffer();

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

static bool swingKeyPressed = false;
static bool abilityKeyPressed = false;
void HandleKeys()
{
  POINT ptVelocity = POINT{ 0, 0 };
  if (GetAsyncKeyState(VK_UP) < 0)
  {
    ptVelocity.y -= 8;
  }
  if (GetAsyncKeyState(VK_DOWN) < 0)
  {
    ptVelocity.y += 8;
  }
  if (GetAsyncKeyState(VK_LEFT) < 0)
  {
    ptVelocity.x -= 8;
  }
  if (GetAsyncKeyState(VK_RIGHT) < 0)
  {
    ptVelocity.x += 8;
  }

  // Set target velocity
  _pPlayer->SetTargetVelocity(ptVelocity);

  // Update/Get direction
  char direction = PlayerDirectionUpdateRoutine();

  if (ptVelocity.x == 0 && ptVelocity.y == 0)
  {
    _pPlayer->SetFrameDelay(1000);
  }
  else
  {
    _pPlayer->SetFrameDelay(3);
  }

  if (GetAsyncKeyState('A') < 0)
  {
    iSelect = 0;
  }
  if (GetAsyncKeyState('S') < 0)
  {
    iSelect = 1;
  }
  if (GetAsyncKeyState('D') < 0)
  {
    iSelect = 2;
  }
  if (GetAsyncKeyState('F') < 0)
  {
    iSelect = 3;
  }

  // Space swings to direction character going
  if (GetAsyncKeyState(VK_SPACE) < 0)
  {
    if (!swingKeyPressed)
    {
      RECT rtPlayerPos = _pPlayer->GetPosition();
      POINT ptPlayerCenterPos = POINT{ (rtPlayerPos.left + rtPlayerPos.right) / 2, (rtPlayerPos.top + rtPlayerPos.bottom) / 2 };
      POINT ptPlayerVelocity = _pPlayer->GetVelocity();

      SwingCombined(ptPlayerVelocity, direction);
      swingKeyPressed = true;
    }
  }
  else
  {
    if (swingKeyPressed)
    {
      swingKeyPressed = false;
    }
  }

  // Use elements with C
  if (GetAsyncKeyState('C') < 0)
  {
    if (!abilityKeyPressed)
    {
      RECT rtPlayerPos = _pPlayer->GetPosition();
      POINT ptPlayerCenterPos = POINT{ (rtPlayerPos.left + rtPlayerPos.right) / 2, (rtPlayerPos.top + rtPlayerPos.bottom) / 2 };
      POINT ptPlayerVelocity = _pPlayer->GetVelocity();

      ElementUseCombined(ptPlayerVelocity, direction);
      abilityKeyPressed = true;
    }
  }
  else
  {
    if (abilityKeyPressed)
    {
      abilityKeyPressed = false;
    }
  }
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
  RECT rtPlayerPos = _pPlayer->GetPosition();
  POINT ptPlayerCenterPos = POINT{ (rtPlayerPos.left + rtPlayerPos.right) / 2, (rtPlayerPos.top + rtPlayerPos.bottom) / 2 };
  POINT ptMouseOffset = POINT{ x - ptPlayerCenterPos.x, y - ptPlayerCenterPos.y };

  char direction = PlayerDirectionUpdateRoutine();

  if (bLeft)
  {
    SwingCombined(ptMouseOffset, direction);
  }
  else
  {
    ElementUseCombined(ptMouseOffset, direction);
  }
}

void MouseButtonUp(int x, int y, BOOL bLeft)
{}

void MouseMove(int x, int y)
{
  // _pPlayer->SetMousePos(x, y);
}

void HandleJoystick(JOYSTATE jsJoystickState)
{}

BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
  Swing* swing = dynamic_cast<Swing*>(pSpriteHitter);
  // SWING INTERACTION
  if (swing)
  {
    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    // SWING TO ENEMY
    if (enemy)
    {
      if (enemy->GetDamageCooldown() <= 0)
      {
        if (swing->GetDirection().x == 0)
        {
          enemy->SetVelocity((enemy->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) / 3 + (rand() % 5) - 2, swing->GetDirection().y * 5);
        }
        else
        {
          enemy->SetVelocity(swing->GetDirection().x * 5, (enemy->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) / 3 + (rand() % 5) - 2);
        }
      }
      enemy->DealDamage(20);
      return false;
    }
    Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
    // SWING TO ROCK
    if (rock)
    {
      if (rock->GetCooldown() > 0) return false;
      rock->SetCooldown(3);
      int hits = rock->GetNumHits();
      if (hits >= rock->GetMaxHits())
      {
        rock->Kill();
        return false;
      }
      rock->GetNumHits(++hits);

      if (swing->GetDirection().x == 0)
      {
        rock->SetVelocity((rock->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) + (rand() % 5) - 2, swing->GetDirection().y * 30);
      }
      else
      {
        rock->SetVelocity(swing->GetDirection().x * 30, (rock->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) + (rand() % 5) - 2);
      }

      return false;
    }
    Fireball* fireball = dynamic_cast<Fireball*>(pSpriteHittee);
    // SWING TO FIREBALL
    if (fireball)
    {
      if (fireball->isEnemy())
      {
        if (swing->GetDirection().x == 0)
        {
          fireball->SetVelocity(((fireball->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) / 3) + (rand() % 5) - 2, swing->GetDirection().y * 10);
        }
        else
        {
          fireball->SetVelocity(swing->GetDirection().x * 10, ((fireball->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) / 3) + (rand() % 5) - 2);
        }
        fireball->parry();
      }
    }
  }

  Rock* rock = dynamic_cast<Rock*>(pSpriteHitter);
  // ROCK INTERACTION
  if (rock)
  {
    Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
    // ROCK TO PUDDLE
    if (puddle)
    {
      Mud* _pMud = new Mud(_pMudBitmap, _pLevel);
      _pMud->SetPositionFromCenter(puddle->GetPositionFromCenter());
      _pGame->AddSprite(_pMud);
      puddle->Kill();
      rock->Kill();
      return false;
    }

    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    // ROCK TO ENEMY
    if (enemy)
    {
      if (rock->GetVelocity().x != 0 || rock->GetVelocity().y != 0)
      {
        if (enemy->GetDamageCooldown() <= 0)
        {
          enemy->SetVelocity(enemy->GetVelocity().x + rock->GetVelocity().x, enemy->GetVelocity().y + rock->GetVelocity().y);
        }
        enemy->DealDamage(rock->GetVelocity().x + rock->GetVelocity().y);
      }
      return false;
    }
  }

  Fireball* fireball = dynamic_cast<Fireball*>(pSpriteHitter);
  // FIREBALL INTERACTION
  if (fireball)
  {
    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    // FIREBALL TO ENEMY
    if (enemy)
    {
      if (fireball->isEnemy() && !fireball->isParried()) return false;
      if (enemy->GetEnemyType() == EnemyType::FIRE_SKULL) return false;
      if (enemy->GetEnemyType() == EnemyType::DEAD_EYE) fireball->Kill();
      enemy->DealDamage(80);
      return false;
    }
    Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
    // FIREBALL TO ROCK
    if (rock)
    {
      rock->Kill();

      vector<POINT> directions = {
      POINT {8 + (rand() % 5), 8 + (rand() % 5)},
      POINT {-2 + (rand() % 5), 8 + (rand() % 5)},
      POINT {-8 - (rand() % 5), 8 + (rand() % 5)},
      POINT {-8 - (rand() % 5), -2 + (rand() % 5)},
      POINT {-8 - (rand() % 5), -8 - (rand() % 5)},
      POINT {-2 + (rand() % 5), -8 - (rand() % 5)},
      POINT {8 + (rand() % 5), -8 - (rand() % 5)},
      POINT {8 + (rand() % 5), -2 + (rand() % 5)},
      };

      for (POINT point : directions)
      {
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
    if (puddle)
    {
      fireball->Kill();
      return false;
    }
    Player* player = dynamic_cast<Player*>(pSpriteHittee);
    // FIREBALL TO PLAYER
    if (player)
    {
      if (fireball->isEnemy())
      {
        player->SubtractHealth(10);
        fireball->Kill();
      }
      return false;
    }
    Ice* ice = dynamic_cast<Ice*>(pSpriteHittee);
    if (ice)
    {
      if (fireball->isEnemy()) return false;
      ice->Kill();
      fireball->Kill();
      return false;
    }
  }

  Flame* flame = dynamic_cast<Flame*>(pSpriteHitter);
  // FLAME INTERACTION
  if (flame)
  {
    Player* player = dynamic_cast<Player*>(pSpriteHittee);
    // FLAME TO PLAYER
    if (player)
    {
      if (player->GetInvFrames() <= 0)
      {
        flame->SubtractTime(20);
      }
      player->SubtractHealth(10);
      return false;
    }
    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    // FLAME TO ENEMY
    if (enemy)
    {
      if (enemy->GetEnemyType() == EnemyType::FIRE_SKULL) return false;
      if (enemy->GetDamageCooldown() <= 0)
      {
        flame->SubtractTime(20);
      }
      enemy->DealDamage(20);
      return false;
    }
    Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
    // FLAME TO PUDDLE
    if (puddle)
    {
      flame->Kill();
      return false;
    }
  }

  Mud* mud = dynamic_cast<Mud*>(pSpriteHitter);
  // MUD INTERACTION
  if (mud)
  {
    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    // MUD TO ENEMY
    if (enemy)
    {
      enemy->SetMudded();
      return false;
    }
    Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
    // MUD TO PUDDLE
    if (puddle)
    {
      if (mud->getSpreadCooldown() <= 0)
      {
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
  if (gust)
  {
    Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHittee);
    if (enemy)
    {
      if (gust->GetVelocity().x == 0)
      {
        enemy->SetVelocity(gust->GetVelocity().x - ((gust->GetPositionFromCenter().x - enemy->GetPositionFromCenter().x) / 6), ((3 * gust->GetVelocity().y) / 2) + enemy->GetVelocity().y);
      }
      else
      {
        enemy->SetVelocity(((3 * gust->GetVelocity().x) / 2) + enemy->GetVelocity().x, gust->GetVelocity().y - ((gust->GetPositionFromCenter().y - enemy->GetPositionFromCenter().y) / 6));
      }
      return false;
    }
    Flame* flame = dynamic_cast<Flame*>(pSpriteHittee);
    if (flame)
    {
      if (!flame->IsCloned() && flame->GetCloneDepth() < 4)
      {

        Flame* pFlame = new Flame(flame->GetBitmap(), _pLevel);
        flame->SetCloneDepth(flame->GetCloneDepth() + 1);
        pFlame->SetCloneDepth(flame->GetCloneDepth());
        flame->SetCloned(true);
        pFlame->SetCloned(true);
        pFlame->SetNumFrames(3);
        if (gust->GetVelocity().x == 0)
        {
          pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x - 10, flame->GetPositionFromCenter().y);
          flame->SetPositionFromCenter(flame->GetPositionFromCenter().x + 10, flame->GetPositionFromCenter().y);
        }
        else
        {
          pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y - 10);
          flame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y + 10);
        }
        _pGame->AddSprite(pFlame);
      }

      if (gust->GetVelocity().x == 0)
      {
        flame->SetVelocity(gust->GetVelocity().x - ((gust->GetPositionFromCenter().x - flame->GetPositionFromCenter().x) / 6), (gust->GetVelocity().y / 2) + flame->GetVelocity().y);
      }
      else
      {
        flame->SetVelocity((gust->GetVelocity().x / 2) + flame->GetVelocity().x, gust->GetVelocity().y - ((gust->GetPositionFromCenter().y - flame->GetPositionFromCenter().y) / 6));
      }
      flame->SetTime((rand() % 40) + 80);
      return false;
    }
    Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
    if (rock)
    {
      if (gust->GetVelocity().x == 0)
      {
        rock->SetVelocity(gust->GetVelocity().x - ((gust->GetPositionFromCenter().x - rock->GetPositionFromCenter().x) / 6), ((gust->GetVelocity().y) / 2) + rock->GetVelocity().y);
      }
      else
      {
        rock->SetVelocity(((gust->GetVelocity().x) / 2) + rock->GetVelocity().x, gust->GetVelocity().y - ((gust->GetPositionFromCenter().y - rock->GetPositionFromCenter().y) / 6));
      }
      return false;
    }
    Puddle* puddle = dynamic_cast<Puddle*>(pSpriteHittee);
    if (puddle)
    {
      Ice* ice = new Ice(_pIceSpriteBitmap, _pLevel);
      ice->SetPositionFromCenter(puddle->GetPositionFromCenter());
      puddle->Kill();
      _pGame->AddSprite(ice);
      return false;
    }
  }
}

//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------

void EnemySpawnRoutine(HDC hDC)
{
  // Resources
  Bitmap* bmpSkullLeft = new Bitmap(hDC, IDB_SKULLLEFT, _hInstance);
  Bitmap* bmpSkullRight = new Bitmap(hDC, IDB_SKULLRIGHT, _hInstance);

  Bitmap* bmpEyeLeft = new Bitmap(hDC, IDB_EYELEFT, _hInstance);
  Bitmap* bmpEyeRight = new Bitmap(hDC, IDB_EYERIGHT, _hInstance);
  Bitmap* bmpEyeBackLeft = new Bitmap(hDC, IDB_EYEBACKLEFT, _hInstance);
  Bitmap* bmpEyeBackRight = new Bitmap(hDC, IDB_EYEBACKLEFT, _hInstance);
  Bitmap* bmpEyeBullet = new Bitmap(hDC, IDB_EYEBULLET, _hInstance);
  Enemy::SetBulletBitmap(bmpEyeBullet);
  Bitmap* bmpSlime = new Bitmap(hDC, IDB_SLIME, _hInstance);

  // Set Random device
  std::random_device rd;
  std::mt19937 gen(rd());

  // All types should be spawned
  bool fireSkullSpawned = false;
  bool deadEyeSpawned = false;
  bool greenBlobSpawned = false;
  bool humongusSpawned = false;
  for (int i = 0; i < 8; i++)
  {
    std::uniform_int_distribution<> dis(0, 3072);

    // Select enemy type randomly
    EnemyType type;

    // If all types are spawned, spawn random
    if (!fireSkullSpawned)
    {
      type = EnemyType::FIRE_SKULL;
      fireSkullSpawned = true;
    }
    else if (!deadEyeSpawned)
    {
      type = EnemyType::DEAD_EYE;
      deadEyeSpawned = true;
    }
    else if (!greenBlobSpawned)
    {
      type = EnemyType::GREEN_BLOB;
      greenBlobSpawned = true;
    }
    else if (!humongusSpawned)
    {
      type = EnemyType::HUMONGUS;
      humongusSpawned = true;
    }
    else
    {
      type = { static_cast<EnemyType>(dis(gen) % 4) };
    }

    Enemy* enemy;

    if (type == EnemyType::FIRE_SKULL)
    {
      enemy = new Enemy(
        bmpSkullLeft, _pLevel, type, _pPlayer
      );

      enemy->LinkBitmapToState(FireSkullState::FIRE_SKULL_LEFT, bmpSkullLeft);
      enemy->LinkBitmapToState(FireSkullState::FIRE_SKULL_RIGHT, bmpSkullRight);
      enemy->SetZOrder(7);
      enemy->SetNumFrames(4);
      enemy->SetFrameDelay(10);
    }
    else if (type == EnemyType::DEAD_EYE)
    {
      enemy = enemy = new Enemy(
        bmpEyeLeft, _pLevel, type, _pPlayer
      );

      enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_LEFT, bmpEyeLeft);
      enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_RIGHT, bmpEyeRight);
      enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_BACK_LEFT, bmpEyeBackLeft);
      enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_BACK_RIGHT, bmpEyeBackRight);
      enemy->SetZOrder(7);
      enemy->SetNumFrames(3);
      enemy->SetFrameDelay(10);
    }
    else if (type == EnemyType::GREEN_BLOB)
    {
      enemy = enemy = new Enemy(
        bmpSlime, _pLevel, type, _pPlayer
      );

      enemy->SetZOrder(7);
      enemy->SetNumFrames(4);
      enemy->SetFrameDelay(10);
    }
    else
    {
      enemy = new Enemy(
        new Bitmap(hDC, 56, 56, RGB(255, 0, 0)), _pLevel, type, _pPlayer
      );

      enemy->SetZOrder(7);
    }

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

}

char PlayerDirectionUpdateRoutine()
{
  POINT ptVelocity = _pPlayer->GetVelocity();
  // Get direction
  char direction = _lastDirection;
  if (ptVelocity.y > ptVelocity.x && ptVelocity.y > -ptVelocity.x)
  {
    //DOWN
    direction = 'D';
    _pPlayer->SetState(PLR_DOWN);
  }
  else if (ptVelocity.y > ptVelocity.x && ptVelocity.y < -ptVelocity.x)
  {
    //LEFT
    direction = 'L';
    _pPlayer->SetState(PLR_LEFT);
  }
  else if (ptVelocity.y < ptVelocity.x && ptVelocity.y > -ptVelocity.x)
  {
    //RIGHT
    direction = 'R';
    _pPlayer->SetState(PLR_RIGHT);
  }
  else if (ptVelocity.y < ptVelocity.x && ptVelocity.y < -ptVelocity.x)
  {
    //UP
    direction = 'U';
    _pPlayer->SetState(PLR_UP);
  }
  _lastDirection = direction;

  return direction;
}

void SwingCombined(POINT targetPos, char direction)
{
  POINT ptPlayerPos = _pPlayer->GetPositionFromCenter();

  Swing* pSwingSprite;
  if (direction == 'D')
  {
    pSwingSprite = new Swing(_pSwingDownBitmap, _pLevel, POINT{ 0,1 });
    pSwingSprite->SetNumFrames(3);
    pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x, ptPlayerPos.y + 24 });
    //DOWN
  }
  else if (direction == 'L')
  {
    pSwingSprite = new Swing(_pSwingLeftBitmap, _pLevel, POINT{ -1,0 });
    pSwingSprite->SetNumFrames(3);
    pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x - 24, ptPlayerPos.y });
    //LEFT
  }
  else if (direction == 'R')
  {
    pSwingSprite = new Swing(_pSwingRightBitmap, _pLevel, POINT{ 1,0 });
    pSwingSprite->SetNumFrames(3);
    pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x + 24, ptPlayerPos.y });
    //RIGHT
  }
  else
  {
    pSwingSprite = new Swing(_pSwingUpBitmap, _pLevel, POINT{ 0,-1 });
    pSwingSprite->SetNumFrames(3);
    pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x, ptPlayerPos.y - 24 });
    //UP
  }

  // Commons
  
  pSwingSprite->SetFrameDelay(1);
  _pGame->AddSprite(pSwingSprite);
}

void ElementUseCombined(POINT targetPos, char direction)
{
  POINT ptPlayerPos = _pPlayer->GetPositionFromCenter();

  if (iSelect == 0)
  {
    Rock* pRock = new Rock(_pRockBitmap, _pLevel);
    pRock->SetZOrder(8);

    POINT rockPos = ptPlayerPos;
    if (direction == 'D')
    {
      rockPos.y += _pPlayer->GetHeight();
      //DOWN
    }
    else if (direction == 'L')
    {
      rockPos.x -= _pPlayer->GetWidth();
      //LEFT
    }
    else if (direction == 'R')
    {
      rockPos.x += _pPlayer->GetWidth();
      //RIGHT
    }
    else
    {
      rockPos.y -= _pPlayer->GetHeight();
      //UP
    }

    pRock->SetPositionFromCenter(rockPos);

    // If rock's position is impossible, place under player
    if (pRock->AmIStuck())
    {
      pRock->SetPositionFromCenter(ptPlayerPos);
    }

    _pGame->AddSprite(pRock);
  }
  if (iSelect == 1)
  {
    Fireball* pFireball = new Fireball(_pFireballBitmap, _pLevel);
    pFireball->SetNumFrames(3);
    pFireball->SetFrameDelay(3);

    POINT fireballPos = ptPlayerPos;
    if (direction == 'D')
    {
      fireballPos.y += _pPlayer->GetHeight();
      pFireball->SetVelocity(POINT{ 0, 15 });
      //DOWN
    }
    else if (direction == 'L')
    {
      fireballPos.x -= _pPlayer->GetWidth();
      pFireball->SetVelocity(POINT{ -15, 0 });
      //LEFT
    }
    else if (direction == 'R')
    {
      fireballPos.x += _pPlayer->GetWidth();
      pFireball->SetVelocity(POINT{ 15, 0 });
      //RIGHT
    }
    else
    {
      fireballPos.y -= _pPlayer->GetHeight();
      pFireball->SetVelocity(POINT{ 0, -15 });
      //UP
    }

    pFireball->SetPositionFromCenter(fireballPos);

    // If fireball's position is impossible, place under player
    if (pFireball->AmIStuck())
    {
      pFireball->SetPositionFromCenter(ptPlayerPos);
    }

    _pGame->AddSprite(pFireball);
  }
  if (iSelect == 2)
  {
    Puddle* pPuddle = new Puddle(_pWaterBitmap, _pLevel);
    pPuddle->SetPosition((_pPlayer->GetPositionFromCenter().x / 32) * 32, (_pPlayer->GetPositionFromCenter().y / 32) * 32);
    _pGame->AddSprite(pPuddle);
  }
  if (iSelect == 3)
  {
    Gust* pGust;
    POINT ptGustVelocity;

    if (direction == 'D')
    {
      pGust = new Gust(_pGustDownBitmap, _pLevel);
      ptGustVelocity = POINT{ 0, 8 };
      //DOWN
    }
    else if (direction == 'L')
    {
      pGust = new Gust(_pGustLeftBitmap, _pLevel);
      ptGustVelocity = POINT{ -8, 0 };
      //LEFT
    }
    else if (direction == 'R')
    {
      pGust = new Gust(_pGustRightBitmap, _pLevel);
      ptGustVelocity = POINT{ 8, 0 };
      //RIGHT
    }
    else
    {
      pGust = new Gust(_pGustUpBitmap, _pLevel);
      ptGustVelocity = POINT{ 0, -8 };
      //UP
    }

    pGust->SetVelocity(ptGustVelocity);
    pGust->SetBoundsAction(BA_DIE);
    POINT ptPlayerVelocity = _pPlayer->GetVelocity();
    _pPlayer->SetVelocity(POINT{ ptPlayerVelocity.x - (ptGustVelocity.x * 2),ptPlayerVelocity.y - (ptGustVelocity.y * 2) });

    pGust->SetPositionFromCenter(ptPlayerPos);
    _pGame->AddSprite(pGust);
  }
}
