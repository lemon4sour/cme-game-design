//-----------------------------------------------------------------
// Actor Object
// C++ Source - Actor.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Actor.h"
#include "cassert"
#include <chrono>

std::vector<Sprite*>* Actor::_vcSprites = nullptr;

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
  // Because we are using integer instead of float, we should add a buffer to the collision detection
  constexpr int BUFFER{ 8 };

  if (m_ptVelocity.x > 0)
  {
    // We are moving right, do we collide with the right wall?
    POINT nextPosition{ m_rcPosition.right + m_ptVelocity.x + BUFFER, m_rcPosition.top };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }
  else if (m_ptVelocity.x < 0)
  {
    // We are moving left, do we collide with the left wall?
    POINT nextPosition{ m_rcPosition.left + m_ptVelocity.x - BUFFER, m_rcPosition.top };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the x velocity
      m_ptVelocity.x = 0;
    }
  }

  if (m_ptVelocity.y > 0)
  {
    // We are moving down, do we collide with the bottom wall?
    POINT nextPosition{ m_rcPosition.left, m_rcPosition.bottom + m_ptVelocity.y + BUFFER };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
    {
      // Yes, we do collide, zero out the y velocity
      m_ptVelocity.y = 0;
    }
  }
  else if (m_ptVelocity.y < 0)
  {
    // We are moving up, do we collide with the top wall?
    POINT nextPosition{ m_rcPosition.left, m_rcPosition.top + m_ptVelocity.y - BUFFER };
    if (m_pLevel->m_layout[nextPosition.y / 128][nextPosition.x / 128] != 0)
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

  m_pSpriteStates[0] = bmpBitmap;
}

//-----------------------------------------------------------------
// Player General Methods
//-----------------------------------------------------------------
void Player::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x)
    m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x)
    m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y)
    m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y)
    m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
}

void Player::SubtractHealth(int value)
{
  if (m_iCurrentHealth > 0)
  {
    m_iCurrentHealth = m_iCurrentHealth - value > 0 ? m_iCurrentHealth - value : 0;
  }
}

SPRITEACTION Player::Update()
{
  UpdateVelocity();
  SPRITEACTION out = Actor::Update();
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

  return out;
}

//-----------------------------------------------------------------
// Swing Constructor(s)/Destructor
//-----------------------------------------------------------------
Swing::Swing(Bitmap* bmpBitmap, Level* pLevel, POINT ptDirection) : Actor(bmpBitmap, pLevel)
{
  m_iActiveTime = 3;
  m_ptDirection = ptDirection;
}

//-----------------------------------------------------------------
// Swing General Methods
//-----------------------------------------------------------------
SPRITEACTION Swing::Update()
{
  SPRITEACTION out = Actor::Update();
  for (Sprite* sprite : (*_vcSprites))
  {
    Enemy* enemy = dynamic_cast<Enemy*>(sprite);
    if (enemy)
    {
      if (enemy->TestCollision(this))
      {
        enemy->Kill();
      }
      continue;
    }
    Rock* rock = dynamic_cast<Rock*>(sprite);
    if (rock)
    {
      if (rock->GetCooldown() > 0) continue;
      rock->SetCooldown(3);
      if (rock->TestCollision(this))
      {
        int hits = rock->GetNumHits();
        if (hits >= rock->GetMaxHits())
        {
          rock->Kill();
          continue;
        }
        rock->GetNumHits(++hits);

        if (m_ptDirection.x == 0)
        {
          rock->SetVelocity((rock->GetPositionFromCenter().x - GetPositionFromCenter().x) + (rand() % 17) - 9, m_ptDirection.y * 100);
        }
        else
        {
          rock->SetVelocity(m_ptDirection.x * 100, (rock->GetPositionFromCenter().y - GetPositionFromCenter().y) + (rand() % 17) - 9);
        }
      }
      continue;
    }
  }
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
  m_pSpriteStates[0] = bmpBitmap;
  m_destination = FindNextDestination();
  switch (type)
  {
    case EnemyType::ANGRY_GUY:
    {
      m_enemySize = 28;
      m_speed = 10;
      break;
    }
    case EnemyType::DUTY_GUY:
    {
      m_enemySize = 24;
      m_speed = 8;
      break;
    }
    case EnemyType::HEAVY_GUY:
    {
      m_enemySize = 32;
      m_speed = 5;
      break;
    }
    case EnemyType::COWARD_GUY:
    {
      m_enemySize = 20;
      m_speed = 11;
      break;
    }
  }
  ChangeBitmap();
}

//-----------------------------------------------------------------
// Enemy General Methods
//-----------------------------------------------------------------
void Enemy::ChangeBitmap()
{
  switch (m_state)
  {
    case EnemyState::PATROLLING:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 75, 255));
      break;
    }
    case EnemyState::ATTACKING:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(255, 75, 75));
      break;
    }
    case EnemyState::ESCAPING:
    {
      GetBitmap()->Create(m_enemySize, m_enemySize, RGB(75, 255, 75));
      break;
    }
  }
}

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
    m_destination = FindNextDestination();
    m_state = EnemyState::PATROLLING;
    ChangeBitmap();
  }
}

void Enemy::UpdateState()
{
  if (m_state == EnemyState::PATROLLING)
  {
    // Check distance to target
    POINT playerPos = m_pTarget->GetPositionFromCenter();
    POINT enemyPos = GetPositionFromCenter();
    int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
    if (distance <= 512)
    {
      if (m_type == EnemyType::COWARD_GUY)
      {
        m_state = EnemyState::ESCAPING;
      }
      else
      {
        m_state = EnemyState::ATTACKING;
      }
      ChangeBitmap();
    }
  }
  else if (m_state == EnemyState::ATTACKING)
  {
    if (m_type != EnemyType::ANGRY_GUY)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 512)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
        ChangeBitmap();
      }
    }
  }
  else if (m_state == EnemyState::ESCAPING)
  {
    if (m_type == EnemyType::COWARD_GUY)
    {
      // Check distance to target
      POINT playerPos = m_pTarget->GetPositionFromCenter();
      POINT enemyPos = GetPositionFromCenter();
      int distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.y - enemyPos.y, 2));
      if (distance > 512)
      {
        // Set last known position as target
        m_destination = m_pLevel->GetNodeFromPosition(playerPos);
        m_state = EnemyState::PATROLLING;
        ChangeBitmap();
      }
    }
  }
}

void Enemy::UpdateVelocity()
{
  if (m_ptTargetVelocity.x < m_ptVelocity.x) m_ptVelocity.x = max(m_ptTargetVelocity.x, m_ptVelocity.x - 5);
  else if (m_ptTargetVelocity.x > m_ptVelocity.x) m_ptVelocity.x = min(m_ptTargetVelocity.x, m_ptVelocity.x + 5);

  if (m_ptTargetVelocity.y < m_ptVelocity.y) m_ptVelocity.y = max(m_ptTargetVelocity.y, m_ptVelocity.y - 5);
  else if (m_ptTargetVelocity.y > m_ptVelocity.y) m_ptVelocity.y = min(m_ptTargetVelocity.y, m_ptVelocity.y + 5);
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
      m_pTarget->SubtractHealth(1);
    }
  }

  // Update last position
  POINT currentPosition = GetPositionFromCenter();

  if (m_lastPosition.x != currentPosition.x || m_lastPosition.y != currentPosition.y)
  {
    m_lastPosition = currentPosition;
    m_lastPositionUpdateTime = GetCurrentTimeMillis();
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

  float multiplier = 8 / sqrt((m_ptVelocity.x * m_ptVelocity.x) + (m_ptVelocity.y * m_ptVelocity.y));

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