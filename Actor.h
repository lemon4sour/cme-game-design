//-----------------------------------------------------------------
// Actor Object
// C++ Header - Actor.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Level.h"
#include "PathFinding.hpp"
#include "Sprite.h"
#include <vector>
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
  static std::vector<Sprite*>* _vcSprites;
  static void initializeSprites(std::vector<Sprite*>* pSpriteVector) { _vcSprites = pSpriteVector; };
  void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap);
  void Actor::SetState(int iState) { m_iState = iState; SetBitmap(m_pSpriteStates[m_iState]); };
  bool Actor::AmIStuck();
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

//-----------------------------------------------------------------
// Player Class
//-----------------------------------------------------------------
class Player : public Actor
{
protected:
  int m_iMaxHealth;
  int m_iCurrentHealth;
  POINT m_ptTargetVelocity;
  POINT m_ptMousePos;
public:
  Player::Player(Bitmap* bmpBitmap, Level* pLevel);
  void Player::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
  POINT Player::GetTargetVelocity() { return m_ptTargetVelocity; };
  void Player::UpdateVelocity();
  int Player::GetMaxHealth() { return m_iMaxHealth; };
  int Player::GetCurrentHealth() { return m_iCurrentHealth; };
  void Player::SubtractHealth(int value);
  void Player::SetMousePos(int x, int y) { m_ptMousePos = POINT{ x,y }; };
  SPRITEACTION Player::Update() override;
};

//-----------------------------------------------------------------
// Swing Class
//-----------------------------------------------------------------
class Swing : public Actor
{
protected:
  int m_iActiveTime;
  POINT m_ptDirection;
public:
  Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection);
  SPRITEACTION Swing::Update() override;
};

//-----------------------------------------------------------------
// Enemy Class
//-----------------------------------------------------------------

enum class EnemyType : std::uint8_t
{
  FIRE,
  WATER,
  EARTH,
  AIR
};

class Enemy : public Actor
{
protected:
  POINT m_ptTargetVelocity;
  int m_speed;
  Player* m_pTarget;
  AStar m_aStar;
  EnemyType m_type;
public:
  Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel, EnemyType type, Player* pTarget);
  void 	Enemy::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
  POINT Enemy::GetTargetVelocity() { return m_ptTargetVelocity; };
  void Enemy::Catch();
  void 	Enemy::UpdateVelocity();
  SPRITEACTION Enemy::Update() override;
};

//-----------------------------------------------------------------
// Rock Class
//-----------------------------------------------------------------
class Rock : public Actor
{
protected:
  int m_iNumHits;
  int m_iMaxHits;
  int m_iCooldown;
public:
  Rock::Rock(Bitmap* bmpBitmap, Level* pLevel);
  void Rock::UpdateVelocity();
  void Rock::SetCooldown(int iCooldown) { m_iCooldown = iCooldown; };
  int Rock::GetCooldown() { return m_iCooldown; };
  int Rock::GetNumHits() { return m_iNumHits; };
  int Rock::GetMaxHits() { return m_iMaxHits; };
  void Rock::GetNumHits(int iNumHits) { m_iNumHits = iNumHits; };
  SPRITEACTION Rock::Update() override;
};