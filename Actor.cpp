//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"
#include "GameEngine.h"
#include "resource.h"
#include <chrono>
#include <random>

// Forward decl.
Enemy* CreateEnemy(EnemyType type);

GameEngine* Actor::s_pGame = nullptr;

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
  if (m_ptVelocity.x > 0)
  {
    // We are moving right, do we collide with the right wall?

    // X axis check
    int nextPositionX = m_rcPosition.right + m_ptVelocity.x;

    // Y axis check
    int nextPositionUpperY = m_rcPosition.top;
    int nextPositionMiddleY = (m_rcPosition.top + m_rcPosition.bottom) / 2;
    int nextPositionBottomY = m_rcPosition.bottom;

    if (m_pLevel->m_layout[nextPositionUpperY / 32][nextPositionX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionMiddleY / 32][nextPositionX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionBottomY / 32][nextPositionX / 32] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }
  else if (m_ptVelocity.x < 0)
  {
    // We are moving left, do we collide with the left wall?

    // X axis check
    int nextPositionX = m_rcPosition.left + m_ptVelocity.x;

    // Y axis check
    int nextPositionUpperY = m_rcPosition.top;
    int nextPositionMiddleY = (m_rcPosition.top + m_rcPosition.bottom) / 2;
    int nextPositionBottomY = m_rcPosition.bottom;

    if (m_pLevel->m_layout[nextPositionUpperY / 32][nextPositionX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionMiddleY / 32][nextPositionX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionBottomY / 32][nextPositionX / 32] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }

  if (m_ptVelocity.y > 0)
  {
    // We are moving down, do we collide with the bottom wall?

    // Y axis check
    int nextPositionY = m_rcPosition.bottom + m_ptVelocity.y;

    // X axis check
    int nextPositionLeftX = m_rcPosition.left;
    int nextPositionMiddleX = (m_rcPosition.left + m_rcPosition.right) / 2;
    int nextPositionRightX = m_rcPosition.right;

    if (m_pLevel->m_layout[nextPositionY / 32][nextPositionLeftX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionY / 32][nextPositionMiddleX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionY / 32][nextPositionRightX / 32] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }
  else if (m_ptVelocity.y < 0)
  {
    // We are moving up, do we collide with the top wall?

    // Y axis check
    int nextPositionY = m_rcPosition.top + m_ptVelocity.y;

    // X axis check
    int nextPositionLeftX = m_rcPosition.left;
    int nextPositionMiddleX = (m_rcPosition.left + m_rcPosition.right) / 2;
    int nextPositionRightX = m_rcPosition.right;

    if (m_pLevel->m_layout[nextPositionY / 32][nextPositionLeftX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionY / 32][nextPositionMiddleX / 32] != 0 ||
        m_pLevel->m_layout[nextPositionY / 32][nextPositionRightX / 32] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }

  SPRITEACTION out = Sprite::Update();

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
Player::Player(Bitmap* bmpBitmap, Level* pLevel) : Actor(bmpBitmap, pLevel)
{
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_iMaxHealth = 100;
  m_iCurrentHealth = m_iMaxHealth;
  m_iInvFrames = 0;

  m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player General Methods
//-----------------------------------------------------------------
void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x)
    m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 1);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x)
    m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 1);

  if (m_ptTargetVelocity.y < m_ptVelocity.y)
    m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 1);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y)
    m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 1);

  static constexpr int MAX_VELOCITY = 20;

  // Cap player speed to 50
  if (m_ptVelocity.x < 0)
  {
    m_ptVelocity.x = max(m_ptVelocity.x, -MAX_VELOCITY);
  }
  else
  {
    m_ptVelocity.x = min(m_ptVelocity.x, MAX_VELOCITY);
  }
  if (m_ptVelocity.y < 0)
  {
    m_ptVelocity.y = max(m_ptVelocity.y, -MAX_VELOCITY);
  }
  else
  {
    m_ptVelocity.y = min(m_ptVelocity.y, MAX_VELOCITY);
  }
}

void Player::SubtractHealth(int value)
{
  if (m_iCurrentHealth > 0 && m_iInvFrames <= 0)
  {
    m_iInvFrames = 10;
    m_iCurrentHealth = m_iCurrentHealth - value > 0 ? m_iCurrentHealth - value : 0;
  }
}

SPRITEACTION Player::Update()
{
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();

  /*

  POINT ptPlayerCenterPos = GetPositionFromCenter();

  POINT ptMouseOffset = POINT{ m_ptMousePos.x - ptPlayerCenterPos.x, m_ptMousePos.y - ptPlayerCenterPos.y };

  if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
  {
    SetState(PLR_DOWN);
    //DOWN
  }
  if (ptMouseOffset.y >= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
  {
    SetState(PLR_LEFT);
    //LEFT
  }
  if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y >= -ptMouseOffset.x)
  {
    SetState(PLR_RIGHT);
    //RIGHT
  }
  if (ptMouseOffset.y <= ptMouseOffset.x && ptMouseOffset.y <= -ptMouseOffset.x)
  {
    SetState(PLR_UP);
    //UP
  }
  */

  if (m_iInvFrames > 0) m_iInvFrames--;

  return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection) : Actor(bmpBitmap, pLevel)
{
  m_iActiveTime = 2;
  m_ptDirection = ptDirection;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------

Bitmap* Enemy::m_bmpBullet = nullptr;

SPRITEACTION Swing::Update()
{
  SPRITEACTION out = Sprite::Update();

  if (m_iActiveTime-- <= 0)
  {
    return SA_KILL;
  }
  return out;
}

//-----------------------------------------------------------------
// Enemy Constructor(s)/Destructor
//-----------------------------------------------------------------
Enemy::Enemy(Bitmap* bmpBitmap, Level* pLevel, EnemyType type, Player* pTarget)
  : Actor(bmpBitmap, pLevel), m_type(type), m_pTarget(pTarget), m_speed(0)
  , m_state(EnemyState::PATROLLING)
  , m_lastPosition(GetPositionFromCenter())
  , m_lastPositionUpdateTime(GetCurrentTimeMillis())
{
  m_aStar = AStar(pLevel);
  m_ptTargetVelocity = POINT{ 0,0 };
  m_iState = 0;
  m_iSize = 24;
  m_iDamageCooldown = 0;
  m_pSpriteStates[0] = bmpBitmap;
  m_destination = FindNextDestination();
  switch (type)
  {
    case EnemyType::FIRE_SKULL:
    {
      m_iAbilityTimer = 30;
      m_speed = 3;
      m_pHealth = 100;
      SetZOrder(7);
      SetNumFrames(4);
      SetFrameDelay(10);
      break;
    }
    case EnemyType::GREEN_BLOB:
    {
      m_enemySize = 24;
      m_speed = 2;
      m_pHealth = 500;
      SetZOrder(7);
      SetNumFrames(4);
      SetFrameDelay(10);
      break;
    }
    case EnemyType::HUMONGUS:
    {
      m_enemySize = 32;
      m_speed = 1;
      m_pHealth = 1000;
      SetZOrder(7);
      SetNumFrames(4);
      SetFrameDelay(50);
      break;
    }
    case EnemyType::DEAD_EYE:
    {
      m_enemySize = 20;
      m_speed = 4;
      m_pHealth = 150;
      SetZOrder(7);
      SetNumFrames(3);
      SetFrameDelay(10);
      break;
    }
  }
}

//-----------------------------------------------------------------
// Enemy General Methods
//-----------------------------------------------------------------
/*
void Enemy::ChangeBitmap()
{
  switch (m_type)
  {
    case EnemyType::FIRE_SKULL:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(255, 75, 75));
      break;
    }
    case EnemyType::GREEN_BLOB:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(255, 75, 75));
      break;
    }
    case EnemyType::HUMONGUS:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 255, 75));
      break;
    }
    case EnemyType::DEAD_EYE:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 255, 75));
      break;
    }
  }
}
*/

void Enemy::HandleStuck()
{
  if (GetCurrentTimeMillis() < m_lastPositionUpdateTime + 500)
  {
    return;
  }

  POINT currentPosition = GetPositionFromCenter();

  if (m_lastPosition.x == currentPosition.x
      && m_lastPosition.y == currentPosition.y)
  {
    if (m_type != EnemyType::HUMONGUS)
    {
      m_destination = FindNextDestination();
      m_state = EnemyState::PATROLLING;
    }
    else
    {
      // Break the wall
      RECT ptPosition = GetPosition();

      // Find direction and break if wall
      if (m_ptVelocity.x > 0)
      {
        // We are moving right, break the right wall
        int nextPositionX = ptPosition.right + m_ptVelocity.x;
        int nextPositionY = ptPosition.top;

        if (m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] != 0)
        {
          m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] = 0;
        }
      }
      else
      {
        // We are moving left, break the left wall
        int nextPositionX = ptPosition.left + m_ptVelocity.x;
        int nextPositionY = ptPosition.top;

        if (m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] != 0)
        {
          m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] = 0;
        }
      }

      if (m_ptVelocity.y > 0)
      {
        // We are moving down, break the bottom wall
        int nextPositionX = ptPosition.left;
        int nextPositionY = ptPosition.bottom + m_ptVelocity.y;

        if (m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] != 0)
        {
          m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] = 0;
        }
      }
      else
      {
        // We are moving up, break the top wall
        int nextPositionX = ptPosition.left;
        int nextPositionY = ptPosition.top + m_ptVelocity.y;

        if (m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] != 0)
        {
          m_pLevel->m_layout[nextPositionY / 32][nextPositionX / 32] = 0;
        }
      }
    }
  }
}

void Enemy::UpdateState()
{
  if (m_type == EnemyType::FIRE_SKULL)
  {
    if (m_ptVelocity.x > 0)
    {
      SetState(FireSkullState::FIRE_SKULL_RIGHT);
    }
    else
    {
      SetState(FireSkullState::FIRE_SKULL_LEFT);
    }
  }
  else if (m_type == EnemyType::DEAD_EYE)
  {
    if (m_ptVelocity.x > 0)
    {
      if (m_ptVelocity.y > 0)
      {
        SetState(GreenBlobState::GREEN_BLOB_RIGHT);
      }
      else
      {
        SetState(GreenBlobState::GREEN_BLOB_BACK_RIGHT);
      }
    }
    else
    {
      if (m_ptVelocity.y > 0)
      {
        SetState(GreenBlobState::GREEN_BLOB_LEFT);
      }
      else
      {
        SetState(GreenBlobState::GREEN_BLOB_BACK_LEFT);
      }
    }
  }

  if (m_state == EnemyState::PATROLLING)
  {
    // Check distance to target
    POINT playerPos = m_pTarget->GetPositionFromCenter();
    POINT enemyPos = GetPositionFromCenter();
    int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
    if (distance <= 128)
    {
      if (m_type == EnemyType::DEAD_EYE)
      {
        m_state = EnemyState::ESCAPING;
      }
      else
      {
        m_state = EnemyState::ATTACKING;
      }
    }
  }
  else if (m_state == EnemyState::ATTACKING)
  {
    if (m_type != EnemyType::FIRE_SKULL)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 128)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
      }
    }
  }
  else if (m_state == EnemyState::ESCAPING)
  {
    if (m_type == EnemyType::DEAD_EYE)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 128)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
      }
    }
  }
}

void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 1);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 1);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 1);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 1);
}

POINT Enemy::FindNextDestination()
{
  POINT dest{ 0,0 };
  bool impossiblePosition{ true };
  while (impossiblePosition)
  {
    dest = POINT{
            static_cast<long>((size_t)rand() % m_pLevel->m_layout[0].size()),
            static_cast<long>((size_t)rand() % m_pLevel->m_layout.size())
    };

    // Check if the destination is a wall
    impossiblePosition = m_pLevel->m_layout[dest.y][dest.x] != 0;
  }
  return dest;
}

void Enemy::Move()
{
  HandleStuck();

  // Which node am I on?
  POINT startNode = m_pLevel->GetNodeFromPosition(GetPositionFromCenter());

  // Which node is target on?
  POINT endNode{ 0,0 };
  if (m_state == EnemyState::PATROLLING)
    endNode = m_destination;
  else if (m_state == EnemyState::ATTACKING)
    endNode = m_pLevel->GetNodeFromPosition(m_pTarget->GetPositionFromCenter());
  else if (m_state == EnemyState::ESCAPING)
    endNode = m_pLevel->GetNodeFromPosition(m_pTarget->GetPositionFromCenter());

  // Find path
  std::vector<POINT> path = m_aStar.findPath(startNode, endNode);

  if (path.empty())
  {
    if (m_state == EnemyState::PATROLLING)
    {
      m_destination = FindNextDestination();
    }
    return;
  }

  POINT nextNode = path[0];

  // Calculate x, y distances to the end node from start node
  int xDistance = nextNode.x - startNode.x;
  int yDistance = nextNode.y - startNode.y;

  // Give the m_speed to bigger component and adjust the other component accordingly
  if (abs(xDistance) > abs(yDistance))
  {
    m_ptTargetVelocity.x = m_speed * (xDistance / abs(xDistance));
    m_ptTargetVelocity.y = m_speed * (yDistance / abs(xDistance));
  }
  else
  {
    m_ptTargetVelocity.x = m_speed * (xDistance / abs(yDistance));
    m_ptTargetVelocity.y = m_speed * (yDistance / abs(yDistance));
  }

  // Reverse if enemy escaping
  if (m_state == EnemyState::ESCAPING)
  {
    m_ptTargetVelocity.x = -m_ptTargetVelocity.x;
    m_ptTargetVelocity.y = -m_ptTargetVelocity.y;
  }
}

void Enemy::DealDamage(int iDamage)
{
  if (m_iDamageCooldown <= 0)
  {
    m_iDamageCooldown = 8;
    m_pHealth -= iDamage;
    if (m_pHealth < 0) Kill();
  }
}

SPRITEACTION Enemy::Update()
{
  UpdateState();
  Move();
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();

  if (m_pTarget != NULL)
  {
    if (TestCollision(m_pTarget))
    {
      m_pTarget->SubtractHealth(10);
    }
  }

  if (m_iDamageCooldown > 0)
  {
    m_iDamageCooldown--;
  }

  // Update last position
  POINT currentPosition = GetPositionFromCenter();

  if (m_lastPosition.x != currentPosition.x || m_lastPosition.y != currentPosition.y)
  {
    m_lastPosition = currentPosition;
    m_lastPositionUpdateTime = GetCurrentTimeMillis();
  }

  if (--m_iAbilityTimer <= 0)
  {
    if (m_type == EnemyType::FIRE_SKULL)
    {
      if (m_state == EnemyState::ATTACKING)
      {
        Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
        pFlame->SetNumFrames(3);
        pFlame->SetPositionFromCenter(GetPositionFromCenter());
        pFlame->SetTime(40);
        GameEngine::GetEngine()->AddSprite(pFlame);
        m_iAbilityTimer = 15;
      }
      else
      {
        m_iAbilityTimer = 0;
      }
    }
    if (m_type == EnemyType::DEAD_EYE)
    {
      if (m_state == EnemyState::PATROLLING)
      {
        m_iAbilityTimer = 0;
      }
      else
      {
        Fireball* pBullet = new Fireball(Enemy::m_bmpBullet, m_pLevel);
        POINT ptTargetPos = m_pTarget->GetPositionFromCenter();
        POINT ptEnemyPos = GetPositionFromCenter();
        POINT ptVelocity = POINT{ ptTargetPos.x - ptEnemyPos.x, ptTargetPos.y - ptEnemyPos.y };

        float multiplier = 10 / sqrt((ptVelocity.x * ptVelocity.x) + (ptVelocity.y * ptVelocity.y));
        pBullet->setEnemy(true);
        pBullet->SetPositionFromCenter(ptEnemyPos);
        pBullet->SetVelocity(ptVelocity.x * multiplier, ptVelocity.y * multiplier);
        pBullet->SetNumFrames(2);
        pBullet->SetFrameDelay(1);

        GameEngine::GetEngine()->AddSprite(pBullet);

        m_iAbilityTimer = 20;
      }
    }
    if (m_type == EnemyType::GREEN_BLOB)
    {
      if (m_state == EnemyState::ATTACKING)
      {
        if (m_pHealth >= 100)
        {
          m_pHealth = m_pHealth / 2;
          Enemy* enemy = CreateEnemy(EnemyType::GREEN_BLOB);
          enemy->SetPositionFromCenter(GetPositionFromCenter().x + (rand() % 50) - 25, GetPositionFromCenter().y + (rand() % 50) - 25);
          enemy->SetAbilityTimer(100);
          enemy->SetHealth(m_pHealth / 2);
          m_iAbilityTimer = 100;
        }
      }
      else
      {
        m_iAbilityTimer = 0;
      }
    }
    if (m_type == EnemyType::HUMONGUS)
    {
      if (m_state == EnemyState::ATTACKING)
      {
        // Set Random device
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> dis(0, 3072);
        EnemyType type = { static_cast<EnemyType>(dis(gen) % 4) };

        Enemy* enemy = CreateEnemy(type);
        enemy->SetPosition(GetPositionFromCenter());
        enemy->SetVelocity(m_pTarget->GetPositionFromCenter().x - GetPositionFromCenter().x, m_pTarget->GetPositionFromCenter().y - GetPositionFromCenter().y);

        m_iAbilityTimer = 500;
      }
      else
      {
        m_iAbilityTimer = 0;
      }
    }
  }

  return out;
}

long long Enemy::GetCurrentTimeMillis()
{
  auto now = std::chrono::steady_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto epoch = now_ms.time_since_epoch();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
  return milliseconds.count();
}

//-----------------------------------------------------------------
// Rock Constructor(s)/Destructor
//-----------------------------------------------------------------
Rock::Rock(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iNumHits = 0;
  m_iMaxHits = 3;
}


//-----------------------------------------------------------------
// Rock General Methods
//-----------------------------------------------------------------
void Rock::UpdateVelocity()
{
  if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

  float multiplier = 2 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

  int decrease_x = (m_ptVelocity.x * multiplier);
  int decrease_y = (m_ptVelocity.y * multiplier);

  if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
  else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

  if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
  else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

SPRITEACTION Rock::Update()
{
  SPRITEACTION out = Sprite::Update();
  UpdateVelocity();

  if (m_iCooldown > 0) m_iCooldown--;

  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top });
  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top });
  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom });
  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom });

  if ((!bTopLeft && !bTopRight) || (!bBottomLeft && !bBottomRight))
  {
    m_ptVelocity.y = -m_ptVelocity.y;
    POINT position = GetPositionFromCenter();
    position.y += m_ptVelocity.y;
    SetPositionFromCenter(position);
  }
  if ((!bTopLeft && !bBottomLeft) || (!bTopRight && !bBottomRight))
  {
    m_ptVelocity.x = -m_ptVelocity.x;
    POINT position = GetPositionFromCenter();
    position.x += m_ptVelocity.x;
    SetPositionFromCenter(position);
  }

  return out;
}

//-----------------------------------------------------------------
// Fireball Constructor(s)/Destructor
//-----------------------------------------------------------------
Fireball::Fireball(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_bEnemy = 0;
  m_bParried = 0;
}

Bitmap* Flame::m_pFlameBitmap = nullptr;

//-----------------------------------------------------------------
// Fireball General Methods
//-----------------------------------------------------------------
SPRITEACTION Fireball::Update()
{

  SPRITEACTION out = Sprite::Update();

  if ((!m_bEnemy && Fireball::AmIStuck()) || (m_bEnemy && Actor::AmIStuck()))
  {
    if (!m_bEnemy)
    {
      Flame* pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
      pFlame->SetNumFrames(3);
      pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
      pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 10) - 5, (-GetVelocity().y / 3) + (rand() % 10) - 5);
      GameEngine::GetEngine()->AddSprite(pFlame);
      pFlame = new Flame(Flame::m_pFlameBitmap, m_pLevel);
      pFlame->SetNumFrames(3);
      pFlame->SetPositionFromCenter(GetPositionFromCenter().x - GetVelocity().x, GetPositionFromCenter().y - GetVelocity().y);
      pFlame->SetVelocity((-GetVelocity().x / 3) + (rand() % 10) - 5, (-GetVelocity().y / 3) + (rand() % 10) - 5);
      GameEngine::GetEngine()->AddSprite(pFlame);
    }
    return SA_KILL;
  }

  return out;
}

bool Fireball::AmIStuck()
{
  BOOL bTopLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.top }) ||
    m_pLevel->IsPointMeltable(POINT{ m_rcPosition.left, m_rcPosition.top });

  BOOL bTopRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.top }) ||
    m_pLevel->IsPointMeltable(POINT{ m_rcPosition.right, m_rcPosition.top });

  BOOL bBottomLeft = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.left, m_rcPosition.bottom }) ||
    m_pLevel->IsPointMeltable(POINT{ m_rcPosition.left, m_rcPosition.bottom });

  BOOL bBottomRight = m_pLevel->IsPointCollidable(POINT{ m_rcPosition.right, m_rcPosition.bottom }) ||
    m_pLevel->IsPointMeltable(POINT{ m_rcPosition.right, m_rcPosition.bottom });


  return (!bTopLeft || !bTopRight || !bBottomLeft || !bBottomRight);
}


//-----------------------------------------------------------------
// Flame Constructor(s)/Destructor
//-----------------------------------------------------------------
Flame::Flame(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iTime = (rand() % 40) + 80;
  m_bCloned = false;
  m_iCloneAgain = 0;
  m_iCloneDepth = 0;
}

//-----------------------------------------------------------------
// Flame General Methods
//-----------------------------------------------------------------
SPRITEACTION Flame::Update()
{

  SPRITEACTION out = Actor::Update();
  UpdateVelocity();

  if (--m_iTime < 0)
  {
    return SA_KILL;
  }

  if (m_bCloned)
  {
    if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0)
    {
      m_iCloneAgain++;
      if (m_iCloneAgain > 2)
      {
        m_bCloned = false;
        m_iCloneAgain = 0;
      }
    }
    else
    {
      m_iCloneAgain = 0;
    }
  }

  return out;
}

void Flame::UpdateVelocity()
{
  if (m_ptVelocity.x == 0 && m_ptVelocity.y == 0) return;

  float multiplier = 2 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

  int decrease_x = (m_ptVelocity.x * multiplier);
  int decrease_y = (m_ptVelocity.y * multiplier);

  if (0 < m_ptVelocity.x) m_ptVelocity.x = max(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));
  else if (0 > m_ptVelocity.x) m_ptVelocity.x = min(0, m_ptVelocity.x - (m_ptVelocity.x * multiplier));

  if (0 < m_ptVelocity.y) m_ptVelocity.y = max(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
  else if (0 > m_ptVelocity.y) m_ptVelocity.y = min(0, m_ptVelocity.y - (m_ptVelocity.y * multiplier));
}

//-----------------------------------------------------------------
// Puddle Constructor(s)/Destructor
//-----------------------------------------------------------------
Puddle::Puddle(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iTime = 100;
}

//-----------------------------------------------------------------
// Puddle General Methods
//-----------------------------------------------------------------
SPRITEACTION Puddle::Update()
{

  SPRITEACTION out = Sprite::Update();

  if (--m_iTime < 0)
  {
    return SA_KILL;
  }

  return out;
}

//-----------------------------------------------------------------
// Mud Constructor(s)/Destructor
//-----------------------------------------------------------------
Mud::Mud(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iTime = 200;
  m_iSpreadCooldown = 3;
}

//-----------------------------------------------------------------
// Mud General Methods
//-----------------------------------------------------------------
SPRITEACTION Mud::Update()
{

  SPRITEACTION out = Sprite::Update();

  if (--m_iTime < 0)
  {
    return SA_KILL;
  }

  if (m_iSpreadCooldown > 0)
  {
    m_iSpreadCooldown--;
  }

  return out;
}


//-----------------------------------------------------------------
// Gust Constructor(s)/Destructor
//-----------------------------------------------------------------
Gust::Gust(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iTime = 10;
}


//-----------------------------------------------------------------
// Gust General Methods
//-----------------------------------------------------------------
SPRITEACTION Gust::Update()
{

  SPRITEACTION out = Sprite::Update();

  if (!m_pLevel->IsPointCollidable(GetPositionFromCenter())) return SA_KILL;

  if (--m_iTime < 0)
  {
    return SA_KILL;
  }

  return out;
}


//-----------------------------------------------------------------
// Ice Constructor(s)/Destructor
//-----------------------------------------------------------------
Ice::Ice(Bitmap* _bmpBitmap, Level* pLevel) : Actor(_bmpBitmap, pLevel)
{
  m_iTime = 300;
}

Ice::~Ice()
{
  m_pLevel->SetTile(m_pLevel->GetNodeFromPosition(GetPositionFromCenter()), 0);
}


//-----------------------------------------------------------------
// Ice General Methods
//-----------------------------------------------------------------
SPRITEACTION Ice::Update()
{

  SPRITEACTION out = Sprite::Update();

  if (--m_iTime < 0)
  {
    return SA_KILL;
  }

  return out;
}

void Ice::SetPositionFromCenter(POINT ptPosition)
{
  Sprite::SetPositionFromCenter(ptPosition);
  m_pLevel->SetTile(m_pLevel->GetNodeFromPosition(ptPosition), 2);
}

