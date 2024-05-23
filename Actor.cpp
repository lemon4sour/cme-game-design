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
			if (rock->GetCooldown() > 0) continue;
			rock->SetCooldown(3);
			if (rock->TestCollision(this)) {
				int hits = rock->GetNumHits();
				if (hits >= rock->GetMaxHits()) {
					rock->Kill();
					continue;
				}
				rock->GetNumHits(++hits);

				if (m_ptDirection.x == 0) {
					rock->SetVelocity((rock->GetPositionFromCenter().x - GetPositionFromCenter().x) + (rand() % 17) - 9, m_ptDirection.y * 100);
				}
				else {
					rock->SetVelocity(m_ptDirection.x * 100, (rock->GetPositionFromCenter().y - GetPositionFromCenter().y) + (rand() % 17) - 9);
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
	}

	return out;
}


