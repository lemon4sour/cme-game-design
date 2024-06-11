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
		TEXT("Space Out"), IDI_SPACEOUT, IDI_SPACEOUT_SM, 768, 768);
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

	// Get DC
	HDC hDC = GetDC(hWindow);

	// Create and load the bitmaps
	InitializeResources(hDC);

	// Level Creation

	_pLevel = new Level(32, 1, _iCurrentLevel);
	_pLevel->MapTile(0, _pEmptyBitmap);
	_pLevel->GetTile(0)->SetCollidable();
	_pLevel->MapTile(1, _pWallBitmap);
	_pLevel->MapTile(2, _pIceBitmap);
	_pLevel->GetTile(2)->SetMeltable();

	// Player creation
	CreatePlayer(hDC);

	// Enemy creation
	CreateEnemies(hDC);

	// Inventory creation
	CreateInventory(hDC);

	// Play the background music
	_pGame->PlayMIDISong(TEXT("assets/sfx/Music.mid"));
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

	// Paint the GUI
	PrintTime(hDC);
	PrintLevel(hDC, _pLevel->GetCurrentLevel());
	_pInventory->Draw(hDC);

	// Draw the sprites
	_pGame->DrawSprites(hDC);

	RECT playerPos = _pPlayer->GetPosition();

	int centerX = (playerPos.left + playerPos.right) / 2;

	POINT pHealthBarPos = POINT{ centerX - 16, playerPos.top - 40 };

	float percentage = (float) _pPlayer->GetCurrentHealth() / (float)100;

	if (percentage > 0.75)
	{
		_pHealthBar100Bitmap->Draw(hDC, pHealthBarPos.x, pHealthBarPos.y, RGB(255, 0, 255));
	}
	else if (percentage > 0.5)
	{
		_pHealthBar75Bitmap->Draw(hDC, pHealthBarPos.x, pHealthBarPos.y, RGB(255, 0, 255));
	}
	else if (percentage > 0.25)
	{
		_pHealthBar50Bitmap->Draw(hDC, pHealthBarPos.x, pHealthBarPos.y, RGB(255, 0, 255));
	}
	else if (percentage > 0)
	{
		_pHealthBar25Bitmap->Draw(hDC, pHealthBarPos.x, pHealthBarPos.y, RGB(255, 0, 255));
	}
	else{
		//TODO game end
	}

	// Draw healthbars
	for (Sprite* sprite : _pGame->GetSprites())
	{
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);

		if (enemy)
		{
			RECT enemyPos = enemy->GetPosition();

			int centerX = (enemyPos.left + enemyPos.right) / 2;

			POINT healthBarPos = POINT{ centerX - 16, enemyPos.top - 40 };

			float percentage = enemy->GetHealth() / (float)enemy->GetMaxHealth();

			if (percentage > 0.75)
			{
				_pHealthBar100Bitmap->Draw(hDC, healthBarPos.x, healthBarPos.y, RGB(255, 0, 255));
			}
			else if (percentage > 0.5)
			{
				_pHealthBar75Bitmap->Draw(hDC, healthBarPos.x, healthBarPos.y, RGB(255, 0, 255));
			}
			else if (percentage > 0.25)
			{
				_pHealthBar50Bitmap->Draw(hDC, healthBarPos.x, healthBarPos.y, RGB(255, 0, 255));
			}
			else
			{
				_pHealthBar25Bitmap->Draw(hDC, healthBarPos.x, healthBarPos.y, RGB(255, 0, 255));
			}
		}
	}

	
}

void GameCycle()
{
	_iGameTime++;

	// Update the sprites
	_pGame->UpdateSprites();

	_pGame->InsertSpritesFromBuffer();

	// Obtain a device context for repainting the game
	HWND  hWindow = _pGame->GetWindow();
	HDC   hDC = GetDC(hWindow);

	if (_pPlayer->GetInvFrames() > 0) {
		if (_iGameTime % 3 == 0) {
			_pPlayer->SetHidden(true);
		}
		else {
			_pPlayer->SetHidden(false);
		}
	}
	else {
		_pPlayer->SetHidden(false);
	}
 
	// Paint the game to the offscreen device context
	GamePaint(_hOffscreenDC);

	if (Enemy::iEnemyCount <= 0 && !_bLevelClear) {
		PlaySound("assets/sfx/level_cleared.wav", NULL, SND_FILENAME | SND_ASYNC);
		_pGame->PauseMIDISong();
		_iBreatherTime = 100;
		_bLevelClear = true;
	}

	if (_bLevelClear) {
		_iBreatherTime--;

		if (_iBreatherTime < 0) {
			NextLevel(GetDC(_pGame->GetWindow()));
			_bLevelClear = false;
			_pGame->PlayMIDISong(TEXT("assets/sfx/Music.mid"));
		}
	}

	// Blit the offscreen bitmap to the game screen
	BitBlt(hDC, 0, 0, _pGame->GetWidth(), _pGame->GetHeight(),
		_hOffscreenDC, 0, 0, SRCCOPY);

	//Spawn Random Orbs
	SpawnOrb();
	

	// Cleanup
	ReleaseDC(hWindow, hDC);
}

static bool swingKeyPressed = false;
static bool abilityKeyPressed = false;
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

	if (GetAsyncKeyState('1') < 0)
	{
		_pInventory->SetISelect(0);
	}
	if (GetAsyncKeyState('2') < 0)
	{
		_pInventory->SetISelect(1);
	}
	if (GetAsyncKeyState('3') < 0)
	{
		_pInventory->SetISelect(2);
	}
	if (GetAsyncKeyState('4') < 0)
	{
		_pInventory->SetISelect(3);
	}

	// Swings to direction character going
	if (GetAsyncKeyState('M') < 0)
	{
		if (!swingKeyPressed)
		{
			PlaySound("assets/sfx/slash.wav",NULL, SND_FILENAME | SND_ASYNC);
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

	// Element use
	if (GetAsyncKeyState('K') < 0)
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
	/*
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
  */
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
					enemy->SetVelocity((enemy->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) / 3 + (rand() % 5) - 2, swing->GetDirection().y * 20);
				}
				else
				{
					enemy->SetVelocity(swing->GetDirection().x * 20, (enemy->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) / 3 + (rand() % 5) - 2);
				}
			}
			enemy->DealDamage(35);
			return false;
		}
		Rock* rock = dynamic_cast<Rock*>(pSpriteHittee);
		// SWING TO ROCK
		if (rock)
		{
			if (rock->GetCooldown() > 0) return false;
			rock->SetCooldown(3);

			if (swing->GetDirection().x == 0)
			{
				rock->SetVelocity((rock->GetPositionFromCenter().x - swing->GetPositionFromCenter().x) + (rand() % 5) - 2, swing->GetDirection().y * 30);
			}
			else
			{
				rock->SetVelocity(swing->GetDirection().x * 30, (rock->GetPositionFromCenter().y - swing->GetPositionFromCenter().y) + (rand() % 5) - 2);
			}
			PlaySound("assets/sfx/earth_hit.wav", NULL, SND_FILENAME | SND_ASYNC);
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
				if (enemy->GetDamageCooldown() <= 0)
				{
					int hits = rock->GetNumHits();
					if (hits >= rock->GetMaxHits())
					{
						rock->Kill();
					}
					rock->SetNumHits(hits + ((abs(rock->GetVelocity().x) + abs(rock->GetVelocity().y)) * 2));
				}

				enemy->DealDamage((abs(rock->GetVelocity().x) + abs(rock->GetVelocity().y)) * 2);

			}
			return false;
		}

		Player* player = dynamic_cast<Player*>(pSpriteHittee);
		if (player)
		{
			rock->SetVelocity(
				rock->GetVelocity().x +
				(rock->GetPositionFromCenter().x
					- player->GetPositionFromCenter().x) / 8,
				rock->GetVelocity().y +
				(rock->GetPositionFromCenter().y
					- player->GetPositionFromCenter().y) / 8
			);
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
			else fireball->Kill();
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

	Enemy* enemy = dynamic_cast<Enemy*>(pSpriteHitter);
	// ENEMY INTERACTION
	if (enemy)
	{
		// Blobs should collide and push each other.
		if (enemy->GetEnemyType() == EnemyType::GREEN_BLOB)
		{
			Enemy* enemy2 = dynamic_cast<Enemy*>(pSpriteHittee);
			if (enemy2)
			{
				if ((enemy2->GetPositionFromCenter().x == enemy->GetPositionFromCenter().x) && (enemy->GetPositionFromCenter().y == enemy2->GetPositionFromCenter().y))
				{
					enemy->SetPosition(enemy->GetPositionFromCenter().x, enemy->GetPositionFromCenter().y + 1);
				}

				if (enemy->GetDamageCooldown() <= 0)
				{
					enemy->SetVelocity(
						enemy->GetVelocity().x +
						(enemy->GetPositionFromCenter().x
							- enemy2->GetPositionFromCenter().x) / 8,
						enemy->GetVelocity().y +
						(enemy->GetPositionFromCenter().y
							- enemy2->GetPositionFromCenter().y) / 8
					);
				}
				return false;
			}
		}
	}

	Orb* orb = dynamic_cast<Orb*>(pSpriteHitter);
	// ORB INTERACTION
	if (orb) {
		// ORB TO PLAYER
		Player* player = dynamic_cast<Player*>(pSpriteHittee);
		if (player) {
			orb->Kill();
			OrbType type = orb->GetType();
			if (type == ORB_HEALTH) {
				player->AddHealth(10);
			}
			else if (type == ORB_EARTH) {
				_pInventory->AddElement(Earth);
			}
			else if (type == ORB_WATER) {
				_pInventory->AddElement(Water);
			}
		}
	}
}

//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------

void InitializeResources(HDC hDC)
{
	// Level resources
	_pEmptyBitmap = new Bitmap(hDC, IDB_EMPTY, _hInstance);
	_pWallBitmap = new Bitmap(hDC, IDB_WALL, _hInstance);

	// Element icon resources
	_pEarthResBitmap = new Bitmap(hDC, IDB_EARTHRES, _hInstance);
	_pFireResBitmap = new Bitmap(hDC, IDB_FIRERES, _hInstance);
	_pWaterResBitmap = new Bitmap(hDC, IDB_WATERRES, _hInstance);
	_pAirResBitmap = new Bitmap(hDC, IDB_AIRRES, _hInstance);

	// Pointer resources (Finger)
	_pPointBitmap = new Bitmap(hDC, IDB_POINT, _hInstance);

	// Health bar resources
	_pHealthBar100Bitmap = new Bitmap(hDC, IDB_HEALTHBAR100, _hInstance);
	_pHealthBar75Bitmap = new Bitmap(hDC, IDB_HEALTHBAR075, _hInstance);
	_pHealthBar50Bitmap = new Bitmap(hDC, IDB_HEALTHBAR050, _hInstance);
	_pHealthBar25Bitmap = new Bitmap(hDC, IDB_HEALTHBAR025, _hInstance);

	// Element resources
	_pIceBitmap = new Bitmap(hDC, IDB_ICE, _hInstance);
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

	// Swing resources
	_pSwingLeftBitmap = new Bitmap(hDC, IDB_SWINGLEFT, _hInstance);
	_pSwingRightBitmap = new Bitmap(hDC, IDB_SWINGRIGHT, _hInstance);
	_pSwingUpBitmap = new Bitmap(hDC, IDB_SWINGUP, _hInstance);
	_pSwingDownBitmap = new Bitmap(hDC, IDB_SWINGDOWN, _hInstance);

	// Player resources
	_pPlayerDownBitmap = new Bitmap(hDC, IDB_PLAYERMOVEDOWN, _hInstance);
	_pPlayerUpBitmap = new Bitmap(hDC, IDB_PLAYERMOVEUP, _hInstance);
	_pPlayerLeftBitmap = new Bitmap(hDC, IDB_PLAYERMOVELEFT, _hInstance);
	_pPlayerRightBitmap = new Bitmap(hDC, IDB_PLAYERMOVERIGHT, _hInstance);

	// Enemy resources
	_pSkullLeftBitmap = new Bitmap(hDC, IDB_SKULLLEFT, _hInstance);
	_pSkullRightBitmap = new Bitmap(hDC, IDB_SKULLRIGHT, _hInstance);
	_pEyeLeftBitmap = new Bitmap(hDC, IDB_EYELEFT, _hInstance);
	_pEyeRightBitmap = new Bitmap(hDC, IDB_EYERIGHT, _hInstance);
	_pEyeBackLeftBitmap = new Bitmap(hDC, IDB_EYEBACKLEFT, _hInstance);
	_pEyeBackRightBitmap = new Bitmap(hDC, IDB_EYEBACKLEFT, _hInstance);
	_pEyeBulletBitmap = new Bitmap(hDC, IDB_EYEBULLET, _hInstance);
	Enemy::SetBulletBitmap(_pEyeBulletBitmap);
	_pSlimeBitmap = new Bitmap(hDC, IDB_SLIME, _hInstance);
	_pHumongousFrontBitmap = new Bitmap(hDC, IDB_HUMUNGOUSFRONT, _hInstance);

	_pOrbHealthBitmap = new Bitmap(hDC, IDB_ORBHEALTH, _hInstance);
	_pOrbFireBitmap = new Bitmap(hDC, IDB_ORBFIRE, _hInstance);
	_pOrbAirBitmap = new Bitmap(hDC, IDB_ORBAIR, _hInstance);
	_pOrbWaterBitmap = new Bitmap(hDC, IDB_ORBWATER, _hInstance);
	_pOrbEarthBitmap = new Bitmap(hDC, IDB_ORBEARTH, _hInstance);
}

void CreatePlayer(HDC hDC)
{
	_pPlayer = new Player(_pPlayerRightBitmap, _pLevel);

	// Link bitmaps to player states
	_pPlayer->LinkBitmapToState(PLR_DOWN, _pPlayerDownBitmap);
	_pPlayer->LinkBitmapToState(PLR_UP, _pPlayerUpBitmap);
	_pPlayer->LinkBitmapToState(PLR_LEFT, _pPlayerLeftBitmap);
	_pPlayer->LinkBitmapToState(PLR_RIGHT, _pPlayerRightBitmap);

	// Set player properties
	_pPlayer->SetState(PLR_RIGHT);
	_pPlayer->SetNumFrames(4);
	_pPlayer->SetFrameDelay(3);
	_pPlayer->SetZOrder(10);
	_pPlayer->SetPosition(POINT{ 64,64 });
	_pGame->AddSprite(_pPlayer);
}

static std::vector<std::vector<int>> enemyMap = {
	{0, 4, 0, 2 },
	{ 2, 3, 0, 3 },
	{ 3, 2, 1, 2 },
	{ 3, 3, 1, 3 },
	{ 2, 3, 3, 2 }
};

void CreateEnemies(HDC hDC)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 3072);

	for (int i = 0; i < enemyMap[_iCurrentLevel -1].size(); i++)
	{
		for (int j = 0; j < enemyMap[_iCurrentLevel -1][i]; j++)
		{
			Enemy* enemy = CreateEnemy(static_cast<EnemyType>(i));

			bool iAmStuck = true;

			
			// Random position between 0 and 1024
			enemy->SetPosition(POINT{ dis(gen), dis(gen) });

			while (enemy->AmIStuck())
			{
				enemy->SetPosition(POINT{ dis(gen), dis(gen) });
			}
			
			_pGame->AddSprite(enemy);
			_vEnemies.push_back(enemy);
			
			
		}
	}
}

void CreateInventory(HDC hDC)
{
	_pInventory = new Inventory(hDC, _pEarthResBitmap, _pWaterResBitmap);
}


void NextLevel(HDC hDC)
{
	// Clear current level
	ClearBeforeNextLevel();

	// Level Creation
	_iCurrentLevel++;
	_pLevel = new Level(32, 1, _iCurrentLevel);
	_pLevel->MapTile(0, _pEmptyBitmap);
	_pLevel->GetTile(0)->SetCollidable();
	_pLevel->MapTile(1, _pWallBitmap);
	_pLevel->MapTile(2, _pIceBitmap);
	_pLevel->GetTile(2)->SetMeltable();
	
	// Player creation
	CreatePlayer(hDC);

	// Enemy creation
	CreateEnemies(hDC);

	CreateInventory(hDC);
}

Orb* CreateRandomOrb() {
	// Set Random device
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 3072);

	OrbType type = { static_cast<OrbType>(dis(gen) % 3) };

	return CreateOrb(type);
}

Orb* CreateOrb(OrbType type) {
	Orb* orb;
	if (type == OrbType::ORB_HEALTH) {
		orb = new Orb(_pOrbHealthBitmap, _pLevel, type);
	}
	else if (type == OrbType::ORB_EARTH) {
		orb = new Orb(_pOrbEarthBitmap, _pLevel, type);
	}
	else if (type == OrbType::ORB_WATER) {
		orb = new Orb(_pOrbWaterBitmap, _pLevel, type);
	}
	orb->SetZOrder(6);
	_pGame->AddSprite(orb);
	return orb;
}

void SpawnOrb() {
	static DWORD currentTime = GetTickCount();

	if (GetTickCount() - currentTime >= 2000 && _iCurrentOrbNumber < _iMaxOrbNumber) {
		currentTime = GetTickCount();
		Orb* orb = CreateRandomOrb();
		orb->SetPositionFromCenter((32 + rand()) % 736, (32 + rand()) % 736);
		while (orb->AmIStuck()) {
			orb->SetPositionFromCenter((32 + rand()) % 736, (32 + rand()) % 736);
		}
		_iCurrentOrbNumber++;
	}
}


char PlayerDirectionUpdateRoutine()
{
	POINT ptVelocity = _pPlayer->GetTargetVelocity();
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
		pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x, ptPlayerPos.y + 36 });
		//DOWN
	}
	else if (direction == 'L')
	{
		pSwingSprite = new Swing(_pSwingLeftBitmap, _pLevel, POINT{ -1,0 });
		pSwingSprite->SetNumFrames(3);
		pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x - 36, ptPlayerPos.y });
		//LEFT
	}
	else if (direction == 'R')
	{
		pSwingSprite = new Swing(_pSwingRightBitmap, _pLevel, POINT{ 1,0 });
		pSwingSprite->SetNumFrames(3);
		pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x + 36, ptPlayerPos.y });
		//RIGHT
	}
	else
	{
		pSwingSprite = new Swing(_pSwingUpBitmap, _pLevel, POINT{ 0,-1 });
		pSwingSprite->SetNumFrames(3);
		pSwingSprite->SetPositionFromCenter(POINT{ ptPlayerPos.x, ptPlayerPos.y - 36 });
		//UP
	}

	// Commons

	pSwingSprite->SetFrameDelay(1);
	_pGame->AddSprite(pSwingSprite);
}

void ElementUseCombined(POINT targetPos, char direction)
{
	POINT ptPlayerPos = _pPlayer->GetPositionFromCenter();

	if (_pInventory->GetISelect() == 0 && _pInventory->UseElement(0))
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

		PlaySound("assets/sfx/earth.wav", NULL, SND_FILENAME | SND_ASYNC);
		_pGame->AddSprite(pRock);
	}

	if (_pInventory->GetISelect() == 1 && _pInventory->UseElement(1))
	{
		Ice* ice = new Ice(_pIceSpriteBitmap, _pLevel);
		POINT icePos = { (ptPlayerPos.x / 32) * 32, (ptPlayerPos.y / 32) * 32 };
		switch (direction)
		{
			case 'L':
				if (icePos.x - 32 >= 32)  // Corrected comparison operator
					icePos.x -= 32;
				else
					icePos.x += 32;
				break;
			case 'R':
				if (icePos.x + 32 > 736)  // Corrected comparison operator
					icePos.x += 32;
				else
					icePos.x -= 32;
				break;
			case 'U':
				if (icePos.y - 32 >= 32)  // Corrected comparison operator
					icePos.y -= 32;
				else
					icePos.y += 32;
				break;
			case 'D':
				if (icePos.y + 32 > 736)  // Corrected comparison operator
					icePos.y += 32;
				else
					icePos.y -= 32;
				break;
		}
		ice->SetPositionFromCenter(icePos);
		
		PlaySound("assets/sfx/ice.wav", NULL, SND_FILENAME | SND_ASYNC);
		_pGame->AddSprite(ice);
	}

	//FIRE
	//if (_pInventory->GetISelect() == 2 && _pInventory->UseElement(2))
	//{
	//	Fireball* pFireball = new Fireball(_pFireballBitmap, _pLevel);
	//	pFireball->SetNumFrames(3);
	//	pFireball->SetFrameDelay(3);

	//	POINT fireballPos = ptPlayerPos;
	//	if (direction == 'D')
	//	{
	//		fireballPos.y += _pPlayer->GetHeight();
	//		pFireball->SetVelocity(POINT{ 0, 15 });
	//		//DOWN
	//	}
	//	else if (direction == 'L')
	//	{
	//		fireballPos.x -= _pPlayer->GetWidth();
	//		pFireball->SetVelocity(POINT{ -15, 0 });
	//		//LEFT
	//	}
	//	else if (direction == 'R')
	//	{
	//		fireballPos.x += _pPlayer->GetWidth();
	//		pFireball->SetVelocity(POINT{ 15, 0 });
	//		//RIGHT
	//	}
	//	else
	//	{
	//		fireballPos.y -= _pPlayer->GetHeight();
	//		pFireball->SetVelocity(POINT{ 0, -15 });
	//		//UP
	//	}

	//	pFireball->SetPositionFromCenter(fireballPos);

	//	// If fireball's position is impossible, place under player
	//	if (pFireball->AmIStuck())
	//	{
	//		pFireball->SetPositionFromCenter(ptPlayerPos);
	//	}

	//	_pGame->AddSprite(pFireball);
	//}
	//AIR
	//if (_pInventory->GetISelect() == 3 && _pInventory->UseElement(3))
	//{
	//	Gust* pGust;
	//	POINT ptGustVelocity;
	//	SwingCombined(_pPlayer->GetVelocity(), 'D');
	//	SwingCombined(_pPlayer->GetVelocity(), 'L');
	//	SwingCombined(_pPlayer->GetVelocity(), 'U');
	//	SwingCombined(_pPlayer->GetVelocity(), 'R');

	//	if (direction == 'D')
	//	{
	//		pGust = new Gust(_pGustDownBitmap, _pLevel);
	//		ptGustVelocity = POINT{ 0, 8 };
	//		//DOWN
	//	}
	//	else if (direction == 'L')
	//	{
	//		pGust = new Gust(_pGustLeftBitmap, _pLevel);
	//		ptGustVelocity = POINT{ -8, 0 };
	//		//LEFT
	//	}
	//	else if (direction == 'R')
	//	{
	//		pGust = new Gust(_pGustRightBitmap, _pLevel);
	//		ptGustVelocity = POINT{ 8, 0 };
	//		//RIGHT
	//	}
	//	else
	//	{
	//		pGust = new Gust(_pGustUpBitmap, _pLevel);
	//		ptGustVelocity = POINT{ 0, -8 };
	//		//UP
	//	}

	//	pGust->SetVelocity(ptGustVelocity);
	//	pGust->SetBoundsAction(BA_DIE);
	//	POINT ptPlayerVelocity = _pPlayer->GetVelocity();
	//	_pPlayer->SetVelocity(POINT{ ptPlayerVelocity.x - (ptGustVelocity.x * 2),ptPlayerVelocity.y - (ptGustVelocity.y * 2) });

	//	pGust->SetPositionFromCenter(ptPlayerPos);
	//	_pGame->AddSprite(pGust);
	//}
}


// INNER FUNCTIONS
void ClearBeforeNextLevel()
{
	Enemy::iEnemyCount = 0;
	_pGame->CleanupSprites();
}

Enemy* CreateEnemy(EnemyType type)
{
	// Resources

	Enemy* enemy;

	if (type == EnemyType::FIRE_SKULL)
	{
		enemy = new Enemy(
			_pSkullLeftBitmap, _pLevel, type, _pPlayer
		);

		enemy->LinkBitmapToState(FireSkullState::FIRE_SKULL_LEFT, _pSkullLeftBitmap);
		enemy->LinkBitmapToState(FireSkullState::FIRE_SKULL_RIGHT, _pSkullRightBitmap);

	}
	else if (type == EnemyType::DEAD_EYE)
	{
		enemy = enemy = new Enemy(
			_pEyeLeftBitmap, _pLevel, type, _pPlayer
		);

		enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_LEFT, _pEyeLeftBitmap);
		enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_RIGHT, _pEyeRightBitmap);
		enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_BACK_LEFT, _pEyeBackLeftBitmap);
		enemy->LinkBitmapToState(GreenBlobState::GREEN_BLOB_BACK_RIGHT, _pEyeBackRightBitmap);

	}
	else if (type == EnemyType::GREEN_BLOB)
	{
		enemy = enemy = new Enemy(
			_pSlimeBitmap, _pLevel, type, _pPlayer
		);
	}
	else
	{
		enemy = new Enemy(
			_pHumongousFrontBitmap, _pLevel, type, _pPlayer

		);

		enemy->SetZOrder(7);
	}

	return enemy;
}
