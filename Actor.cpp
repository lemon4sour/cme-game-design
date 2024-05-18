//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"


//-----------------------------------------------------------------
// Actor General Methods
//-----------------------------------------------------------------
void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap) {
	m_pSpriteStates[iState] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player Constructor(s)/Destructor
//-----------------------------------------------------------------
Player::Player(HDC hDC) {
	m_iState = 0;
	m_iSize = 16;
	m_iHealth = 100;
	Bitmap* bitmap = new Bitmap(hDC, 16, 16, RGB(255, 255, 255));
	m_pSpriteStates[0] = bitmap;
	m_pSprite = new Sprite(bitmap);
}

Player::~Player() {
	delete m_pSprite;
}