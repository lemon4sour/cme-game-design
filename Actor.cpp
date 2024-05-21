//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"

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
	return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
	m_iActiveTime = 3;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------
SPRITEACTION Swing::Update()
{
	SPRITEACTION out = Actor::Update();
	for (Sprite* sprite : (*_vcSprites)) {
		Enemy* enemy = dynamic_cast<Enemy*>(sprite);
		if (enemy) {
			if (enemy->TestCollision(this)) {
				enemy->Kill();
			}
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

	if (ptPlayerCenterPos.x < ptEnemyCenterPos.x)
	{
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
