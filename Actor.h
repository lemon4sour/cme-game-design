//-----------------------------------------------------------------
// Actor Object
// C++ Header - Actor.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Sprite.h"

//-----------------------------------------------------------------
// Actor Class
//-----------------------------------------------------------------
class Actor {
protected:
	Sprite* m_pSprite;
	Bitmap* m_pSpriteStates[256];
	int m_iState;
	int m_iSize;

public:
	void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap);
	void Actor::SetState(int iState);
	Sprite* Actor::GetSprite() { return m_pSprite; };
};

enum enumPlayer : int {
	PLR_STAND = 0,
	PLR_DOWN = 1,
	PLR_UP = 2,
	PLR_LEFT = 3,
	PLR_RIGHT = 4,
};

class Player : public Actor {
protected:
	int m_iHealth;
public:
	Player::Player(HDC hDC);
	Player::~Player();
	void Player::Move(int iDirectionX, int iDirectionY);
	void Player::SubtractHealth(int value);
	void Player::AddHealth(int value);
};