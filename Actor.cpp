//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"

GameEngine* Actor::_pGame = nullptr;

Actor::Actor(Bitmap* bmpBitmap, Level* pLevel) : Sprite(bmpBitmap)
{
  m_pLevel = pLevel;
}

//-----------------------------------------------------------------
// Actor General Methods
//-----------------------------------------------------------------
void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap)
{
  m_pSpriteStates[iState] = bmpBitmap;
}

SPRITEACTION Actor::Update()
{

  POINT ptOldPosition, ptNewPosition, ptSpriteSize;
  ptOldPosition.x = m_rcPosition.left;
  ptOldPosition.y = m_rcPosition.top;

  SPRITEACTION out = Sprite::Update();

  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

  if (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight)
  {
    SetPosition(ptOldPosition);
  }



  return out;
}

bool Actor::AmIStuck()
{
  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

  if (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------
// Player Constructor(s)/Destructor
//-----------------------------------------------------------------
Player::Player(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel) {
	m_ptTargetVelocity = POINT{ 0,0 };
	m_iState = 0;
	m_iSize = 24;
	m_iMaxHealth = 100;
	m_iCurrentHealth = m_iMaxHealth;
	
	m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player General Methods
//-----------------------------------------------------------------
void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Player::SubtractHealth(int value) {
	if (m_iCurrentHealth > 0) {
		m_iCurrentHealth = m_iCurrentHealth - value > 0 ? m_iCurrentHealth - value : 0;
	}
}

SPRITEACTION Player::Update() {
	SPRITEACTION out = Actor::Update();
	UpdateVelocity();
	POINT ptPlayerCenterPos = GetPositionFromCenter();

	POINT ptMouseOffset = POINT{ m_ptMousePos.x - ptPlayerCenterPos.x, m_ptMousePos.y - ptPlayerCenterPos.y };

	if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
	{
		SetState(PLR_DOWN);
		//DOWN
	}
	if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
	{
		SetState(PLR_LEFT);
		//LEFT
	}
	if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
	{
		SetState(PLR_RIGHT);
		//RIGHT
	}
	if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
	{
		SetState(PLR_UP);
		//UP
	}

	return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection) : Actor(bmpBitmap, pLevel)
{
	m_iActiveTime = 3;
	m_ptDirection = ptDirection;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------
SPRITEACTION Swing::Update()
{
	SPRITEACTION out = Actor::Update();
	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Rock* rock = dynamic_cast<Rock*>(sprite);
		if (rock) {
			if (rock->TestCollision(this)) {
				if (rock->GetCooldown() > 0) continue;
				rock->SetCooldown(3);
				int hits = rock->GetNumHits();
				if (hits >= rock->GetMaxHits()) {
					rock->Kill();
					continue;
				}
				rock->GetNumHits(++hits);

				if (m_ptDirection.x == 0) {
					rock->SetVelocity((rock->GetPositionFromCenter().x - GetPositionFromCenter().x) + (rand() % 17) - 9, m_ptDirection.y * 120);
				}
				else {
					rock->SetVelocity(m_ptDirection.x * 120, (rock->GetPositionFromCenter().y - GetPositionFromCenter().y) + (rand() % 17) - 9);
				}
				
			}
			continue;
		}
	}
	if (m_iActiveTime-- <= 0)
	{
		return SA_KILL;
	}
	return out;
}

//-----------------------------------------------------------------
// Enemy Constructor(s)/Destructor
//-----------------------------------------------------------------
Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_pTarget = NULL;

  m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Enemy General Methods
//-----------------------------------------------------------------
void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Enemy::Catch()
{
	if (m_pTarget == NULL) return;

	// TODO: Avoid obstacles, USE A* ALGORITHM
	POINT ptPlayerCenterPos = m_pTarget->GetPositionFromCenter();
	POINT ptEnemyCenterPos = GetPositionFromCenter();

	if (ptPlayerCenterPos.x < ptEnemyCenterPos.x) {
		m_ptTargetVelocity.x = -m_difficulty;
	}
	else if (ptPlayerCenterPos.x > ptEnemyCenterPos.x)
	{
		m_ptTargetVelocity.x = m_difficulty;
	}

	if (ptPlayerCenterPos.y < ptEnemyCenterPos.y)
	{
		m_ptTargetVelocity.y = -m_difficulty;
	}
	else if (ptPlayerCenterPos.y > ptEnemyCenterPos.y)
	{
		m_ptTargetVelocity.y = m_difficulty;
	}
}

SPRITEACTION Enemy::Update() {
	SPRITEACTION out = Actor::Update();
	Catch();
	UpdateVelocity();
	if (m_pTarget != NULL) {
		if (TestCollision(m_pTarget))
		{
			m_pTarget->SubtractHealth(1);
		}
	}
	return out;
}

//-----------------------------------------------------------------
// Rock Constructor(s)/Destructor
//-----------------------------------------------------------------
Rock::Rock(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iNumHits = 0;
	m_iMaxHits = 3;
}


//-----------------------------------------------------------------
// Rock General Methods
//-----------------------------------------------------------------
void Rock::UpdateVelocity()
{
	if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

	float multiplier = 8 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

	int decrease_x = (m_ptVelocity.x * multiplier);
	int decrease_y = (m_ptVelocity.y * multiplier);

	if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
	else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

	if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
	else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

SPRITEACTION Rock::Update() {

	SPRITEACTION out = Sprite::Update();
	UpdateVelocity();

	if (m_iCooldown > 0) m_iCooldown--;

	BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
	BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
	BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
	BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

	if ((!bTopLeft && !bTopRight) || (!bBottomLeft && !bBottomRight))
	{
		m_ptVelocity.y = -m_ptVelocity.y;
		POINT position = GetPositionFromCenter();
		position.y += m_ptVelocity.y;
		SetPositionFromCenter(position);
	}
	if ((!bTopLeft && !bBottomLeft) || (!bTopRight && !bBottomRight))
	{
		m_ptVelocity.x = -m_ptVelocity.x;
		POINT position = GetPositionFromCenter();
		position.x += m_ptVelocity.x;
		SetPositionFromCenter(position);
	}

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Puddle* puddle = dynamic_cast<Puddle*>(sprite);
		if (puddle) {
			if (puddle->TestCollision(this)) {
				Mud* _pMud = new Mud(Mud::m_pMudBitmap, m_pLevel);
				_pMud->SetPositionFromCenter(puddle->GetPositionFromCenter());
				_pGame->AddSprite(_pMud);
				puddle->Kill();

				return SA_KILL;
			}
			continue;
		}
	}

	return out;
}

//-----------------------------------------------------------------
// Fireball Constructor(s)/Destructor
//-----------------------------------------------------------------
Fireball::Fireball(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
}

Bitmap* Flame::m_pFlameBitmap = nullptr;

//-----------------------------------------------------------------
// Fireball General Methods
//-----------------------------------------------------------------
SPRITEACTION Fireball::Update() {

	SPRITEACTION out = Sprite::Update();

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Rock* rock = dynamic_cast<Rock*>(sprite);
		if (rock) {
			if (rock->TestCollision(this)) {
				rock->Kill();

				{
					Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(40 + (rand() % 20), 40 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(0 + (rand() % 20), 40 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(-60 + (rand() % 20), 40 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(-60 + (rand() % 20), 0 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(-60 + (rand() % 20), -60 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(0 + (rand() % 20), -60 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(40 + (rand() % 20), -60 + (rand() % 20));
					_pGame->AddSprite(pFlame);

					pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
					pFlame->SetNumFrames(3);
					pFlame->SetPositionFromCenter(rock->GetPositionFromCenter());
					pFlame->SetVelocity(40 + (rand() % 20), 0 + (rand() % 20));
					_pGame->AddSprite(pFlame);
				}

				return SA_KILL;
			}
			continue;
		}
		Puddle* puddle = dynamic_cast<Puddle*>(sprite);
		if (puddle) {
			if (puddle->TestCollision(this)) {
				return SA_KILL;
			}
			continue;
		}
	}

	if (AmIStuck()) {
		Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
		pFlame->SetNumFrames(3);
		pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
		pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 40) - 20, (-GetVelocity().y / 3) + (rand() % 40) - 20);
		_pGame->AddSprite(pFlame);
		pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
		pFlame->SetNumFrames(3);
		pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
		pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 40) - 20, (-GetVelocity().y / 3) + (rand() % 40) - 20);
		_pGame->AddSprite(pFlame);
		return SA_KILL;		
	}

	return out;
}

//-----------------------------------------------------------------
// Flame Constructor(s)/Destructor
//-----------------------------------------------------------------
Flame::Flame(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = (rand() % 40) + 80;
	m_bCloned = false;
	m_iCloneAgain = 0;
	m_iCloneDepth = 0;
}

//-----------------------------------------------------------------
// Flame General Methods
//-----------------------------------------------------------------
SPRITEACTION Flame::Update() {

	SPRITEACTION out = Actor::Update();
	UpdateVelocity();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	if (m_bCloned) {
		if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) {
			m_iCloneAgain++;
			if (m_iCloneAgain > 2) {
				m_bCloned = false;
				m_iCloneAgain = 0;
			}
		}
		else {
			m_iCloneAgain = 0;
		}
	}

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Puddle* puddle = dynamic_cast<Puddle*>(sprite);
		if (puddle) {
			if (puddle->TestCollision(this)) {
				return SA_KILL;
			}
			continue;
		}
	}

	return out;
}

void Flame::UpdateVelocity()
{
	if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

	float multiplier = 8 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

	int decrease_x = (m_ptVelocity.x * multiplier);
	int decrease_y = (m_ptVelocity.y * multiplier);

	if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
	else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

	if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
	else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

//-----------------------------------------------------------------
// Puddle Constructor(s)/Destructor
//-----------------------------------------------------------------
Puddle::Puddle(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 100;
}

//-----------------------------------------------------------------
// Puddle General Methods
//-----------------------------------------------------------------
SPRITEACTION Puddle::Update() {

	SPRITEACTION out = Sprite::Update();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
	}

	return out;
}

//-----------------------------------------------------------------
// Mud Constructor(s)/Destructor
//-----------------------------------------------------------------
Mud::Mud(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 200;
	m_iSpreadCooldown = 3;
}

Bitmap* Mud::m_pMudBitmap = nullptr;

//-----------------------------------------------------------------
// Mud General Methods
//-----------------------------------------------------------------
SPRITEACTION Mud::Update() {

	SPRITEACTION out = Sprite::Update();

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	bool spread = false;
	if (m_iSpreadCooldown <= 0) {
		spread = true;
	}
	else {
		m_iSpreadCooldown--;
	}

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Puddle* puddle = dynamic_cast<Puddle*>(sprite);
		if (puddle) {
			if (spread) {
				if (puddle->TestCollision(this)) {
					Mud* _pMud = new Mud(Mud::m_pMudBitmap, m_pLevel);
					_pMud->SetPositionFromCenter(puddle->GetPositionFromCenter());
					_pGame->AddSprite(_pMud);
					puddle->Kill();
				}
			}
			continue;
		}
	}

	return out;
}


//-----------------------------------------------------------------
// Gust Constructor(s)/Destructor
//-----------------------------------------------------------------
Gust::Gust(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel) {
	m_iTime = 10;
}


//-----------------------------------------------------------------
// Gust General Methods
//-----------------------------------------------------------------
SPRITEACTION Gust::Update() {

	SPRITEACTION out = Sprite::Update();

	if (!m_pLevel->IsPointCollidable(GetPositionFromCenter())) return SA_KILL;

	if (--m_iTime < 0) {
		return SA_KILL;
	}

	for (Sprite* sprite : *(_pGame->GetSpritesListPointer())) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
			continue;
		}
		Flame* flame = dynamic_cast<Flame*>(sprite);
		if (flame) {
			if (flame->TestCollision(this)) {
				if (!flame->IsCloned() && flame->GetCloneDepth() < 4) {
					
					Flame* pFlame = new Flame(flame->GetBitmap(), m_pLevel);
					flame->SetCloneDepth(flame->GetCloneDepth() + 1);
					pFlame->SetCloneDepth(flame->GetCloneDepth());
					flame->SetCloned(true);
					pFlame->SetCloned(true);
					pFlame->SetNumFrames(3);
					if (m_ptVelocity.x == 0) {
						pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x - 40, flame->GetPositionFromCenter().y);
						flame->SetPositionFromCenter(flame->GetPositionFromCenter().x + 40, flame->GetPositionFromCenter().y);
					}
					else {
						pFlame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y - 40);
						flame->SetPositionFromCenter(flame->GetPositionFromCenter().x, flame->GetPositionFromCenter().y + 40);
					}
					_pGame->AddSprite(pFlame);
				}

				if (m_ptVelocity.x == 0) {
					flame->SetVelocity(m_ptVelocity.x - ((GetPositionFromCenter().x - flame->GetPositionFromCenter().x) / 6), (m_ptVelocity.y / 2) + flame->GetVelocity().y);
				}
				else {
					flame->SetVelocity((m_ptVelocity.x / 2) + flame->GetVelocity().x, m_ptVelocity.y - ((GetPositionFromCenter().y - flame->GetPositionFromCenter().y) / 6));
				}
				flame->SetTime((rand() % 40) + 80);
			}
			continue;
		}
		Rock* rock = dynamic_cast<Rock*>(sprite);
		if (rock) {
			if (rock->TestCollision(this)) {
				if (m_ptVelocity.x == 0) {
					rock->SetVelocity(m_ptVelocity.x - ((GetPositionFromCenter().x - rock->GetPositionFromCenter().x) / 6), ((3 * m_ptVelocity.y) / 2) + rock->GetVelocity().y);
				}
				else {
					rock->SetVelocity(((3 * m_ptVelocity.x) / 2) + rock->GetVelocity().x, m_ptVelocity.y - ((GetPositionFromCenter().y - rock->GetPositionFromCenter().y) / 6));
				}
			}
			continue;
		}
	}

	return out;
}


