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
  void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap);
  void Actor::SetState(int iState);
  SPRITEACTION Actor::Update() override;
};

enum enumPlayer : int
{
  PLR_STAND = 0,
  PLR_DOWN = 1,
  PLR_UP = 2,
  PLR_LEFT = 3,
  PLR_RIGHT = 4,
};

class Player : public Actor
{
protected:
  int m_iHealth;
  POINT m_ptTargetVelocity;
public:
  Player::Player(Bitmap* bmpBitmap, Level* pLevel);
  void Player::SubtractHealth(int value);
  void Player::AddHealth(int value);
  void Player::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
  POINT Player::GetTargetVelocity() { return m_ptTargetVelocity; };
  void Player::UpdateVelocity();
};

class Enemy : public Actor
{
protected:
  POINT m_ptTargetVelocity;
  int m_difficulty{ 10 };
public:
  Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel);
  void 	Enemy::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
  POINT Enemy::GetTargetVelocity() { return m_ptTargetVelocity; };
  void Enemy::Catch(Player* pPlayer);
  void 	Enemy::UpdateVelocity();
};

class Swing : public Actor
{
protected:
  int m_iActiveTime;
public:
  Swing::Swing(Bitmap* bmpBitmap, Level* pLevel);
  SPRITEACTION Swing::Update() override;
};