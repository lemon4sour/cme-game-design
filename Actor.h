//-----------------------------------------------------------------
// Actor Object
// C++ Header - Actor.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Level.h"
#include "Sprite.h"
#include <windows.h>
#include <vector>

//-----------------------------------------------------------------
// Actor Class
//-----------------------------------------------------------------
class Actor : public Sprite
{
protected:
  Bitmap* m_pSpriteStates[256];
  int m_iState;
  int m_iSize;
  Level* m_pLevel;
  Actor::Actor(Bitmap* bmpBitmap, Level* pLevel);

public:
  static std::vector<Sprite*>* _vcSprites;
  static void initializeSprites(std::vector<Sprite*>* pSpriteVector) { _vcSprites = pSpriteVector; };
  void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap);
  void Actor::SetState(int iState) { m_iState = iState; };
  bool Actor::AmIStuck();
  SPRITEACTION Actor::Update() override;
};

// std::vector<Sprite*>* Actor::_vcSprites = nullptr;

enum enumPlayer : int
{
  PLR_STAND = 0,
  PLR_DOWN = 1,
  PLR_UP = 2,
  PLR_LEFT = 3,
  PLR_RIGHT = 4,
};

//-----------------------------------------------------------------
// Player Class
//-----------------------------------------------------------------
class Player : public Actor
{
protected:
	int m_iMaxHealth;
	int m_iCurrentHealth;
	POINT m_ptTargetVelocity;
public:
	Player::Player(Bitmap* bmpBitmap, Level* pLevel);
	void Player::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
	POINT Player::GetTargetVelocity() { return m_ptTargetVelocity; };
	void Player::UpdateVelocity();
	int Player::GetMaxHealth() { return m_iMaxHealth; };
	int Player::GetCurrentHealth() { return m_iCurrentHealth; };
	void Player::SubtractHealth(int value);
	SPRITEACTION Player::Update() override;
};

//-----------------------------------------------------------------
// Swing Class
//-----------------------------------------------------------------
class Swing : public Actor
{
protected:
	int m_iActiveTime;
public:
	Swing::Swing(Bitmap* bmpBitmap, Level* pLevel);
	SPRITEACTION Swing::Update() override;
};

//-----------------------------------------------------------------
// Enemy Class
//-----------------------------------------------------------------
class Enemy : public Actor
{
protected:
	POINT m_ptTargetVelocity;
	int m_difficulty{ 10 };
	Player* m_pTarget;
public:
	Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel);
	void 	Enemy::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
	POINT Enemy::GetTargetVelocity() { return m_ptTargetVelocity; };
	void Enemy::Catch();
	void 	Enemy::UpdateVelocity();
	void Enemy::SetTarget(Player* pActor) { m_pTarget = pActor; };
	SPRITEACTION Enemy::Update() override;
};