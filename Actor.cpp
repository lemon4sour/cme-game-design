//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"

Actor::Actor(Bitmap* bmpBitmap, Level* pLevel) : Sprite(bmpBitmap) {
	m_pLevel = pLevel;
}

//-----------------------------------------------------------------
// Actor General Methods
//-----------------------------------------------------------------
void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap) {
	m_pSpriteStates[iState] = bmpBitmap;
}

SPRITEACTION Actor::Update() {

	POINT ptOldPosition, ptNewPosition, ptSpriteSize;
	ptOldPosition.x = m_rcPosition.left;
	ptOldPosition.y = m_rcPosition.top;

	SPRITEACTION out = Sprite::Update();

	BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
	BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
	BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
	BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

	if (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight) {
		SetPosition(ptOldPosition);
	}

	return out;
}

//-----------------------------------------------------------------
// Player Constructor(s)/Destructor
//-----------------------------------------------------------------
Player::Player(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel) {
	m_iState = 0;
	m_iSize = 24;
	m_iHealth = 100;
	
	m_pSpriteStates[0] = bmpBitmap;
}

void Player::UpdateVelocity() {
	if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
	else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

	if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
	else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}