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

class GameEngine;

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
  static GameEngine* s_pGame;
  static void initializeGame(GameEngine* pGame) { s_pGame = pGame; };
  void Actor::LinkBitmapToState(int iState, Bitmap* bmpBitmap);
  void Actor::SetState(int iState) { m_iState = iState; SetBitmap(m_pSpriteStates[m_iState]); };
  virtual bool Actor::AmIStuck();
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
  // POINT m_ptMousePos;
  int m_iInvFrames;
  int m_iSwingCooldown;
  int m_iAbilityCooldown;
public:
  Player::Player(Bitmap* bmpBitmap, Level* pLevel);
  void Player::SetTargetVelocity(POINT ptVelocity)
  {
    m_ptTargetVelocity = ptVelocity;
  }
  POINT Player::GetTargetVelocity() { return m_ptTargetVelocity; };
  void Player::UpdateVelocity();
  int Player::GetMaxHealth() { return m_iMaxHealth; };
  int Player::GetCurrentHealth() { return m_iCurrentHealth; };
  int Player::GetInvFrames() { return m_iInvFrames; };
  void Player::SubtractHealth(int value);
  void Player::AddHealth(int value);
  // void Player::SetMousePos(int x, int y) { m_ptMousePos = POINT{ x,y }; };
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
  POINT Swing::GetDirection() { return m_ptDirection; };
  SPRITEACTION Swing::Update() override;
};

//-----------------------------------------------------------------
// Enemy Class
//-----------------------------------------------------------------

enum class EnemyType : std::uint8_t
{
  FIRE_SKULL,
  GREEN_BLOB,
  HUMONGUS,
  DEAD_EYE
};

enum class EnemyState : std::uint8_t
{
  PATROLLING,
  ATTACKING,
  ESCAPING,
};

enum FireSkullState : int
{
  FIRE_SKULL_LEFT = 1,
  FIRE_SKULL_RIGHT = 2,
};

enum GreenBlobState : int
{
  GREEN_BLOB_LEFT = 1,
  GREEN_BLOB_RIGHT = 2,
  GREEN_BLOB_BACK_LEFT = 3,
  GREEN_BLOB_BACK_RIGHT = 4,
};

class Enemy : public Actor
{
protected:
  int m_pHealth;
  int m_maxHealth;
  POINT m_ptTargetVelocity;
  int m_speed;
  Player* m_pTarget;
  AStar m_aStar;
  EnemyType m_type;
  POINT m_destination{ 0,0 };
  EnemyState m_state;
  long long m_lastPositionUpdateTime;
  POINT m_lastPosition;
  int m_iDamageCooldown;
  int m_enemySize;
  int m_iAbilityTimer;
public:
  static Bitmap* m_bmpBullet;
  static void SetBulletBitmap(Bitmap* pBulletBitmap) { m_bmpBullet = pBulletBitmap; };
  Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel, EnemyType type, Player* pTarget);
  void 	Enemy::SetTargetVelocity(POINT ptVelocity) { m_ptTargetVelocity = ptVelocity; };
  POINT Enemy::GetTargetVelocity() { return m_ptTargetVelocity; };
  EnemyType Enemy::GetEnemyType() { return m_type; };
  void Enemy::UpdateState();
  void Enemy::Move();
  void 	Enemy::UpdateVelocity();
  //void Enemy::ChangeBitmap();
  void Enemy::SetAbilityTimer(int iAbilityTimer) { m_iAbilityTimer = iAbilityTimer; };
  void Enemy::DealDamage(int iDamage);
  int Enemy::GetMaxHealth() { return m_maxHealth; };
  int Enemy::GetHealth() { return m_pHealth; };
  void Enemy::SetHealth(int iHealth) { m_pHealth = iHealth; };
  int Enemy::GetDamageCooldown() { return m_iDamageCooldown; };
  SPRITEACTION Enemy::Update() override;
private:
  POINT FindNextDestination();
  void HandleStuck();
  long long GetCurrentTimeMillis();
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
  void Rock::SetNumHits(int iNumHits) { m_iNumHits = iNumHits; };
  SPRITEACTION Rock::Update() override;
};

//-----------------------------------------------------------------
// Fireball Class
//-----------------------------------------------------------------
class Fireball : public Actor
{
protected:
  bool m_bEnemy;
  bool m_bParried;
public:
  Fireball::Fireball(Bitmap* bmpBitmap, Level* pLevel);
  bool isEnemy() { return m_bEnemy; };
  void setEnemy(bool bEnemy) { m_bEnemy = bEnemy; };
  void parry() { m_bParried = true; };
  bool isParried() { return m_bParried; };
  bool AmIStuck() override;
  SPRITEACTION Fireball::Update() override;
};

//-----------------------------------------------------------------
// Flame Class
//-----------------------------------------------------------------
class Flame : public Actor
{
protected:
  int m_iTime;
  bool m_bCloned;
  int m_iCloneAgain;
  int m_iCloneDepth;
public:
  static Bitmap* m_pFlameBitmap;
  static void setFlameBitmap(Bitmap* pFlameBitmap) { m_pFlameBitmap = pFlameBitmap; };
  void SetTime(int iTime) { m_iTime = iTime; };
  void SubtractTime(int iTime) { m_iTime -= iTime; };
  void SetCloned(bool bCloned) { m_bCloned = bCloned; };
  bool IsCloned() { return m_bCloned; };
  void SetCloneDepth(int iCloneDepth) { m_iCloneDepth = iCloneDepth; };
  int GetCloneDepth() { return m_iCloneDepth; };
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
  int m_iSpreadCooldown;
public:
  Mud::Mud(Bitmap* bmpBitmap, Level* pLevel);
  int getSpreadCooldown() { return m_iSpreadCooldown; };
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

//-----------------------------------------------------------------
// Ice Class
//-----------------------------------------------------------------
class Ice : public Actor
{
protected:
  int m_iTime;
public:
  Ice::Ice(Bitmap* bmpBitmap, Level* pLevel);
  Ice::~Ice();
  SPRITEACTION Ice::Update() override;
  void Ice::SetPositionFromCenter(POINT ptPosition) override;
};

//-----------------------------------------------------------------
// Orb Class
//-----------------------------------------------------------------

enum OrbType : int {
    ORB_HEALTH,
    ORB_FIRE,
    ORB_WATER,
    ORB_EARTH,
    ORB_AIR
};

class Orb : public Actor
{
private:
    OrbType orbType;
public:
    Orb::Orb(Bitmap* bmpBitmap, Level* pLevel, OrbType type);
    OrbType Orb::GetType() { return orbType; };
};