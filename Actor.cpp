//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"

Actor::Actor(Bitmap* bmpBitmap) : Sprite(bmpBitmap) {}

//-----------------------------------------------------------------
// Actor General Methods
//-----------------------------------------------------------------
void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap) {
	m_pSpriteStates[iState] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player Constructor(s)/Destructor
//-----------------------------------------------------------------
Player::Player(Bitmap* bmpBitmap) : Actor(bmpBitmap) {
	m_iState = 0;
	m_iSize = 24;
	m_iHealth = 100;
	
	m_pSpriteStates[0] = bmpBitmap;
}

void Player::UpdateVelocity() {
	if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 6);
	else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 6);

	if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 6);
	else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 6);
}