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

void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;

  m_pSpriteStates[0] = bmpBitmap;
}


void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

	if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
	else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Player::ReduceHealth(int value) {
	if (m_iCurrentHealth > 0) {
		m_iCurrentHealth = m_iCurrentHealth - value > 0 ? m_iCurrentHealth - value: 0;
	}
}

void Enemy::Catch(Player* pPlayer)
{
	// TODO: Avoid obstacles, USE A* ALGORITHM
	POINT ptPlayerCenterPos = POINT{ (pPlayer->GetPosition().left + pPlayer->GetPosition().right) / 2, (pPlayer->GetPosition().top + pPlayer->GetPosition().bottom) / 2 };
	POINT ptEnemyCenterPos = POINT{ (m_rcPosition.left + m_rcPosition.right) / 2, (m_rcPosition.top + m_rcPosition.bottom) / 2 };

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



//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
	m_iActiveTime = 5;
}
SPRITEACTION Swing::Update()
{
	SPRITEACTION out = Actor::Update();
	if (m_iActiveTime-- <= 0)
	{
		return SA_KILL;
	}
	return out;
}