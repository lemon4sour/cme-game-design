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
#include "GameEngine.h"
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
  static GameEngine* _pGame;
  static void initializeGame(GameEngine* pGame) { _pGame = pGame; };
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

//-----------------------------------------------------------------
// Fireball Class
//-----------------------------------------------------------------
class Fireball : public Actor
{
protected:
public:
	Fireball::Fireball(Bitmap* bmpBitmap, Level* pLevel);
	SPRITEACTION Fireball::Update() override;
};

//-----------------------------------------------------------------
// Flame Class
//-----------------------------------------------------------------
class Flame : public Actor
{
protected:
	int m_iTime;
public:
	static Bitmap* m_pFlameBitmap;
	static void setFlameBitmap(Bitmap* pFlameBitmap) { m_pFlameBitmap = pFlameBitmap; };
	Flame::Flame(Bitmap* bmpBitmap, Level* pLevel);
	void Flame::UpdateVelocity();
	SPRITEACTION Flame::Update() override;
};

//-----------------------------------------------------------------
// Water Class
//-----------------------------------------------------------------
class Puddle : public Actor
{
protected:
	int m_iTime;
public:
	Puddle::Puddle(Bitmap* bmpBitmap, Level* pLevel);
	SPRITEACTION Puddle::Update() override;
};

//-----------------------------------------------------------------
// Mud Class
//-----------------------------------------------------------------
class Mud : public Actor
{
protected:
	int m_iTime;
public:
	static Bitmap* m_pMudBitmap;
	static void setMudBitmap(Bitmap* pMudBitmap) { m_pMudBitmap = pMudBitmap; };
	Mud::Mud(Bitmap* bmpBitmap, Level* pLevel);
	SPRITEACTION Mud::Update() override;
};

//-----------------------------------------------------------------
// Gust Class
//-----------------------------------------------------------------
class Gust : public Actor
{
protected:
	int m_iTime;
public:
	Gust::Gust(Bitmap* bmpBitmap, Level* pLevel);
	SPRITEACTION Gust::Update() override;
};
